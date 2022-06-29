import serial
import struct


class TimeoutException(Exception):
    pass


class Eeprom:
    def __init__(self, port):
        self.s = serial.Serial(port, 38400, timeout=0.1)

    def read_all(self):
        return b"".join(self.read(i) for i in range(0x40))

    def read_range(self, start, end):
        if 0 > start > 0x40 or 0 > end > 0x40:
            raise ValueError()

        return b"".join(self.read(i) for i in range(start, end))

    def write_all(self, data):
        if len(data) != 0x80:
            raise ValueError()

        for i in range(0x40):
            self.write(i, data[i * 2 : i * 2 + 2])

    def write_range(self, start, data):
        end = start + len(data) // 2 
        if 0 > start > 0x40 or 0 > end > 0x40 or len(data) & 1:
            raise ValueError()

        for i in range(len(data) // 2 ):
            self.write(i + start, data[i * 2 : i * 2 + 2])


    def read(self, address):
        return self.send_command("R", address, b'\0\0')

    def write(self, address, data):
        self.send_command("W", address, data)

    def erase(self, address):
        self.send_command("E", address, b'\0\0')

    def send_command(self, command, address, data):
        if address > 0x3F:
            raise ValueError()

        while self.s.read():
            pass

        command = struct.pack("<BBBB2s", 0xA5, 0xC3, ord(command), address, data)

        self.s.write(command)
        reply = self.s.read(6)

        if len(reply) != 6 or reply[0] != 0x5A or reply[1] != 0x3C or reply[2:4] != command[2:4]:
            raise TimeoutException()

        return reply[4:6]

def main():
    port = input("PORT: ")
    eeprom = Eeprom(port)

    config = input("config: ")
    with open(config, "rb") as f:
        data = f.read()

    assert len(data) == 128
    print("original config: {}".format(eeprom.read_all()))

    eeprom.write_all(data)

    print("eeprom written")

    if eeprom.read_all() == data:
        print("eeprom verified")

    else:
        print("eeprom write error")

if __name__ == '__main__':
    main()