from parsers import *
import ast

class OpCode:
    def __init__(self, name, template, *arguments):
        self.name = name
        self.template = template
        self.arguments = arguments
        self.mask = [0] * len(self.template)

        for arg in self.arguments:
            arg.mask(self.mask)

        for t, m in zip(self.template, self.mask):
            # sanity
            assert (t & m) == 0

    def __len__(self):
        return len(self.template)

    def __str__(self):
        return "Op({}: {})".format(self.name, ", ".join(i.__class__.__name__ for i in self.arguments))

    def __repr__(self):
        return str(self)

    def match(self, data):
        if len(data) < len(self):
            return False

        for template, mask, byte in zip(self.template, self.mask, data):
            if (byte & ~mask) != template:
                return False

        return True

    def decode(self, data, address):
        data = data[:len(self)]
        return ast.Instruction(self, address, *[arg.decode(data) for arg in self.arguments])

# https://www.renesas.com/kr/en/document/mah/upd750068-4-bit-single-chip-microcontrollers-users-manual
# datasheet seems to be missing docs on XCHG A, HL/+/- AND MOV A HL/+/- encoding, which is probably the 
# codes E1 and E9 (and possibly some more)

OPMAP = [

    # these are actually special cases of other instructions, so try them first
    OpCode("EI", [0x9D, 0xB2]), # this is essentially SET1 [FB2].3
    OpCode("EI", [0x9D, 0x98], IE4(1)), # again, SET1 [FB(8-F)].(1/3)

    OpCode("DI", [0x9C, 0xB2]), # this is essentially CLR1 [FB2].3
    OpCode("DI", [0x9C, 0x98], IE4(1)),# again, CLR1 [FB(8-F)].(1/3)

    OpCode("HALT", [0x9D, 0xA3]), # SET1 [PCC].2
    OpCode("STOP", [0x9D, 0xB3]), # SET1 [PCC].3

    OpCode("IN", [0xA3, 0xF0], Static("A"), Port(1)), # is just mov A, [xF(0-F)]
    OpCode("IN", [0xA2, 0xF0], Static("XA"), Port(1)), # is just mov XA, [xF(0-F)]

    OpCode("OUT", [0x93, 0xF0], Port(1), Static("A")), # is just mov [xF(0-F)], A
    OpCode("OUT", [0x92, 0xF0], Port(1), Static("XA")), # is just mov [xF(0-F)], XA

    OpCode("SEL", [0x99, 0x20], RB(1)), # ?
    OpCode("SEL", [0x99, 0x10], MB(1)), # ?

    OpCode("NOP", [0x60]), # this is basically add A, #0

    # these are guesses cause the datasheet is incomplete

    OpCode("MOV", [0xE1], Static("A"), Static("@HL")),
    OpCode("XCH", [0xE9], Static("A"), Static("@HL")),

    # normal instruction listing follows

    OpCode("MOV", [0x70], Static("A"), Imm4(0)),
    OpCode("MOV", [0x9A, 0x08], Reg1(1), Imm4(1, 4)),
    OpCode("MOV", [0x89, 0x00], RP(0, 1), Imm8(1)),
    OpCode("MOV", [0xE0], Static("A"), RPA1(0)),
    OpCode("MOV", [0xAA, 0x18], Static("XA"), Static("@HL")),
    OpCode("MOV", [0xE8], Static("@HL"), Static("A")),
    OpCode("MOV", [0xAA, 0x10], Static("@HL"), Static("XA")),
    OpCode("MOV", [0xA3, 0x00], Static("A"), Addr(1)),
    OpCode("MOV", [0xA2, 0x00], Static("XA"), ByteAddr(1, 1)),
    OpCode("MOV", [0x93, 0x00], Addr(1), Static("A")),
    OpCode("MOV", [0x92, 0x00], ByteAddr(1, 1), Static("XA")),
    OpCode("MOV", [0x99, 0x78], Static("A"), Reg(1)),
    OpCode("MOV", [0xAA, 0x58], Static("XA"), RP_(1)),
    OpCode("MOV", [0x99, 0x70], Reg1(1), Static("A")),
    OpCode("MOV", [0xAA, 0x50], RP_1(1), Static("XA")),

    OpCode("XCH", [0xE8], Static("A"), RPA1(0)),
    OpCode("XCH", [0xAA, 0x11], Static("XA"), Static("@HL")),
    OpCode("XCH", [0xB3, 0x00], Static("A"), Addr(1)),
    OpCode("XCH", [0xB2, 0x00], Static("XA"), ByteAddr(1, 1)),
    OpCode("XCH", [0xD8], Static("A"), Reg1(0)),
    OpCode("XCH", [0xAA, 0x40], Static("XA"), RP_(1)),

    OpCode("MOVT", [0xD4], Static("XA"), Static("@PCDE")),
    OpCode("MOVT", [0xD0], Static("XA"), Static("@PCXA")),
    OpCode("MOVT", [0xD1], Static("XA"), Static("@BCXA")),
    OpCode("MOVT", [0xD5], Static("XA"), Static("@BCDE")),

    OpCode("MOV1", [0xBD, 0x00], Static("CY"), BitManip(1)),
    OpCode("MOV1", [0x9B, 0x00], BitManip(1), Static("CY")),

    OpCode("ADDS", [0x60], Static("A"), Imm4(0)),
    OpCode("ADDS", [0xB9, 0x00], Static("XA"), Imm8(1)),
    OpCode("ADDS", [0xD2], Static("A"), Static("@HL")),
    OpCode("ADDS", [0xAA, 0xC8], Static("XA"), RP_(1)),
    OpCode("ADDS", [0xAA, 0xC0], RP_1(1), Static("XA")),

    OpCode("ADDC", [0xA9], Static("A"), Static("@HL")),
    OpCode("ADDC", [0xAA, 0xD8], Static("XA"), RP_(1)),
    OpCode("ADDC", [0xAA, 0xD0], RP_1(1), Static("XA")),

    OpCode("SUBS", [0xA8], Static("A"), Static("@HL")),
    OpCode("SUBS", [0xAA, 0xE8], Static("XA"), RP_(1)),
    OpCode("SUBS", [0xAA, 0xE0], RP_1(1), Static("XA")),

    OpCode("SUBC", [0xB8], Static("A"), Static("@HL")),
    OpCode("SUBC", [0xAA, 0xF8], Static("XA"), RP_(1)),
    OpCode("SUBC", [0xAA, 0xF0], RP_1(1), Static("XA")),

    OpCode("AND", [0x99, 0x30], Static("A"), Imm4(1)),
    OpCode("AND", [0x90], Static("A"), Static("@HL")),
    OpCode("AND", [0xAA, 0x98], Static("XA"), RP_(1)),
    OpCode("AND", [0xAA, 0x90], RP_1(1), Static("XA")),

    OpCode("OR", [0x99, 0x40], Static("A"), Imm4(1)),
    OpCode("OR", [0xA0], Static("A"), Static("@HL")),
    OpCode("OR", [0xAA, 0xA8], Static("XA"), RP_(1)),
    OpCode("OR", [0xAA, 0xA0], RP_1(1), Static("XA")),

    OpCode("XOR", [0x99, 0x50], Static("A"), Imm4(1)),
    OpCode("XOR", [0xB0], Static("A"), Static("@HL")),
    OpCode("XOR", [0xAA, 0xB8], Static("XA"), RP_(1)),
    OpCode("XOR", [0xAA, 0xB0], RP_1(1), Static("XA")),

    OpCode("RORC", [0x98], Static("A")),

    OpCode("NOT", [0x99, 0x5F], Static("A")),

    OpCode("INCS", [0xC0], Reg(0)),
    OpCode("INCS", [0x88], RP1(0, 1)),
    OpCode("INCS", [0x99, 0x02], Static("@HL")),
    OpCode("INCS", [0x82, 0x00], Addr(1)),

    OpCode("DECS", [0xC8], Reg(0)),
    OpCode("DECS", [0xAA, 0x68], RP_(1)),

    OpCode("SKE", [0x9A, 0x00], Reg(1), Imm4(1, 4)),
    OpCode("SKE", [0x99, 0x60], Static("@HL"), Imm4(1)),
    OpCode("SKE", [0x80], Static("A"), Static("@HL")),
    OpCode("SKE", [0xAA, 0x89], Static("XA"), Static("@HL")),
    OpCode("SKE", [0x99, 0x08], Static("A"), Reg(1)),
    OpCode("SKE", [0xAA, 0x48], Static("XA"), RP_(1)),

    OpCode("SET1", [0xE7], Static("CY")),
    OpCode("SET1", [0x85, 0x00], BitAddr(0, 4)),
    OpCode("SET1", [0x9D, 0x00], BitManip(1)),

    OpCode("CLR1", [0xE6], Static("CY")),
    OpCode("CLR1", [0x84, 0x00], BitAddr(0, 4)),
    OpCode("CLR1", [0x9C, 0x00], BitManip(1)),

    OpCode("SKT", [0xD7], Static("CY")),

    OpCode("SKT", [0x87, 0x00], BitAddr(0, 4)),
    OpCode("SKT", [0xBF, 0x00], BitManip(1)),

    OpCode("SKF", [0x86, 0x00], BitAddr(0, 4)),
    OpCode("SKF", [0xBE, 0x00], BitManip(1)),

    OpCode("SKTCLR", [0x9F, 0x00], BitManip(1)),

    OpCode("NOT1", [0xD6], Static("CY")),

    OpCode("AND1", [0xAC, 0x00], Static("CY"), BitManip(1)),

    OpCode("OR1", [0xAE, 0x00], Static("CY"), BitManip(1)),

    OpCode("XOR1", [0xBC, 0x00], Static("CY"), BitManip(1)),

    OpCode("BR", [0xAB, 0x00, 0x00], CodeAddr(2)),
    OpCode("BR", [0x00], RelAaddr(0)),
    OpCode("BR", [0xF0], RelSaddr(0)),
    OpCode("BR", [0x99, 0x04], Static("PCDE")),
    OpCode("BR", [0x99, 0x00], Static("PCXA")),
    OpCode("BR", [0x99, 0x05], Static("BCDE")),
    OpCode("BR", [0x99, 0x01], Static("BCXA")),

    OpCode("BRA", [0xBA, 0x00, 0x00], CodeAddr1(2)),

    OpCode("BRCB", [0x50, 0x00], CodeCaddr(1)),

    OpCode("CALLA", [0xBB, 0x00, 0x00], CodeAddr1(2)),

    OpCode("CALL", [0xAB, 0x40, 0x00], CodeAddr(2)),

    OpCode("CALLF", [0x40, 0x00], CodeFaddr(1)),

    OpCode("RET", [0xEE]),

    OpCode("RETS", [0xE0]),

    OpCode("RETI", [0xEF]),

    OpCode("PUSH", [0x49], RP(0, 1)),
    OpCode("PUSH", [0x99, 0x07], Static("BS")),

    OpCode("POP", [0x48], RP(0, 1)),
    OpCode("POP", [0x99, 0x06], Static("BS")),

    OpCode("GETI", [0x00], TableIndex(0))
]

# specials not part of the general opmap
BYTE = OpCode(".byte", [0x00])
WORD = OpCode(".word", [0x00, 0x00])
TBR = OpCode("TBR", [0x00, 0x00], CodeTaddr(1))
TCALL = OpCode("TCALL", [0x00, 0x01], CodeTaddr(1))


# dispatch helper for decodng operations
OPDICT = [[op for op in OPMAP if (i & ~op.mask[0]) == op.template[0]] for i in range (256)]
def decode_op(data, index, offset=0):
    # max op length is 3 bytes
    data = data[index:index + 3]

    options = OPDICT[data[0]]

    for option in options:
        if not option.match(data):
            continue

        try:
            return len(option), option.decode(data, index + offset)
        except BadDecode:
            pass
        except Exception:
            print(option, data)
            raise

    return 1, ast.Instruction(BYTE, index + offset, ast.Immediate(data[0]))

# section parsers with their different rules

def decode_vector_table(image, start=0x00, end=0x20) -> ast.Section:
    data = image[start:end]
    instructions = [
        ast.Instruction(WORD, i + start, ast.CodeAddress((data[i] << 8) + data[i + 1])) for i in range(0, end - start, 2)
    ]
    return ast.Section(".vectors", data, start, instructions)

def decode_geti_table(image, start=0x20, end=0x80) -> ast.Section:
    data = image[start:end]

    instructions = []
    for i in range(0, end - start, 2):
        if TBR.match(data[i:i+2]):
            instructions.append(TBR.decode(data[i:i+2], i + start))

        elif TCALL.match(data[i:i+2]):
            instructions.append(TCALL.decode(data[i:i+2], i + start))

        else:
            advance, instr = decode_op(data[i:i+2], 0, start)
            instructions.append(instr)
            if advance == 1:
                _, instr = decode_op(data[i:i+2], 1, start)
                instructions.append(instr)

    return ast.Section(".geti", data, start, instructions)

def decode_data(image, name, start, end) -> ast.Section:
    data = image[start:end]
    instructions = [ast.Instruction(BYTE, start + i, ast.Immediate(d)) for i, d in enumerate(data)]
    return ast.Section(name, data, start, instructions)

def decode_code(image, name, start, end) -> ast.Section:
    data = image[start:end]
    instructions = []

    index = 0
    while index < len(data):
        advance, instruction = decode_op(data, index, start)
        index += advance
        instructions.append(instruction)

    return ast.Section(name, data, start, instructions)

if __name__ == '__main__':
    pass
    
