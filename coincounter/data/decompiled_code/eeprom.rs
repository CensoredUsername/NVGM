// 0x38
static mut eeprom_sync_index: u8;

// 0x4B.1
static mut sending_ewds: bool;


// reads the EEPROM to memory bank 1, and verifies it a second time
// retries until success
// sub_0563
fn initialize_eeprom() {
    loop {
        disable_memory_bank()
        read_eeprom_to_bank_1()
        validate_eeprom_bank_1()
        if !CY {
            break
        }
    }
    disable_memory_bank()
    port0_readout = 0xF
    port1_readout = 0xF
}


// occasionally sends a write disable, and reads out an eeprom word
// sub_0b96
fn sync_eeprom_to_bank_1() {
    if !port0_readout.0 || suppress_eeprom_sync {
        return
    }
    if (eeprom_sync_index & 0xF) == 0 && !sending_ewds {
        sending_ewds = 1
        send_eeprom_ewds()
        return
    }
    sending_ewds = 0
    B,C,D,E = get_eeprom_symbols()
    HL = eeprom_sync_index
    eeprom_sync_index = (eeprom_sync_index + 4) & 0xFC

    enable_memory_bank()

    select_memory_bank(0xF)
    send_eeprom_command(6, HL, B, D)
    read_eeprom_reply(B, C, E)
    write_bank1(HL, BSB[0:1])
    write_bank1(HL + 2, BSB[2:3])
}


// reads the eeprom and copies over its contents to memory bank 1 (all 256 nibbles)
// sub_0bd8
fn read_eeprom_to_bank_1() {
    B, C, D, E = get_eeprom_symbols()
    enable_memory_bank()
    select_memory_bank(1)
    HL = 0
    loop {
        reconfigure_timers()
        send_eeprom_command(6, HL, B, D)
        read_eeprom_reply(B, C, E)
        *HL = BSB[0:1]
        HL += 2
        *HL = BSB[2:3]
        HL += 2
        if HL == 0 {
            break;
        }
    }
}

// Does another round of fetches from the EEPROM, and checks if it matches
// what came in last time
// sub_0c06
fn validate_eeprom_bank_1() {
    enable_memory_bank()
    select_memory_bank(1)
    HL = 0
    loop {
        B, C, D, E = reconfigure_timers()
        if validate_eeprom_four_reads(HL, B, C, D, E) {
            CY = 1
            return
        }

        H += 1
        if H == 0 {
            break
        }
    }
    CY = 0
}

// loads 4x4nibbles of data from the eeprom and checks if it matches the
// recorded data
// sub_0c20
fn validate_eeprom_four_reads(HL, B, C, D, E) {
    CY = 1
    select_memory_bank(1)
    loop {
        assert_eeprom_cs(B)

        send_eeprom_command(6, HL, B, D)
        read_eeprom_reply(B, C, E)


        XA = BSB[0:1]
        if A != *HL {
            return;
        }
        L += 1
        A = X
        if A != *HL {
            return;
        }
        L += 1
        XA = BSB[2:3]
        if A != *HL {
            return;
        }
        L += 1
        A = X
        if A != *HL {
            return;
        }
        L += 1
        if L == 0 {
            break;
        }
    }
    CY = 0
    return;
}

// writes the contents of bank 1 to the EEPROM
// sub_0c51
fn write_eeprom_from_bank_1() {
    B, C, D, E = get_eeprom_symbols()
    select_memory_bank(1)
    HL = 0
    loop {
        reset_basic_timer()
        write_eeprom_four_writes(HL, B, D, E)
        H += 1
        if H == 0 {
            return
        }
    }
}

// does 4 EEPROM writes
// sub_0c66
fn write_eeprom_four_writes(HL, B, D, E) {
    loop {
        select_memory_bank(0xF)
        assert_eeprom_cs()
        send_eeprom_command(5, H, L, B, D)
        BSB[0:1] = read_bank1(HL)
        BSB[2:3] = read_bank1(HL + 2)
        send_eeprom_data(B, D, E)
        L += 4
        if L == 0 {
            return
        }
    }
}

// sends a EWEN command
// sub_00c90
fn send_eeprom_ewen() {
    B, C, D, E = get_eeprom_symbols()
    H = 0xF
    X = 4
    send_eeprom_command(X, H, L, B, D)
    PORT2 = E
}

// sends a EWDS command
// sub_0c98
fn send_eeprom_ewds() {
    B, C, D, E = get_eeprom_symbols()
    H = 0x0
    X = 4
    send_eeprom_command(X, H, L, B, D)
    PORT2 = E
}

// bitbangs 10 microwire symbols using the bit shuffle buffer.
// sub_0ca7
fn send_eeprom_command(X, H, L, B, D) {
    BSB[3] = X
    BSB[2] = H
    BSB[1] = L
    L = 0xF
    while L != 5 {
        if BSB[L >> 2] & (1 << (L & 3)) {
            A = D
        } else {
            A = B
        }
        PORT2 = A
        PORT2 = A | 4
        PORT2 = A & 0xb
        L -= 1
        if L == 5 {
            break
        }
    }
}

// writing microwire bitbang output for 16 cycles
// sub_0ccd
fn send_eeprom_data(B, D, E) {
    L = 0xF
    loop {
        if BSB[L >> 2] & (1 << (L & 3)) {
            A = D
        } else {
            A = B
        }
        PORT2 = A
        PORT2 = A | 4
        PORT2 = A & 0xB
        if L == 0 {
            break
        }
        L -= 1
    }
    PORT2 = E;
}

// more microwire bitbanging, this time it's taking input for 16 cycles
// sub_0ce9
fn read_eeprom_reply(B, C, E) {
    L = 0xF
    loop {
        PORT2 = C
        PORT2 = B
        if (PORT1 & 4) {
            BSL[L >> 2] |= 1 << (L & 3);
        } else {
            BSL[L >> 2] &= ~(1 << (L & 3));
        }
        if L == 0 {
            break
        }
        L -= 1
    }
    PORT2 = E;
}

// bitbanging microwire symbols. it reads port2_gpio_output because that has the current solenoid output state
// and that GPIO is in the same bank.
// E = all lines low
// B = assert CS
// C = assert CS && assert CLK
// D = assert CS && assert MOSI
// sub_0d07
fn get_eeprom_symbols() -> B, C, D, E {
    E = port2_gpio_output & 1
    B = E | 2
    C = E | 6
    D = E | 0xA
}

// asserts the eeprom CS, then waits until MISO goes high
// sub_0d1c
fn assert_eeprom_cs(B) {
    PORT2 = B
    nop()
    nop()
    while(!(PORT1 & 4));
    nop()
    nop()
    nop()
}
