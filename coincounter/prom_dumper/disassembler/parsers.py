import ast


class BadDecode(Exception):
    # Exception raised when we can't fit a parser properly
    pass


class ArgParser:
    # the byte in which the argument starts, and the bit where the argument starts bytes counted from the left, bits from smallest to largest
    # except for addresses, no fields ever cross over multiple bytes
    def __init__(self, bytepos, bitpos=0):
        self.bytepos = bytepos
        self.bitpos = bitpos

    def slice1(self, bytes):
        byte = bytes[self.bytepos]
        return byte >> self.bitpos

    def mask1(self, bytes, width):
        mask = (1 << width) - 1
        bytes[self.bytepos] |= (mask << self.bitpos)

    def mask(self):
        raise NotImplementedError()

    def decode(self, bytes):
        raise NotImplementedError()

# register addressing

class Reg(ArgParser):
    def decode(self, bytes):
        field = self.slice1(bytes) & 7
        return ast.Direct(ast.Register(field))

    def mask(self, bytes):
        self.mask1(bytes, 3)

class Reg1(ArgParser):
    def decode(self, bytes):
        field = self.slice1(bytes) & 7
        if field == 0:
            raise BadDecode()
        return ast.Direct(ast.Register(field))

    def mask(self, bytes):
        self.mask1(bytes, 3)

class RP_(ArgParser):
    def decode(self, bytes):
        field = self.slice1(bytes) & 7
        return ast.Direct(ast.RegisterPair(field))

    def mask(self, bytes):
        self.mask1(bytes, 3)

class RP_1(ArgParser):
    def decode(self, bytes):
        field = self.slice1(bytes) & 7
        if field == 0:
            return BadDecode()
        return ast.Direct(ast.RegisterPair(field))

    def mask(self, bytes):
        self.mask1(bytes, 3)

class RPA(ArgParser):
    def decode(self, bytes):
        field = self.slice1(bytes) & 7
        try:
            return ast.Indirect(ast.RegisterIndirect(field))
        except ValueError:
            raise BadDecode()

    def mask(self, bytes):
        self.mask1(bytes, 3)

class RPA1(ArgParser):
    def decode(self, bytes):
        field = self.slice1(bytes) & 7
        if field != 4 and field != 5:
            raise BadDecode()
        return ast.Indirect(ast.RegisterIndirect(field))

    def mask(self, bytes):
        self.mask1(bytes, 3)

class RP(ArgParser):
    def decode(self, bytes):
        field = self.slice1(bytes) & 3
        return ast.Direct(ast.RegisterPair(field << 1))

    def mask(self, bytes):
        self.mask1(bytes, 2)

class RP1(ArgParser):
    def decode(self, bytes):
        field = self.slice1(bytes) & 3
        if field == 0:
            raise BadDecode()
        return ast.Direct(ast.RegisterPair(field << 1))

    def mask(self, bytes):
        self.mask1(bytes, 2)

class RP2(ArgParser):
    def decode(self, bytes):
        field = self.slice1(bytes) & 3
        if field == 0 or field == 1:
            raise BadDecode()
        return ast.Direct(ast.RegisterPair(field << 1))

    def mask(self, bytes):
        self.mask1(bytes, 2)

# immediates

class Imm4(ArgParser):
    def decode(self, bytes):
        field = self.slice1(bytes) & 0xF
        return ast.Immediate(field)

    def mask(self, bytes):
        self.mask1(bytes, 4)

class Imm8(ArgParser):
    def decode(self, bytes):
        field = self.slice1(bytes) & 0xFF
        return ast.Immediate(field)

    def mask(self, bytes):
        self.mask1(bytes, 8)

class Port(ArgParser):
    def decode(self, bytes):
        field = self.slice1(bytes) & 0xF
        return ast.Immediate(field, prefix="PORT")

    def mask(self, bytes):
        self.mask1(bytes, 4)

class RB(ArgParser):
    def decode(self, bytes):
        field = self.slice1(bytes) & 0x3
        return ast.Immediate(field, prefix="RB")

    def mask(self, bytes):
        self.mask1(bytes, 2)

class MB(ArgParser):
    def decode(self, bytes):
        field = self.slice1(bytes) & 0xF
        return ast.Immediate(field, prefix="MB")

    def mask(self, bytes):
        self.mask1(bytes, 4)

# interrupt literals

class IE4(ArgParser):
    def decode(self, bytes):
        field = self.slice1(bytes) & 0x27
        field = (field & 7) | ((field & 0x20) >> 2)
        try:
            return ast.Direct(ast.IExxx(field))
        except ValueError:
            raise BadDecode()

    def mask(self, bytes):
        bytes[self.bytepos] |= (0x27 << self.bitpos)

# addressing

class Addr(ArgParser):
    def decode(self, bytes):
        field = self.slice1(bytes) & 0xFF
        return ast.Address(field)

    def mask(self, bytes):
        self.mask1(bytes, 8)

class ByteAddr(ArgParser):
    def decode(self, bytes):
        field = self.slice1(bytes) & 0x7F
        return ast.Address(field * 2)

    def mask(self, bytes):
        self.mask1(bytes, 7)

class BitAddr(ArgParser):
    def decode(self, bytes):
        addr = bytes[1]
        bit = self.slice1(bytes) & 3
        return ast.BitAddr(addr, bit)

    def mask(self, bytes):
        self.mask1(bytes, 2)
        bytes[1] |= 0xFF

class BitManip(ArgParser):
    def decode(self, bytes):
        byte = bytes[self.bytepos]
        op = byte >> 6

        imm = byte & 0xF
        bit = (byte >> 4) & 3

        if op == 2:
            return ast.BitAddr(imm + 0xFB0, bit)

        elif op == 3:
            return ast.BitAddr(imm + 0xFF0, bit)

        elif op == 1:
            if bit != 0:
                raise BadDecode()

            return ast.BitAddrLow((imm << 2) + 0xFC0)

        else:
            return ast.BitAddrHigh(imm, bit)

    def mask(self, bytes):
        bytes[self.bytepos] |= 0xFF

# code addresses

class TableIndex(ArgParser):
    def decode(self, bytes):
        field = self.slice1(bytes) & 0x3F
        field <<= 1
        if field < 0x20:
            raise BadDecode()
        return ast.CodeAddress(field)

    def mask(self, bytes):
        self.mask1(bytes, 6)

class CodeTaddr(ArgParser):
    def decode(self, bytes):
        field = (bytes[self.bytepos] + (bytes[self.bytepos - 1] << 8)) & 0x3FFE
        return ast.CodeAddress(field)

    def mask(self, bytes):
        bytes[self.bytepos - 1] |= 0x3F
        bytes[self.bytepos] |= 0xFE

class CodeFaddr(ArgParser):
    def decode(self, bytes):
        field = (bytes[self.bytepos] + (bytes[self.bytepos - 1] << 8)) & 0x7FF
        return ast.CodeAddress(field)

    def mask(self, bytes):
        bytes[self.bytepos - 1] |= 0x7
        bytes[self.bytepos] |= 0xFF

class CodeCaddr(ArgParser):
    def decode(self, bytes):
        field = (bytes[self.bytepos] + (bytes[self.bytepos - 1] << 8)) & 0xFFF
        return ast.CodeAddress(field, pcmask=0x3000)

    def mask(self, bytes):
        bytes[self.bytepos - 1] |= 0xF
        bytes[self.bytepos] |= 0xFF

class CodeAddr(ArgParser):
    def decode(self, bytes):
        field = (bytes[self.bytepos] + (bytes[self.bytepos - 1] << 8)) & 0x3FFF
        return ast.CodeAddress(field)

    def mask(self, bytes):
        bytes[self.bytepos - 1] |= 0x3F
        bytes[self.bytepos] |= 0xFF

class CodeAddr1(ArgParser):
    def decode(self, bytes):
        field = (bytes[self.bytepos] + (bytes[self.bytepos - 1] << 8)) & 0x7FFF
        return ast.CodeAddress(field)

    def mask(self, bytes):
        bytes[self.bytepos - 1] |= 0x7F
        bytes[self.bytepos] |= 0xFF

class RelAaddr(ArgParser):
    def decode(self, bytes):
        field = self.slice1(bytes) & 0xF
        return ast.CodeAddress(field + 1, False)

    def mask(self, bytes):
        self.mask1(bytes, 4)

class RelSaddr(ArgParser):
    def decode(self, bytes):
        field = self.slice1(bytes) & 0xF
        return ast.CodeAddress(field - 15, False)

    def mask(self, bytes):
        self.mask1(bytes, 4)

# AST node that doesn't actually have any encoding

class Static(ArgParser):
    def __init__(self, text):
        self.text = text

    def decode(self, bytes):
        return ast.Literal(self.text)

    def mask(self, bytes):
        pass


