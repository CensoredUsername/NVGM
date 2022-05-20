import re

regex = re.compile(r"MOSI: (0x[0-9A-F]+);  MISO: (0x[0-9A-F]+)")

def main():
    with open("eeprom_dump.csv", "r", encoding="utf-8") as f:
        header = next(f)
        lines = list(f)

    # parse it into pairs of (MOSI, MISO)
    pairs = []
    for line in lines:
        if match := regex.search(line):
            pairs.append((int(match.group(1), 16), int(match.group(2), 16)))

    values = {}

    for mosi, miso in pairs:
        start = (mosi >> 24) & 3
        command = (mosi >> 22) & 3
        address = (mosi >> 16) & 0x3F
        data_mosi = (mosi >> 0) & 0xFFFF
        data_miso = (miso >> 0) & 0xFFFF

        assert start == 0b01
        if command != 0b10:
            print("unknown command: start, command, address, data_mosi, data_miso")
        else:
            values[address] = data_miso
            assert data_mosi == 0x0000

    print(values)

    buf = []
    for address in range(64):
        # big endian
        value = values[address]
        buf.append(value >> 8)
        buf.append(value & 0xFF)

    for i, b in enumerate(buf):
        print("{:02x}: {:02x}".format(i, b))

if __name__ == '__main__':
    main()
