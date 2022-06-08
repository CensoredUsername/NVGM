// utility functions
fn disable_interrupts() {
    asm DI;
}

fn enable_interrupts() {
    asm EI;
}

fn start_basic_timer(setting) {
    BTM = 8 | setting
}

fn restart_basic_timer() {
    asm SET1 BTM.3
}

fn enable_interrupt(IExxx) {
    asm EI IExxx
}

fn disable_interrupt(IExxx) {
    asm DI IExxx
}

fn disable_memory_bank() {
    asm CLR1 MBE
}

fn enable_memory_bank() {
    asm SET1 MBE
}

fn select_memory_bank(MBn) {
    asm SEL MBn
}

fn enable_watchdog_timer() {
    asm SET1 WDTM.3
}

fn configure_watch_mode(setting) {
    WM = setting
}

fn read_from_bank_0(HL) {
    enable_memory_bank();
    select_memory_bank(0);
    return *HL
}

fn read_from_bank_1(HL) {
    enable_memory_bank();
    select_memory_bank(1);
    return *HL
}


// reset handler
fn RESET() {
    disable_interrupts()
    start_basic_timer(0)
    enable_watchdog_timer()
    configure_watch_mode(6) // enabled, IRQW at 3.91ms intervals

    // wait until the watch interrupt request is asserted,
    // then clear it
    while (!IRQW);
    IRQW = 0

    PCC = 3 // divide input clock by 4
    SOS = 1 // not using subsystem clock
    SBS = 8 // Stack in bank 0, MKI mode

    enable_memory_bank()
    select_memory_bank(0)
    // zero all of memory bank 0 (except for the registers)
    for (u8 HL = 8; HL != 0; HL++) {
        *HL = 0;
    }

    // REALLY disable interrupts
00a3:
    disable_interrupts()
    disable_memory_bank()
    start_basic_timer()
    SP = 0 // initialize the stack pointer to 0 (or well, 256 but overflow). the stack grows downwards
    initialize_gpio_and_interrupts()
    check_environment()
    initialize_eeprom()
    reconfigure_timers()
    enable_interrupt(IET0)
    enable_interrupts()

00BD:
    disable_memory_bank()
    select_memory_bank(0)
    var_04B.0 = 0

    // hmm, what's going on here? this pin is routed out on a connector
    if (!(PORT0.0 || PORT0.0 || PORT0.0 || PORT0.0)) {
        goto 01de;
    }

    var_09 = 0
    var_08 = 4

    do {
        set_0e_to_5()
        if (var_2D.3 || !PORT0.0) {
            goto lbl_0177
        }
    } while var_08.2

    sub_0418()



    todo...
}

fn sub_0418() -> skip {
    enable_memory_bank()
    select_memory_bank(0)
    var_42w = 0x20

    HL = 0
    B = 6

start:
    push hl
    if (read_from_bank_1(HL) == 0xF && read_from_bank_1(HL + 1) == 0xF) {
        goto error
    }

    var_44w = read_from_bank_1(HL + 2)
    var_80w = read_from_bank_1(HL + 4)
    L += 6

    C = 5
    XA = 0x10
    loop {
        var_28w = XA
        A = *var_28w
        X = *(var_28w + 1)

        if !sub_04cb(HL, XA) {
            goto error
        }

        HL += 4
        XA = var_28w
        A += 2

        if C == 0 {
            break
        }

        C -= 1
    }

    if var_80.0 && (var_1C > 0x2C) {
        goto error;
    }

    if not var_80.2 {
        goto error;
    }

    if var_1e != 0xF {
        goto error;
    }

    if var_80.3 {
        if (PORT0.0 || PORT0.0 || PORT0.0) {
            loop {
                if (PORT1.0) {
                    if (PORT1.0 && PORT1.0 && PORT1.0) {
                        goto error;
                    }
                    continue
                } else if !PORT1.0 && !PORT1.0 && !PORT1.0 {
                    break
                }
            }
        }
    }

    pop HL
    X = 3
    A = (H >> 1) + 1
    var_42w = XA
    var_34w = read_from_bank_1(HL);
    return true

error:
    pop hl
    H += 2
    if (B == 0) {
        return false
    }
    B -= 1
    goto start
}

fn sub_04cb(HL, XA) -> skip {
    if (XA > read_from_bank_1(HL)) {
        if (read_from_bank_1(HL + 2) > XA) {
            return true
        }
    }
    return false
}

// literally sets 0e to 5
fn set_0e_to_5() {
    var_0e = 5
}

// asserts the eeprom CS, then waits until MISO goes high
fn assert_eeprom_cs(B) {
    PORT2 = B
    nop()
    nop()
    while(!(PORT1 & 4));
    nop()
    nop()
    nop()
}

// checks the IO for sanity
// 12V needs to be above  10.2 V, both oscillators above 3.8V amplitude, and the photoswitch must not be blocked
fn check_environment() {
    do {
        disable_memory_bank()
        restart_basic_timer()
        do {
            adc_start_read(3)
            XA = adc_finish_read(3)
        } while XA < 0xA7;

        C = 0
        adc_start_read(0)
        XA = adc_finish_read(0)
        if XA < 0xC3 {
            C |= 1
        }

        adc_start_read(1)
        XA = adc_finish_read(1)
        if XA < 0xC3 {
            C |= 2
        }

        if (PORT1.3 && PORT1.3 && PORT1.3) {
            C |= 4
        }

    } while C;
}

// reads the EEPROM to memory bank 1, and verifies it a second time
// retries until success
fn initialize_eeprom() {
    do {
        disable_memory_bank()
        read_eeprom_to_bank_1()
        validate_eeprom_bank_1()
    } while CY;
    disable_memory_bank()
    var_02C = 0xFF
}

// sets GPIO settings, disables all interrupts
fn initialize_gpio_and_interrupts() {
    INTA = 0
    INTC = 0
    INTE = 0
    INTF = 0
    INTG = 0
    INTH = 0
    PORT2 = 0
    PORT3 = 0
    PORT6 = 0xF // all port6 pins are set high
    PMGA = 0xFF // port 3 and 6 are all outputs
    PMGB = 0x04 // port 4 and 5 are inputs, port 2 is output
}

// reconfigures watch mode and timer 0
fn reconfigure_timers() {
    configure_watch_mode(0)
    TMOD0 = 0xFA
    TM0 = 0x7C
}

// resets the basic timer
fn reconfigure_timers() {
    disable_memory_bank()
    reset_basic_timer()
}

// returns what's stored at var_30
fn get_var_30() -> A { // returns in A. skips instruction after returning
    // MB = unknown
    A = var_30;
}







// occasionally sends a write disable, and reads out an eeprom word
fn sync_eeprom_to_bank_1() {
    if !var_2c.0 || var_4b.0 {
        return
    }
    if (var_38w & 0xF) == 0 && !var_4b.1 {
        var_4b.1 = 1
        send_eeprom_ewds()
    }
    var_4b.1 = 0
    B,C,D,E = get_eeprom_symbols()
    HL = var_38w
    var_38w = (var_38w + 4) & 0xFC

    enable_memory_bank()

    select_memory_bank(0xF)
    send_eeprom_command(6, HL, B, D)
    read_eeprom_reply(B, C, E)
    write_bank1(HL, BSB[0:1])
    write_bank1(HL + 2, BSB[2:3])
}

// reads the eeprom and copies over its contents to memory bank 1 (all 256 nibbles)
fn read_eeprom_to_bank_1() {
    B, C, D, E = get_eeprom_symbols()
    enable_memory_bank()
    select_memory_bank(1)
    HL = 0
    do {
        reconfigure_timers()
        send_eeprom_command(6, HL, B, D)
        read_eeprom_reply(B, C, E)
        *HL = BSB[0:1]
        HL += 2
        *HL = BSB[2:3]
        HL += 2
    } while HL != 0
}

// Does another round of fetches from the EEPROM, and checks if it matches
// what came in last time
fn validate_eeprom_bank_1() {
    enable_memory_bank()
    select_memory_bank(1)
    HL = 0
    do {
        B, C, D, E = reconfigure_timers()
        if validate_eeprom_single_load(HL, B, C, D, E) {
            CY = 1
            return
        }

        H += 1
    } while H != 0
    CY = 0
}

// loads 4x4nibbles of data from the eeprom and checks if it matches the
// recorded data
fn validate_eeprom_single_load(HL, B, C, D, E) {
    CY = 1
    select_memory_bank(1)
    do {
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
    } while L != 0;
    CY = 0
    return;
}

// writes the contents of bank 1 to the EEPROM
fn write_eeprom_from_bank_1() {
    B, C, D, E = get_eeprom_symbols()
    select_memory_bank(1)
    HL = 0
    do {
        reset_basic_timer()
        write_eeprom_four_writes(HL, B, D)
        H += 1
    } while H != 0
}

// does a 4 EEPROM writes
fn write_eeprom_four_writes(HL, B, D) {
    do {
        select_memory_bank(0xF)
        assert_eeprom_cs()
        send_eeprom_command(5, H, L, B, D)
        BSB[0:1] = read_bank1(HL)
        BSB[2:3] = read_bank1(HL + 2)
        send_eeprom_data()
        L += 4
    } while L != 0
}

// sends a EWEN command
fn send_eeprom_ewen() {
    B, C, D, E = get_eeprom_symbols()
    H = 0xF
    X = 4
    send_eeprom_command(X, H, L, B, D)
    PORT2 = E
}

// sends a EWDS command
fn send_eeprom_ewds() {
    B, C, D, E = get_eeprom_symbols()
    H = 0x0
    X = 4
    send_eeprom_command(X, H, L, B, D)
    PORT2 = E
}

// bitbangs 10 microwire symbols using the bit shuffle buffer. 
fn send_eeprom_command(X, H, L, B, D) {
    BSB[3] = X
    BSB[2] = H
    BSB[1] = L
    L = 0xF
    do {
        if BSB[L >> 2] & (1 << (L & 3)) {
            A = D
        } else {
            A = B
        }
        PORT2 = A
        PORT2 = A | 4
        PORT2 = A & 0xb
        L -= 1
    } while L != 5;
}

// writing microwire bitbang output for 16 cycles
fn send_eeprom_data() {
    L = 0xF
    do {
        if BSB[L >> 2] & (1 << (L & 3)) {
            A = D
        } else {
            A = B
        }
        PORT2 = A
        PORT2 = A | 4
        PORT2 = A & 0xB
        L -= 1
    } while L != 0xF;
    PORT2 = E;
}

// more microwire bitbanging, this time it's taking input for 16 cycles
fn read_eeprom_reply(B, C, E) {
    L = 0xF
    do {
        PORT2 = C
        PORT2 = B
        if (PORT1 & 4) {
            BSL[L >> 2] |= 1 << (L & 3);
        } else {
            BSL[L >> 2] &= ~(1 << (L & 3));
        }
        L--
    } while L != 0xF;
    PORT2 = E;
}

// bitbanging microwire symbols. it reads var_2e because that has the current solenoid output state
// and that GPIO is in the same bank.
// E = all lines low
// B = assert CS
// C = assert CS && assert CLK
// D = assert CS && assert MOSI
fn get_eeprom_symbols() -> B, C, D, E {
    E = var_2e & 1
    B = E | 2
    C = E | 6
    D = E | 0xA
}

// Queues up an ADC read
fn adc_start_read(X) -> X {
    disable_memory_bank();
    ADM = 0x88 | (X << 4);
    while (EOC);
    return X;
}

// Waits until an ADC read has completed, then returns the resulting sample
fn adc_finish_read(X) -> XA {
    disable_memory_bank();
    while (!EOC);
    return SA;
}

// literally just increments XA
fn increment_xa(XA) -> XA {
    return XA + 1
}

// really get variable 30
fn really_get_var_30() -> A {
    A = get_var_30()
}

// does.. something
fn do_bank1_magic() {
    enable_memory_bank()
    select_memory_bank(0)
    var_b0w = 0
    var_b2w = 0
    var_b4w = 0
    var_b6w = 0
    var_b8w = 0
    var_baw = 0
    var_bcw = 0
    var_bew = 0
    var_c0w = 0
    var_c2w = 0
    var_c4w = 0
    var_c6w = 0
    var_caw = 0
    B = 0xF
    HL = 0

    loop {
        push BC
        L = 0
        var_68w = read_bank1(HL)
        add_16bit(0xB0, 0x68)
        L += 2
        var_68w = read_bank1(HL)
        add_16bit(0xB4, 0x68)
        L += 2
        var_68w = read_bank1(HL)
        add_16bit(0xB8, 0x68)
        L += 2
        var_68w = read_bank1(HL)
        add_16bit(0xBC, 0x68)
        L += 2
        var_68w = read_bank1(HL)
        add_16bit(0xC0, 0x68)
        L += 2
        var_68w = read_bank1(HL)
        add_16bit(0xC4, 0x68)
        H += 1
        if B == 0 {
            return
        }
        B -= 1
    }
}

// processes values in 0xB0 - 0xBC, doing some processing with them
// and stuffing the result in 0x50 - 0x60
fn process_magic_lower() {
    enable_memory_bank()
    select_memory_bank(0)
    var_50w = load_table()
    var_52w = load_table()
    var_54w = load_table()
    var_56w = magic_table_add(0xB0, 0x68)
    var_58w = magic_table_subtract(0xB0, 0x68)
    var_5Aw = magic_table_add(0xB4, 0x68)
    var_5Cw = magic_table_subtract(0xB4, 0x68)
    var_5Ew = magic_table_add(0xB8, 0x68)
}

// processes values in 0xB8 - 0xC8, doing some processing with them
// and stuffing the result in 0x50 - 0x60
fn process_magic_upper() {
    enable_memory_bank()
    select_memory_bank(0)
    var_50w = magic_table_subtract(0xB8, 0x68)
    var_52w = magic_table_add(0xBC, 0x68)
    var_54w = magic_table_subtract(0xBC, 0x68)
    var_52w = magic_table_add(0xC0, 0x68)
    var_58w = magic_table_subtract(0xC0, 0x68)
    var_5Aw = magic_table_add(0xC4, 0x68)
    var_5Cw = magic_table_subtract(0xC4, 0x68)
    var_5Ew = 0xFF
}

// swaps two values pointed to by HL and DE
// var_68 contains a 16 bit value
// the next value from the table is loaded, shifted 4 up and subtracted from it
// the subtraction is clamped to 0, shifted right by 4 and returned in XA
fn magic_table_add(HL, DE) -> XA {
    copy_16bit(HL, DE)
    XA = load_table()
    var_64 = 0
    var_65 = A
    var_66 = X
    var_67 = 0
    HL = 0x68
    DE = 0x64
    add_16bit(HL, DE)
    if var_68 == 0 {
        XA = 0Xff
    } else {
        X = var_6A
        A = var_69
    }
}

// swaps two values pointed to by HL and DE
// var_68 contains a 16 bit value
// the next value from the table is loaded, shifted 4 up and subtracted from it
// the subtraction is clamped to 0, shifted right by 4 and returned in XA
fn magic_table_subtract(HL, DE) -> XA {
    copy_16bit(HL, DE)
    XA = load_table()
    var_64 = 0
    var_65 = A
    var_66 = X
    var_67 = 0
    HL = &var_68
    DE = &var_64
    subtract_16bit(HL, DE)
    if var_6B == 0xF {
        XA = 0
    } else {
        X = var_6a
        A = var_69
    }
}


// read a 8 bit value from bank 1
fn read_bank1(HL) -> XA {
    XA = *HL
}

// write a 8 bit value to bank 1
fn write_bank1(HL, XA) {
    *HL = XA
}

// copies 16-bit values
fn copy_16bit(HL, DE) {
    B = 3
    loop {
        *DE = HL
        if B == 0 {
            break;
        }
        B -= 1
    }
}

// 16-bit addition, adds *DE to *HL
fn add_16bit(HL, DE) {
    B = 3
    loop {
        *HL += DE
        L += 1
        E += 1

        if B == 0 {
            break;
        }
        B -= 1
    } 
}

// 16-bit subtract, subtracts *DE from *HL
fn subtract_16bit(HL, DE) {
    B = 3
    loop {
        *HL -= DE
        L += 1
        E += 1

        if B == 0 {
            break;
        }
        B -= 1
    } 
}

static data_1000: u8[0xF * 8] = {
    0x01, 0x00, 0x01, 0x08, 0x07, 0x06, 0x08, 0x08, 0x08, 0x07, 0x05, 0x06, 0x06, 0x08, 0x08,
    0x05, 0x00, 0x00, 0x06, 0x08, 0x07, 0x09, 0x09, 0x09, 0xff, 0xff, 0x06, 0x06, 0x08, 0x08,
    0x10, 0x00, 0x07, 0x07, 0x07, 0x08, 0x08, 0x0a, 0x09, 0xff, 0xff, 0x06, 0x06, 0x08, 0x08,
    0x50, 0x00, 0x05, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x05, 0x06, 0x06, 0x08, 0x08,
    0x50, 0x00, 0x05, 0x06, 0x07, 0x06, 0x06, 0x07, 0x05, 0x06, 0x05, 0x06, 0x06, 0x08, 0x08,
    0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
}

// indexes into data_1000
static data_1078: u8[8] = {0x00, 0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69};

fn load_table_index() {
    // MB = unknown
    XA = data_indices_1078[var_30]
    DE = 0
    var_60 = XA
}

fn load_table() -> XA {
    // MB = unknown
    XA = data_table_1000[var_60w]
    var_60w += 1
}

fn load_from_table_10a8(A) -> XA {
    return data_table_10a8[A]
}

static data_table_10a8: u8[16] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x27,
    0x7f, 0x6f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
}
