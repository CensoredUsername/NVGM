import math

def analyse_data(filename, value):
    with open("data/coin_serial_dumps/" + filename, "r", encoding="utf-8") as f:
        data = f.read()

    datapoints = []
    for line in data.splitlines():
        if line:
            line = line[2:-1]
            datapoints.append([int(i, 16) * 16 for i in line.split()])

    minima = [min(i) for i in zip(*datapoints)]
    maxima = [max(i) for i in zip(*datapoints)]
    average = [sum(i) // len(datapoints) for i in zip(*datapoints)]
    dev = [max - min for min, max in zip(minima, maxima)]
    differences = [0] * len(average)
    for datapoint in datapoints:
        for i in range(len(differences)):
            differences[i] += (datapoint[i] - average[i]) ** 2
    stddev = [int(round((d / len(datapoints)) ** 0.5)) for d in differences]

    # analysis

    print("{}:".format(filename))
    print("min: {}".format(", ".join("{:03X}".format(i) for i in minima)))
    print("max: {}".format(", ".join("{:03X}".format(i) for i in maxima)))
    print("avg: {}".format(", ".join("{:03X}".format(i) for i in average)))
    print("dev: {}".format(", ".join("{:03X}".format(i) for i in dev)))
    print("std: {}".format(", ".join("{:03X}".format(i) for i in stddev)))

    # building config
    flags = (0
        | (minima[6] >= 0x300)
        | (maxima[7] == 0x000) << 1
        | (minima[8] == 0x0F0) << 2
    )
    config = [value, 0, flags]
    for i in range(6):
        top = min(0xFF0, average[i] + max(0x50, 3 * stddev[i]))
        bot = max(0x000, average[i] - max(0x50, 3 * stddev[i]))
        config.append((top >> 4) + ((top & 8) != 0))
        config.append((bot >> 4) + ((bot & 8) != 0))

    config.append(0xFF)
    print("config: {}".format(", ".join("{:02X}".format(i) for i in config)))


    return config

def check_data_with_config(filename, config):
    with open("data/coin_serial_dumps/" + filename, "r", encoding="utf-8") as f:
        data = f.read()

    datapoints = []
    for line in data.splitlines():
        if line:
            line = line[2:-1]
            datapoints.append([int(i, 16) for i in line.split()])

    passed = (datapoint for datapoint in datapoints if 
        config[3] >= datapoint[0] >= config[4] and
        config[5] >= datapoint[1] >= config[6] and
        config[7] >= datapoint[2] >= config[8] and
        config[9] >= datapoint[3] >= config[10] and
        config[11] >= datapoint[4] >= config[12] and
        config[13] >= datapoint[5] >= config[14] and
        (config[2] & 1 == 0 or datapoint[6] > 0x2C) and
        (config[2] & 2 == 0 or datapoint[7] == 0x00) and
        (config[2] & 4 == 0 or datapoint[8] == 0x0F) 
    )
    return len(list(passed))



def main():
    files = ["2euro", "1euro", "50cent", "20cent", "10cent", "100yen"]
    values = [0x20, 0x10, 0x5, 0x2, 0x1, 0x10]

    configs = []
    for file, value in zip(files, values):
        config = analyse_data(file + ".txt", value)
        passes = [check_data_with_config(name + ".txt", config) for name in files]
        print("passes: {}".format(passes))
        configs.append(config)

    # don't put the yen coins in the config
    configs = configs[0:5]
    configs.sort(key=lambda c:c[0])
    configs.append(bytes([
        0xFF, 0x00, 0x45, 0x55, 0x52, 0x4F, 0x20, 0x52, 0x4F, 0x4D, 0x20, 0x48, 0x41, 0x43, 0x4B, 0x0A,
        0xFF, 0x00, 0x42, 0x79, 0x20, 0x43, 0x65, 0x6E, 0x73, 0x6F, 0x72, 0x65, 0x64, 0x2D, 0x20, 0x0A,
        0xFF, 0x00, 0x20, 0x20, 0x20, 0x20, 0x55, 0x73, 0x65, 0x72, 0x6E, 0x61, 0x6D, 0x65, 0x20, 0x0A,
    ]))

    with open("data/eeprom_configs/config_new.bin", "wb") as f:
        f.write(b"".join(bytes(c) for c in configs))

if __name__ == '__main__':
    main()
