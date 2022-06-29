from enum import Enum


class ASTNode:
    def __str__(self):
        raise NotImplementedError()


class Register(Enum):
    A = 0
    X = 1
    L = 2
    H = 3
    E = 4
    D = 5
    C = 6
    B = 7

class RegisterPair(Enum):
    XA = 0
    XA_ = 1
    HL = 2
    HL_ = 3
    DE = 4
    DE_ = 5
    BC = 6
    BC_ = 7

class RegisterIndirect(Enum):
    HL = 0
    HL_INC = 2
    HL_DEC = 3
    DE = 4
    DL = 5

class RegisterQuad(Enum):
    PCXA = 0
    BCXA = 1
    PCDE = 4
    BCDE = 5

class IExxx(Enum):
    IEBT = 0
    IEW = 2
    IET0 = 4
    IECSI = 5
    IE0 = 6
    IE2 = 7
    IE4 = 8
    IET1 = 12
    IE1 = 14


class Literal(ASTNode):
    # used for stuff like the carry flag, BS. or any static register
    def __init__(self, literal):
        self.literal = literal

    def __str__(self):
        return self.literal

class Direct(ASTNode):
    # direct register (any of Register, RegisterPair, registerQuad, or IExxx)
    def __init__(self, register):
        self.register = register

    def __str__(self):
        return self.register.name

class Indirect(ASTNode):
    # indirect register (any of RegisterIndirect, RegisterPair, RegisterQuad)
    def __init__(self, register):
        self.register = register

    def __str__(self):
        return "@" + self.register.name

class Immediate(ASTNode):
    # an immediate, normally prefixed with a #
    def __init__(self, value, prefix="#"):
        self.value = value
        self.prefix = prefix

    def __str__(self):
        return "{}{:X}".format(self.prefix, self.value)

class CodeAddress(ASTNode):
    # absolute code addresses are prefixed with a !, relative code addresses with a $
    def __init__(self, address, absolute=True, pcmask=0):
        self.address = address
        self.absolute = absolute
        self.name = None
        self.pcmask = 0

    def __str__(self):
        if self.name != None:
            return self.name
        elif self.absolute:
            return "!{:04X}".format(self.address)
        else:
            return "${:02X}".format(self.address)

class Address(ASTNode):
    # a simple data address. an x is left to indicate we don't know the bank
    def __init__(self, address):
        self.address = address
        self.name = None

    def __str__(self):
        name = (self.name + "/") if self.name else ""
        return "[{}?{:02X}]".format(name, self.address)

class BitAddr(ASTNode):
    # fmem.bit or mem.bit
    def __init__(self, address, bit):
        self.address = address
        self.bit = bit
        self.name = None

    def __str__(self):
        if self.address > 0xFF:
            if self.name:
                return "[{}]".format(self.name)
            else:
                return "[{:03X}.{}]".format(name, self.address, self.bit)
        else:
            name = (self.name + "/") if self.name else ""
            return "[{}?{:02X}.{}]".format(name, self.address, self.bit)

class BitAddrLow(ASTNode):
    # pmem.@L
    def __init__(self, address):
        self.address = address

    def __str__(self):
        return "[{:03X}].@L".format(self.address)

class BitAddrHigh(ASTNode):
    # @H+mem.bit
    def __init__(self, address, bit):
        self.address = address
        self.bit = bit

    def __str__(self):
        return "@H+[?{:2X}.{}]".format(self.address, self.bit)

# a single (pseudo)instruction

class Instruction(ASTNode):
    # an instruction is an opcode with a list of comma-separated arguments
    def __init__(self, opcode, address, *arguments):
        self.opcode = opcode
        self.arguments = arguments
        self.address = address

    def __str__(self):
        if self.arguments:
            return "{} {}".format(self.opcode.name, ", ".join(str(arg) for arg in self.arguments))
        else:
            return self.opcode.name

# a section of the executable

class Section(ASTNode):
    def __init__(self, name, source, address, instructions):
        self.name = name
        self.instructions = instructions
        self.address = address
        self.source = source

    def get_source(self, address, length):
        return self.source[address - self.address : address - self.address + length]

