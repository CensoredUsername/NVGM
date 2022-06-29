// accumulators used for creating a new config
// 0xB0
static mut accum_16bit: [u16; 6];

// staging buffer where a new config is written to before it's copied over to memory bank 1
// 0x50
static mut config_staging_buffer: [u4; 16];

// data describing the value of a coin, some other data, and uncertainties for each 6 samples
// data_1000
static coin_data: u8[0xF * 8] = [
    0x01, 0x00, 0x01, 0x08, 0x07, 0x06, 0x08, 0x08, 0x08, 0x07, 0x05, 0x06, 0x06, 0x08, 0x08,
    0x05, 0x00, 0x00, 0x06, 0x08, 0x07, 0x09, 0x09, 0x09, 0xff, 0xff, 0x06, 0x06, 0x08, 0x08,
    0x10, 0x00, 0x07, 0x07, 0x07, 0x08, 0x08, 0x0a, 0x09, 0xff, 0xff, 0x06, 0x06, 0x08, 0x08,
    0x50, 0x00, 0x05, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x05, 0x06, 0x06, 0x08, 0x08,
    0x50, 0x00, 0x05, 0x06, 0x07, 0x06, 0x06, 0x07, 0x05, 0x06, 0x05, 0x06, 0x06, 0x08, 0x08,
    0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
];

// indices into coin_data
// data_1078
static coin_data_indices: u8[8] = [0x00, 0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69];

// loads the index for the start of the coin cata for the coin specified by encoder1_readout
// note: abuses total_cost to hold the index
// sub_1080
fn load_coin_data_start() {
    // MB = unknown
    XA = coin_data_indices[encoder1_readout]
    total_cost = XA
}

// loads the next value out of coin data and increments the index
// note: abuses total_cost to hold the index
// sub_1094
fn load_coin_data_next() -> XA {
    // MB = unknown
    XA = coin_data[total_cost]
    total_cost += 1
}

// builds a new config, and writes it to a slot determined by encoder 1 in the eeprom
// sub_d43
fn build_new_config() -> skip {
    A = get_encoder_readout()

    HL = A * 32

    accumulate_samples()
    reset_basic_timer()

    load_coin_data_start()
    reset_basic_timer()

    build_config_lower()
    reset_basic_timer()

    read_eeprom_to_bank_1()
    reset_basic_timer()

    copy_new_config(HL)

    build_config_upper()
    reset_basic_timer()

    HL += 16
    copy_new_config(HL)

    // copies old top/bottom to F4/F6
    write_bank1(0xF4, read_bank1(0xF0))
    write_bank1(0xF6, read_bank1(0xF2))
    // writes down new top/bottom osc values
    write_bank1(0xF0, top_osc_max)
    write_bank1(0xF2, bottom_osc_max)

    return true
}

// copies 16 nibbles from 050 to 1HL. probably this writes a newly assembled config.
// sub_04fc
fn copy_new_config(HL) {
    DE = &config_staging_buffer
    B = 0xF
    enable_memory_bank()
    select_memory_bank(0)
    {
        A = *DE
        write_bank1(HL, A)
        L += 1
        DE += 1
        if (B == 0) {
            return
        }
        B -= 1
    }
}

// accumulates samples stored in bank1 (6 different sets, sampled 16 times each)
// into the 6 accumulators
// sub_0d95
fn accumulate_samples() {
    enable_memory_bank()
    select_memory_bank(0)
    accum_16bit[0] = 0
    accum_16bit[1] = 0
    accum_16bit[2] = 0
    accum_16bit[3] = 0
    accum_16bit[4] = 0
    accum_16bit[5] = 0

    temps[6:7] = 0
    B = 0xF
    HL = 0

    loop {
        temps[4:5] = read_bank1(HL)
        add_16bit(&accum_16bit[0], &temps[4])
        temps[4:5] = read_bank1(HL + 2)
        add_16bit(&accum_16bit[1], &temps[4])
        temps[4:5] = read_bank1(HL + 4)
        add_16bit(&accum_16bit[2], &temps[4])
        temps[4:5] = read_bank1(HL + 6)
        add_16bit(&accum_16bit[3], &temps[4])
        temps[4:5] = read_bank1(HL + 8)
        add_16bit(&accum_16bit[4], &temps[4])
        temps[4:5] = read_bank1(HL + 10)
        add_16bit(&accum_16bit[5], &temps[4])
        HL += 16
        if B == 0 {
            return
        }
        B -= 1
    }
}

// builds the lower part of a new config
// sub_0e21
fn build_config_lower() {
    enable_memory_bank()
    select_memory_bank(0)
    config_staging_buffer[0] = load_coin_data_next()
    config_staging_buffer[2] = load_coin_data_next()
    config_staging_buffer[4] = load_coin_data_next()
    config_staging_buffer[6] = config_add_upwards_uncertainty(&accum_16bit[0], &temps[4])
    config_staging_buffer[8] = config_add_downwards_uncertainty(&accum_16bit[0], &temps[4])
    config_staging_buffer[A] = config_add_upwards_uncertainty(&accum_16bit[1], &temps[4])
    config_staging_buffer[C] = config_add_downwards_uncertainty(&accum_16bit[1], &temps[4])
    config_staging_buffer[E] = config_add_upwards_uncertainty(&accum_16bit[2], &temps[4])
}

// builds the upper part of a new config
// sub_0e62
fn build_config_upper() {
    enable_memory_bank()
    select_memory_bank(0)
    config_staging_buffer[0] = config_add_downwards_uncertainty(&accum_16bit[2], &temps[4])
    config_staging_buffer[2] = config_add_upwards_uncertainty(&accum_16bit[3], &temps[4])
    config_staging_buffer[4] = config_add_downwards_uncertainty(&accum_16bit[3], &temps[4])
    config_staging_buffer[2] = config_add_upwards_uncertainty(&accum_16bit[4], &temps[4])
    config_staging_buffer[8] = config_add_downwards_uncertainty(&accum_16bit[4], &temps[4])
    config_staging_buffer[A] = config_add_upwards_uncertainty(&accum_16bit[5], &temps[4])
    config_staging_buffer[C] = config_add_downwards_uncertainty(&accum_16bit[5], &temps[4])
    config_staging_buffer[E] = 0xFF
}

// takes the accumulator in HL, moves it to the temp in DE and adds load_coin_data_next() << 4 to it
// then shifts the value right 4 bits, clamps it to 0 - FF and returns it in XA
// sub_0eaa
fn config_add_upwards_uncertainty(HL, DE) -> XA {
    copy_16bit(HL, DE)
    XA = load_coin_data_next()
    temps[0:3] = {0, A, X, 0}
    add_16bit(&temps[4], &temps[0])
    if temps[7] != 0 {
        XA = 0xFF
    } else {
        X = temps[6]
        A = temps[5]
    }
}

// takes the accumulator in HL, moves it to the temp in DE and subtracts load_coin_data_next() << 4 from it
// then shifts the value right 4 bits, clamps it to 0 - FF and returns it in XA
// sub_0ed1
fn config_add_downwards_uncertainty(HL, DE) -> XA {
    copy_16bit(HL, DE)
    XA = load_coin_data_next()
    temps[0:3] = {0, A, X, 0}
    add_16bit(&temps[4], &temps[0])
    if temps[7] == 0xF {
        XA = 0
    } else {
        X = temps[6]
        A = temps[5]
    }
}
