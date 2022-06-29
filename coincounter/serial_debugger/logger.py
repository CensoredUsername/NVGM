import serial

def main():
    port = input("enter port: ")
    s = serial.Serial(port, baudrate=38400, timeout=0.1)

    session = input("enter session name: ")

    with open("data/coin_serial_dumps/" + session + ".txt", "wb") as f:
        for i in range(64):
            s.write(b"\n")
            print("{}: ".format(i), end='', flush=True)
            s.flush()

            buf = bytearray()
            while True:
                char = s.read(1)
                if not char:
                    continue
                elif char == b"\r":
                    continue
                elif char == b"\n":
                    break
                buf += char

            print(buf)
            f.write(buf)
            f.write(b"\n")

if __name__ == '__main__':
    main()
