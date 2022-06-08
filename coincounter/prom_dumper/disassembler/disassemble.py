import opmap, ast
import binascii
from collections import deque

def main():
    with open("../bin1.bin", "rb") as f:
        image = f.read()

    # extract our sections
    sections = []
    sections.append(opmap.decode_vector_table(image))
    sections.append(opmap.decode_geti_table(image))
    sections.append(opmap.decode_code(image, ".text", 0x80, 0x1000))
    sections.append(opmap.decode_data(image, ".data", 0x1000, 0x1080))
    sections.append(opmap.decode_code(image, ".text2", 0x1080, 0x10A8))
    sections.append(opmap.decode_data(image, ".data2", 0x10A8, 0x10B8))


    analysis = Analysis()
    analysis.resolve_branch_dests(sections)
    analysis.name_memory(sections)
    analysis.name_jumps(sections)
    analysis.identify_basic_blocks(sections[2], sections[4])
    analysis.build_call_graph(0x0080, 0x05CB, 0x05E3)

    with open("test.s", "w", encoding="utf-8") as f:
        for section in sections:
            f.write("\n    ; SECTION {}\n\n".format(section.name))
            for i in section.instructions:
                if i.address in analysis.functions:
                    f.write("\n    ; FUNCTION {}".format(analysis.label_names[i.address]))

                    blocks, calls = analysis.functions[i.address]
                    if calls:
                        f.write(" CALLS\n        ; {}".format(",\n        ; ".join(analysis.label_names[c] for c in calls)))

                    f.write("\n")

                elif i.address in analysis.dead_blocks:
                    f.write("\n    ; DEAD BLOCK {}\n".format(analysis.label_names[i.address]))

                f.write("{:04x}: ".format(i.address))
                source = binascii.hexlify(section.get_source(i.address, len(i.opcode)), b" ").decode("latin1")
                while len(source) < 8:
                    source = "   " + source

                f.write(source)
                if i.address in analysis.label_names:
                    f.write(" {:>10}: {}".format(analysis.label_names[i.address], i))
                else:
                    f.write("             {}".format(i))

                if i.address in analysis.dead_instructions:
                    f.write(" ; DEAD")

                f.write("\n")

class Analysis:
    def __init__(self):
        self.branch_dests = set()
        self.call_dests = set()
        self.label_names = {}
        self.basic_blocks = {} # start_addr -> [branches], [calls]
        self.dead_instructions = set()
        self.dead_blocks = {}
        self.functions = {} # entry_point -> [blocks], [calls]


    def resolve_branch_dests(self, sections):
        # analyses the given sections to figure out all the branch and call destinations
        self.call_dests.update(instruction.arguments[0].address for instruction in sections[0].instructions)

        for section in sections:
            for instruction in section.instructions:
                for arg in instruction.arguments:
                    if isinstance(arg, ast.CodeAddress):
                        if not arg.absolute:
                            arg.address += instruction.address
                            arg.absolute = True

                        if instruction.opcode.name in ("CALL", "CALLF", "CALLA"):
                            self.call_dests.add(arg.address)
                        elif instruction.opcode.name in ("BR", "BRA", "BRCB"):
                            self.branch_dests.add(arg.address)

    def name_jumps(self, sections):
        # gives names to all labels used, and applies them to the relevant arguments
        self.label_names = {dest: "lbl_{:04x}".format(dest) for dest in self.branch_dests}
        self.label_names.update({dest: "sub_{:04x}".format(dest) for dest in self.call_dests})
        self.label_names.update(get_known_codelocs())

        for section in sections:
            for instruction in section.instructions:
                for arg in instruction.arguments:
                    if isinstance(arg, ast.CodeAddress):
                        if arg.address in self.label_names:
                            arg.name = self.label_names[arg.address]

    def name_memory(self, sections):
        data_names = get_known_datalocs()

        for section in sections:
            for instruction in section.instructions:
                for arg in instruction.arguments:
                    if isinstance(arg, ast.BitAddr):
                        address = arg.address
                        if address <= 0xFF:
                            address += 0xF00

                        if (address, arg.bit) in data_names:
                            arg.name = data_names[address, arg.bit]
                        elif address in data_names:
                            arg.name = "{}.{}".format(data_names[address], arg.bit)

                    elif isinstance(arg, ast.Address):
                        address = arg.address
                        if address <= 0xFF:
                            address += 0xF00

                        if address in data_names:
                            arg.name = data_names[address]

    def identify_basic_blocks(self, *sections):
        # the task of this section is to split the codebase into a list
        # of basic blocks. A basic block has a single entry point, zero or 
        # more calls, and zero or more exit points

        for section in sections:
            skipping = False
            current_block = None

            for instruction in section.instructions:
                if current_block is None:
                    # we're outside a block, check if we're at the entry point for a new one, if true start it.
                    if instruction.address in self.call_dests or instruction.address in self.branch_dests:
                        current_block = (instruction.address, [], [])
                        skipping = False

                elif instruction.address in self.call_dests or instruction.address in self.branch_dests:
                    # ran into a new block while processing a previous one, so add a fallthrough branch and
                    # start the new one.
                    current_block[1].append(instruction.address)
                    self.basic_blocks[current_block[0]] = (current_block[1], current_block[2])
                    current_block = (instruction.address, [], [])
                    skipping = False

                if current_block is None:
                    # instruction outside of a block
                    self.dead_instructions.add(instruction.address)
                    continue

                # terminating instructions
                if instruction.opcode.name in ("STOP", "HALT", "RET", "RETI", "RETS") and not skipping:
                    self.basic_blocks[current_block[0]] = (current_block[1], current_block[2])
                    current_block = None

                # branches
                elif instruction.opcode.name in ("BRCB", "BR", "BRA"):
                    target = instruction.arguments[0]
                    if skipping:
                        if isinstance(target, ast.CodeAddress):
                            if target.address in self.call_dests:
                                # conditional tail call 
                                current_block[2].append(target.address)

                            else:
                                # conditional branch
                                current_block[1].append(target.address)

                    else:
                        if isinstance(target, ast.CodeAddress):
                            if target.address in self.call_dests:
                                # conditional tail call 
                                current_block[2].append(target.address)

                            else:
                                # unconditional branch
                                current_block[1].append(target.address)

                        self.basic_blocks[current_block[0]] = (current_block[1], current_block[2])
                        current_block = None

                # calls
                elif instruction.opcode.name in ("CALL", "CALLA", "CALLF"):
                    current_block[2].append(instruction.arguments[0].address)

                # handle skipping state
                if current_block and instruction.opcode.name in ("SKE", "SKT", "SKF", "SKTCLR", "ADDS", "SUBS", "INCS", "DECS", "CALL", "CALLA", "CALLF"):
                    skipping = True
                else:
                    skipping = False

            if current_block:
                self.basic_blocks[current_block[0]] = (current_block[1], current_block[2])

    def build_call_graph(self, *entry_points):
        entry_points = deque(entry_points)

        while len(entry_points):
            entry_point = entry_points.pop()
            if entry_point in self.functions:
                continue

            calls_in_function = set()
            blocks_in_function = set()
            blocks_to_traverse = deque([entry_point])

            while blocks_to_traverse:
                current_block_addr = blocks_to_traverse.pop()
                if current_block_addr in blocks_in_function:
                    continue

                if current_block_addr not in self.basic_blocks:
                    print("Error: dead jump to {:04x} found".format(current_block_addr))
                    continue

                blocks_in_function.add(current_block_addr)
                branches, calls = self.basic_blocks[current_block_addr]
                calls_in_function.update(calls)
                blocks_to_traverse.extend(branches)

            entry_points.extend(calls_in_function)
            self.functions[entry_point] = (blocks_in_function, calls_in_function)

        self.dead_blocks = {
            block
            for block in self.basic_blocks.keys()
            if not any(block in blocks for blocks, calls in self.functions.values())
        }


def get_known_codelocs():
    locs = {}

    locs[0x0] = "RESET_LOC"
    locs[0x2] = "INTBT/4_LOC"
    locs[0x4] = "INT0_LOC"
    locs[0x6] = "INT1_LOC"
    locs[0x8] = "INTCSI_LOC"
    locs[0xA] = "INTT0_LOC"
    locs[0xC] = "INTT1_LOC"

    locs[0x20] = "JMPRST"

    locs[0x80] = "RESET"
    locs[0x5CB] = "INTCSI"
    locs[0x5E3] = "INTT0"

    # based on disassembly
    locs[0xd1c] = "assert_eeprom_cs"
    locs[0x50f] = "check_environment"
    locs[0x563] = "initialize_eeprom"
    locs[0x67e] = "initialize_gpio_and_interrupts"
    locs[0x69c] = "reconfigure_timers"
    locs[0x6b1] = "set_0e_to_5"
    locs[0x6b7] = "reset_basic_timer"
    locs[0x6e9] = "get_var_30"
    locs[0xb96] = "sync_eeprom_to_bank_1"
    locs[0xbd8] = "read_eeprom_to_bank_1"
    locs[0xc06] = "validate_eeprom_bank_1"
    locs[0xc20] = "validate_eeprom_single_load"
    locs[0xc51] = "write_eeprom_from_bank_1"
    locs[0xc66] = "write_eeprom_four_writes"
    locs[0xc90] = "send_eeprom_ewen"
    locs[0xc98] = "send_eeprom_ewds"
    locs[0xca7] = "send_eeprom_command"
    locs[0xccd] = "send_eeprom_data"
    locs[0xce9] = "read_eeprom_reply"
    locs[0xd07] = "get_eeprom_symbols"
    locs[0xd2e] = "adc_start_read"
    locs[0xd3b] = "adc_finish_read"

    locs[0xd43] = "really_get_var_30"
    locs[0xd95] = "do_bank1_magic"
    locs[0xe21] = "process_magic_lower"
    locs[0xe62] = "process_magic_upper"
    locs[0xeaa] = "magic_table_add"
    locs[0xed1] = "magic_table_subtract"
    locs[0xef8] = "read_bank1"
    locs[0xeff] = "write_bank1"
    locs[0xf06] = "copy_16bit"
    locs[0xf48] = "add_16bit"
    locs[0xf57] = "subtract_16bit"
    locs[0xf6c] = "increment_xa"

    locs[0x1000] = "data_table_1000"
    locs[0x1078] = "data_indices_1078"
    locs[0x1080] = "load_table_index"
    locs[0x1094] = "load_table"
    locs[0x109f] = "load_from_table_10a8"
    locs[0x10a8] = "data_table_10a8"


    return locs

def get_known_datalocs():
    locs = {}
    # int -> name for nibble/byte accesses, (int, int) -> name for bit accesses

    # locs[0x000]= "A0"
    # locs[0x001]= "X0"
    # locs[0x002]= "L0"
    # locs[0x003]= "H0"
    # locs[0x004]= "E0"
    # locs[0x005]= "D0"
    # locs[0x006]= "C0"
    # locs[0x007]= "B0"

    # locs[0x008]= "A1"
    # locs[0x009]= "X1"
    # locs[0x00A]= "L1"
    # locs[0x00B]= "H1"
    # locs[0x00C]= "E1"
    # locs[0x00D]= "D1"
    # locs[0x00E]= "C1"
    # locs[0x00F]= "B1"

    # locs[0x010]= "A2"
    # locs[0x011]= "X2"
    # locs[0x012]= "L2"
    # locs[0x013]= "H2"
    # locs[0x014]= "E2"
    # locs[0x015]= "D2"
    # locs[0x016]= "C2"
    # locs[0x017]= "B2"

    # locs[0x018]= "A3"
    # locs[0x019]= "X3"
    # locs[0x01A]= "L3"
    # locs[0x01B]= "H3"
    # locs[0x01C]= "E3"
    # locs[0x01D]= "D3"
    # locs[0x01E]= "C3"
    # locs[0x01F]= "B3"

    locs[0xF80]= "SP"

    locs[0xF82]= "RBS"
    locs[0xF83]= "MBS"
    locs[0xF84]= "SBS"
    locs[0xF85]= "BTM"
    locs[0xF86]= "BT"

    locs[0xF8B]= "WDTM"

    locs[0xF98]= "WM"

    locs[0xFA0]= "TM0"
    locs[0xFA2]= "TOE0"
    locs[0xFA4]= "T0"
    locs[0xFA6]= "TMOD0"

    locs[0xFA8]= "TM1"
    locs[0xFAA]= "TOE1"
    locs[0xFAC]= "T1"
    locs[0xFAE]= "TMOD1"

    locs[0xFB0]= "PSW"
    locs[0xFB0, 0]= "RBE"
    locs[0xFB0, 1]= "MBE"
    locs[0xFB0, 2]= "IST0"
    locs[0xFB0, 3]= "IST1"
    locs[0xFB1, 0]= "SK0"
    locs[0xFB1, 1]= "SK1"
    locs[0xFB1, 2]= "SK2"
    locs[0xFB1, 3]= "CY"

    locs[0xFB2]= "IPS"
    locs[0xFB3]= "PCC"
    locs[0xFB4]= "IM0"
    locs[0xFB5]= "IM1"
    locs[0xFB6]= "IM2"
    locs[0xFB7]= "SCC"

    locs[0xFB8]= "INTA"
    locs[0xFB8, 0]= "IRQBT"
    locs[0xFB8, 1]= "IEBT"
    locs[0xFB8, 2]= "IRQ4"
    locs[0xFB8, 3]= "IE4"

    locs[0xFBA]= "INTC"
    locs[0xFBA, 0]= "IRQW"
    locs[0xFBA, 1]= "IEW"

    locs[0xFBC]= "INTE"
    locs[0xFBC, 0]= "IRQT0"
    locs[0xFBC, 1]= "IET0"
    locs[0xFBC, 2]= "IRQT1"
    locs[0xFBC, 3]= "IET1"

    locs[0xFBD]= "INTF"
    locs[0xFBD, 0]= "IRQCSI"
    locs[0xFBD, 1]= "IECSI"

    locs[0xFBE]= "INTG"
    locs[0xFBE, 0]= "IRQ0"
    locs[0xFBE, 1]= "IE0"
    locs[0xFBE, 2]= "IRQ1"
    locs[0xFBE, 3]= "IE1"

    locs[0xFBF]= "INTH"
    locs[0xFBF, 0]= "IRQ2"
    locs[0xFBF, 1]= "IE2"

    locs[0xFC0]= "BSB0"
    locs[0xFC1]= "BSB1"
    locs[0xFC2]= "BSB2"
    locs[0xFC3]= "BSB3"

    locs[0xFCF]= "SOS"

    locs[0xFD0]= "CLOM"
    locs[0xFD8]= "ADM"
    locs[0xFD8, 2]= "EOC"
    locs[0xFD8, 3]= "SOC"
    locs[0xFD9, 0]= "ADM4"
    locs[0xFD9, 1]= "ADM5"
    locs[0xFD9, 2]= "ADM6"
    locs[0xFD9, 3]= "ADEN"

    locs[0xFDA]= "SA"

    locs[0xFDC]= "POGA"

    locs[0xFE0]= "CSIM"
    locs[0xFE1, 3]= "CSIE"

    locs[0xFE2]= "SBC"
    locs[0xFE2, 0]= "RELT"
    locs[0xFE2, 1]= "CMDT"

    locs[0xFE4]= "SIO"

    locs[0xFE8]= "PMGA"
    locs[0xFEC]= "PMGB"

    locs[0xFF0]= "PORT0"
    locs[0xFF0, 1]= "SCKP"
    locs[0xFF1]= "PORT1"
    locs[0xFF2]= "PORT2"
    locs[0xFF3]= "PORT3"
    locs[0xFF4]= "PORT4"
    locs[0xFF5]= "PORT5"
    locs[0xFF6]= "PORT6"
    locs[0xFF6, 0]= "KR0"
    locs[0xFF6, 1]= "KR1"
    locs[0xFF6, 2]= "KR2"
    locs[0xFF6, 3]= "KR3"
    locs[0xFFB]= "PORT11"

    return locs


def resolve_labels(sections, used_codelocs):
    for section in sections:
        for instruction in section.instructions:
            for arg in instruction.arguments:
                if isinstance(arg, ast.CodeAddress):
                    if arg.address in used_codelocs:
                        arg.name = used_codelocs[arg.address]

if __name__ == '__main__':
    main()