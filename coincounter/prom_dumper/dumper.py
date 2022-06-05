import serial
import threading
import re

def readline(conn):
    line = b""
    while True:
        char = conn.read(1)
        if char == b"\n":
            return line
        elif char:
            line += char
        else:
            return None


def read_thread(conn, file, chan, bin):
    line = b""
    REGEX = re.compile(r"([0-9A-F]+): ([0-9A-F]+)")
    while chan[0]:
        char = conn.read(1)
        if not char:
            continue
        if char == b"\n":
            print(line.decode("latin1"))
            match = REGEX.match(line.decode("latin1"))
            if match:
                bin.write(bytes([int(match.group(2), 16)]))
            file.write(line)
            file.write(b"\n")
            line = b""
        else:
            line += char

def main():
    print("Please input the port")
    port = input()
    conn = serial.Serial(port, 38400, timeout=1.0)

    chan = [True]
    bin = open("bin.bin", "wb")

    with open("log.txt", "wb") as f:
        thread = threading.Thread(target=read_thread, name="read_thread", args=(conn, f, chan, bin))
        thread.start()

        try:
            while chan[0]:
                text = input()
                if text == "quit":
                    break
                else:
                    conn.write(text.encode("latin1") + b"\n")
        finally:
            chan[0] = False


if __name__ == '__main__':
    main()
