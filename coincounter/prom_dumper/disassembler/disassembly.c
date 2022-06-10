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
        reset_watchdog_next_INTT0()
        if (port1_readout.3 || !PORT0.0) {
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

// serial interrupt. seems to not really be used except to set a single var?
fn INTCSI() {
    var_3c.0 = 0
    enable_interrupt(IECSI)
    enable_interrupts()
}

// timer 0 interrupt. Seems to do most of the work
fn INTT0() {
    disable_memory_bank()
    TMOD0 = 0xFA
    if reset_watchdog_next_INTT0_flag == 5 {
        reset_watchdog_next_INTT0_flag = 0
        BTM = 8
    }

    var_49 += 1
    update_io()
    sub_082f()
    if !var_49.0 { // evaluated every 2 calls
        update_solenoid_state()
        update_IO_OUT()
        update_relay()
        read_encoder_setting()
        sub_0b64()
    } else if !var_49.1 { // evaluated every 4 calls
        sync_eeprom_to_bank_1()
    } else { // evaluated every 4 calls
        if (!toggle_display_if_external_error()) {
            update_display()
        }
    }

    enable_interrupt(IET0)
    enable_interrupts()
}

fn sub_082f() {
    if (var_08.2) {
        // lbl_0864
        if (var_09.0) {
            if (sub_0a79()) {
                return
            }
            var_09 = 1
            var_4b.0 = 1
            var_10w = top_osc_readout
            var_18w = top_osc_readout
            var_20w = top_osc_readout
            var_24w = top_osc_readout
            var_74w = top_osc_readout
            previous_top_osc_readout = top_osc_readout
            var_70w = bottom_osc_readout
            last_bottom_osc_1 = bottom_osc_readout
            var_12w = top_osc_max
            var_14w = bottom_osc_max
            var_1cw = 0
            var_46 = 0
            var_47 = 0
            var_1e = 0
            var_48 = 0
            osc_count[0:2] = 0x448
            return
        } else {
            increment_osc_count_and_on_rollover()
            if var_09.3 && !both_oscs_above_max() {
                return
            }
            // lbl_08ac

            // todo

        }
    } else if (var_08.0) {
        // lbl_083b
        if !sub_0a79() {
            return
        }
        var_08.0
        var_09 = 0
        osc_count[0:2] = 0x448

    } else if (var_08.1) {
        // lbl_084d
        if increment_osc_count_and_on_rollover() && !both_oscs_above_max() {
            return
        }
        var_08 = 3
        var_09 = 0
        var_4b.0 = 0
        if (PORT0.0) {
            var_08 = 0
        }
    }
}


// implements a rather weird sort of state machine on the IO OUT signal
// so starting from IO_OUT_pulse_state being 0 and IO_OUT_pulses_remaining being 1
// call 0:
//  8aw is set to 0x32 and 8c.0 to 1
// call 1 - 50:
//  KR0 = 0
//  8aw -= 1
// call 51:
//  KR0 = 1
//  8aw = 0x32, 8c.1 = 1
// call 51 - 100
//  KR0 = 1
//  8aw -= 1
// call 101
//  88 -= 1
//  8c = 0
// call 102
//  KR0 = 1
// the next 0x32 calls will just decrement
// so essentially, it will send pulses on IO_OUT until 88 reaches 0.
fn update_IO_OUT() {
    enable_memory_bank()
    select_memory_bank(0)
    if IO_OUT_pulses_remaining == 0 {
        KR0 = 1
        return
    }

    if !IO_OUT_pulse_state.1 {
        if !IO_OUT_pulse_state.0 {
            IO_OUT_pulse_time_remaining = 0x32
            IO_OUT_pulse_state.0 = 1
            return
        }

        KR0 = 0
        if IO_OUT_pulse_time_remaining == 0 {
            KR0 = 1
            IO_OUT_pulse_time_remaining = 0x32
            IO_OUT_pulse_state.1. = 1
        } else {
            IO_OUT_pulse_time_remaining -= 1
        }
    } else {
        KR0 = 1
        if IO_OUT_pulse_time_remaining == 0 {
            if (IO_OUT_pulses_remaining != 0) {
                IO_OUT_pulses_remaining -= 1
            }
            IO_OUT_pulse_state = 0
        } else  {
            IO_OUT_pulse_time_remaining -= 1
        }
    }
}

fn update_relay() {
    if !port0_readout.0 && port1_readout.1 {
        return
    }
    if (relay_uptime_remaining == 0) {
        port3_gpio_output.0 = 0 // this is the relay
        var_4a.0 = 0
        return
    } else {
        port3_gpio_output.0 = 1 // this is the relay
        if relay_uptime_remaining != 0 {
            relay_uptime_remaining -= 1
        }
    }
}

fn read_encoder_setting() {
    if var_4b.3 {
        return;
    }
    X = encoder_2_readout
    A = encoder_1_readout
    var_3ew = XA
    var_60w = XA
}


fn sub_0b64() {
    if var_3cw == 0 {
        var_4a.2 = 0
        return
    } else  {
        var_3cw -= 1
    }
}

fn toggle_display_if_external_error() -> skip {
    enable_memory_bank()
    select_memory_bank(0)

    if PORT11.2 { // IO in high, i.e. no error
        suppress_display_flag = 0

        display_error_timeout[0:1] = 0xE7
        return false
    }

    // toggle the suppress_display_flag every 0xE7 calls to this function
    display_error_timeout[0:1] += 1
    if display_error_timeout == 0 {
        display_error_timeout = 0xE7
        suppress_display_flag ^= 0xF
        return false
    }

    // if the flag is set, zero all the segment digit drivers and skip the logic for driving it
    if suppress_display_flag.0 {
        port3_gpio_output &= 1
    }
    return suppress_display_flag.0
}

fn sub_04cb(HL, XA) -> skip {
    if (XA > read_from_bank_1(HL)) {
        if (read_from_bank_1(HL + 2) > XA) {
            return true
        }
    }
    return false
}

// literally sets 0e to 5. this will stay set to 5 until INTT0 hits, at which point it will reset the watchdog timer
fn reset_watchdog_next_INTT0() {
    reset_watchdog_next_INTT0_flag = 5
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

// returns what's stored at encoder1_readout
fn get_encoder_readout() -> A, skip { // returns in A. skips instruction after returning
    // MB = unknown
    A = encoder1_readout;
    return true
}

// synchronizes the state of all digital IO with the values in memory
fn update_gpio() {
    PORT2 = port2_gpio_output
    PORT3 = var_2f
    port0_readout = PORT0
    port1_readout = PORT1
    encoder1_readout = ~PORT4
    encoder2_readout = ~PORT5
    port6_readout = PORT6
}

// wat
fn update_display() {
    buf0_16bit[0] = var_60
    A = var_3e
    HL = &buf0_16bit
    sub_01cc(A, HL)
    A = var_3f
    sub_01cc(A, HL)
    PORT3 = PORT3 & 1;
    if current_digit == 0 {
        var_41 = 0
    }
    A = current_digit
    C = A
    A += 1
    if (A == 3) {
        A = 0
    }
    current_digit = A
    C -= 1
    if (C == 0xF) {
        A = buf0_16bit[0]
        if A == 0 && PORT11.2 {
            XA = 0
            C = 8
            goto 07d3
        }
    }

    bitbang_serial_8bits(XA)
    nop(6)
    STROBE = 1
    nop(6)
    STROBE = 0
    port3_gpio_output = (port3_gpio_output & 1) | C
    PORT3 = port3_gpio_output
}

//? idk what this does. it confuses me
fn sub_01cc(A, HL) -> HL {
    temp = *HL - A
    *HL = A
    temp += 0xA
    if temp >= 0xA {
        *HL = temp
    }
    L += 1
}

// bitbang 8 bits to the serial IO expander
fn bitbang_serial_8bits(XA) {
    bitbang_serial_4bits(X)
    bitbang_serial_4bits(A)
}

// bitbang 4 bits to the serial IO expander
fn bitbang_serial_4bits(A) {
    B = 3
    loop {
        if (A & 8) {
            SDI = 1
        } else {
            SDI = 0
        }
        A <<= 1
        nop()
        nop()
        nop()
        nop()
        nop()
        nop()
        CLK = 0
        nop()
        nop()
        nop()
        nop()
        nop()
        nop()
        CLK = 1
        if B != 0 {
            B -= 1
        } else {
            break
        }
    }
}

// synchronizes the state of all digital and analogue io with memory
fn update_io() {
    adc_start_read(0)
    update_gpio()
    top_osc_readout = adc_finish_read(0)
    adc_start_read(1)
    bottom_osc_readout = adc_finish_read(1)
}

// ok this one is huge
fn sub_082f() {
    // todo
}

// sets 46.1 and 46.2 based on bottom_osc_readout being larger or smaller than last_bottom_osc_1
// with some debouncing
fn sub_099e() {
    if (last_bottom_osc_1 < bottom_osc_readout) {
        if !var_46.2 {
            if last_bottom_osc_1 >= bottom_osc_readout - 8 {
                return
            }
            var_46.2 = 1
            var_46.1 = 1
            var_12w = top_osc_max
        }
        last_bottom_osc_1 = bottom_osc_readout
        return
    } else {
        if var_46.2 {
            if last_bottom_osc_1 < bottom_osc_readout + 8 {
                return;
            }
            var_46.2 = 0
            var_46.1 = 0
        }
        last_bottom_osc_1 = bottom_osc_readout
        return
    }

}

// sets 47.2 and var_1e based on if last_bottom_osc_2 > bottom_osc_readout
// with some debouncing
fn sub_0a08() {
    if (last_bottom_osc_2 < bottom_osc_readout) {
        if !var_47.2 {
            if (bottom_osc_readout < last_bottom_osc_2) {
                return
            }
            var_47.2 = 1
        }
        last_bottom_osc_2 = bottom_osc_readout
        return
    } else {
        if var_47.2 {
            if (last_bottom_osc_2 < bottom_osc_readout + 2) {
                return
            }
            var_47.2 = 0
            var_1e = 0xF
        }
        last_bottom_osc_2 = bottom_osc_readout
    }
}

// saturating increment to var_1cw
fn sub_0a4f() {
    if (var_1cw != 0xFF) {
        var_1cw += 1
    }
}

// increments the 12-bit value in 4d.4e.4f. if it rolls over, sets a bunch of values
fn increment_osc_count_and_on_rollover() {
    osc_count[0:2] += 1
    if osc_count[0:2] != 0 {
        return
    }
    
    var_08 = 4
    var_09 = 0
    var_46 = 0
    var_4b.0 = 0
}

// definitely the core of some signal processing
fn sub_0a79() -> skip {
    if !var_09.1 {

        top_osc_max = top_osc_readout
        bottom_osc_max = bottom_osc_readout
        var_09.1 = 1
        var_09.2 = 0
        var_08.3 = 0
        osc_count[0:2] = 0xE00
        return false
    }

    if !var_09.2 {
        // if current top oscillator reading > max
        // update max from both
        if top_osc_readout > top_osc_max {
            top_osc_max = top_osc_readout
            bottom_osc_max = bottom_osc_readout
            return false
        }

        // if top_osc_readout drops 12 below the max
        // probably a coin has been thrown in, start processing it
        if top_osc_readout + 0xC <= top_osc_max {
            var_09.2 = 1
            var_08.3 = 1
            previous_top_osc_readout = top_osc_readout
            var_1f = 0
            var_46 = 0
            osc_count[0:1] = 0x00
            return false
        }

        // if we've gone 0xFFF samples without anything happening
        // flag to re-initialize next cycle
        osc_count[0:2] += 1
        if osc_count[0:2] == 0 {
            var_09.1 = 0
        }
        return false
    }

    // 
    if top_osc_above_max() {
        var_09.1 = 0
        return false
    }

    if osc_count[0:1] != 0xFF {
        osc_count[0:1] += 1
    }

    sub_0b10()

    if (bottom_osc_readout > bottom_osc_max) {
        return false
    }
    if (bottom_osc_readout < bottom_osc_max + 0xC) {
        return false
    }

    if osc_count[0:1] < 6 {
        var_09.1 = 0
        return false
    } else {
        return true
    }

}

// compares the value in 0a to the value in 2a.
// if it's more than 4 larger than the one in 2a it sets 46.1 and assigns 0xF to 1F
// if it's more than 4 smaller than the one in 2a it resets 46.1
// then it sets 2a to 0a
// so essentially it detects if the value in 0a is descending across multiple calls
// or ascending over multiple calls 
fn sub_0b10() {
    if (top_osc_readout > previous_top_osc_readout) {
        if (!var_46.0) {
            if ((top_osc_readout - previous_top_osc_readout) <= 4) {
                return
            }

            var_46.0 = 1
            var_1F = 0xF
        }
        previous_top_osc_readout = top_osc_readout
    } else {
        if (var_46.0) {
            if ((previous_top_osc_readout - top_osc_readout) < 4) {
                return
            }

            var_46.0 = 0
        } else {
            previous_top_osc_readout = top_osc_readout
        }
    }
}

fn bottom_osc_above_max() -> skip {
    return bottom_osc_above_max() && top_osc_above_max()
}

// returns false when bottom_osc_readout drops below the threshold
fn bottom_osc_above_max() -> skip {
    if (bottom_osc_readout > bottom_osc_max) {
        return true;
    } else if bottom_osc_readout > bottom_osc_max - 6 {
        return true
    }
    return false
}

// returns false when top_osc_readout drops below the threshold
fn top_osc_above_max() -> skip {
    if (top_osc_readout > top_osc_max) {
        return true;
    } else if top_osc_readout > top_osc_max - 6 {
        return true
    }
    return false
}

// hmm?
fn sub_0b64() {
    if (var_3cw) {
        var_3cw -= 1
    } else {
        var_4A.2 = 0
    }
}

// controls the state of the solenoid
// solenoid_uptime_remaining is likely a counter indicating how long it ought
// to stay activated. var_4A.2 
fn update_solenoid_state() {
    if (solenoid_uptime_remaining) {
        solenoid_uptime_remaining -= 1
        port2_gpio_output |= 1;
    } else {
        var_4A.2 = 0
        port2_gpio_output &= 0xE
    }
}


// occasionally sends a write disable, and reads out an eeprom word
fn sync_eeprom_to_bank_1() {
    if !port0_readout.0 || var_4b.0 {
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

// bitbanging microwire symbols. it reads port2_gpio_output because that has the current solenoid output state
// and that GPIO is in the same bank.
// E = all lines low
// B = assert CS
// C = assert CS && assert CLK
// D = assert CS && assert MOSI
fn get_eeprom_symbols() -> B, C, D, E {
    E = port2_gpio_output & 1
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
fn really_get_encoder_readout() -> A {
    A = get_encoder_readout()

    .. TODO
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
        buf1_16bit[0:1] = read_bank1(HL)
        add_16bit(0xB0, &buf1_16bit)
        L += 2
        buf1_16bit[0:1] = read_bank1(HL)
        add_16bit(0xB4, &buf1_16bit)
        L += 2
        buf1_16bit[0:1] = read_bank1(HL)
        add_16bit(0xB8, &buf1_16bit)
        L += 2
        buf1_16bit[0:1] = read_bank1(HL)
        add_16bit(0xBC, &buf1_16bit)
        L += 2
        buf1_16bit[0:1] = read_bank1(HL)
        add_16bit(0xC0, &buf1_16bit)
        L += 2
        buf1_16bit[0:1] = read_bank1(HL)
        add_16bit(0xC4, &buf1_16bit)
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
    var_56w = magic_table_add(0xB0, &buf1_16bit)
    var_58w = magic_table_subtract(0xB0, &buf1_16bit)
    var_5Aw = magic_table_add(0xB4, &buf1_16bit)
    var_5Cw = magic_table_subtract(0xB4, &buf1_16bit)
    var_5Ew = magic_table_add(0xB8, &buf1_16bit)
}

// processes values in 0xB8 - 0xC8, doing some processing with them
// and stuffing the result in 0x50 - 0x60
fn process_magic_upper() {
    enable_memory_bank()
    select_memory_bank(0)
    var_50w = magic_table_subtract(0xB8, &buf1_16bit)
    var_52w = magic_table_add(0xBC, &buf1_16bit)
    var_54w = magic_table_subtract(0xBC, &buf1_16bit)
    var_52w = magic_table_add(0xC0, &buf1_16bit)
    var_58w = magic_table_subtract(0xC0, &buf1_16bit)
    var_5Aw = magic_table_add(0xC4, &buf1_16bit)
    var_5Cw = magic_table_subtract(0xC4, &buf1_16bit)
    var_5Ew = 0xFF
}

// swaps two values pointed to by HL and DE
// buf1_16bit[0] contains a 16 bit value
// the next value from the table is loaded, shifted 4 up and subtracted from it
// the addition is clamped to 0xFF, shifted right by 4 and returned in XA
fn magic_table_add(HL, DE) -> XA {
    copy_16bit(HL, DE)
    XA = load_table()
    buf0_16bit[0:3] = {0, A, X, 0}
    add_16bit(&buf1_16bit, &buf0_16bit)
    if buf1_16bit[0] == 0 {
        XA = 0xFF
    } else {
        X = buf1_16bit[2]
        A = buf1_16bit[1]
    }
}

// swaps two values pointed to by HL and DE
// buf1_16bit[0] contains a 16 bit value
// the next value from the table is loaded, shifted 4 up and subtracted from it
// the subtraction is clamped to 0, shifted right by 4 and returned in XA
fn magic_table_subtract(HL, DE) -> XA {
    copy_16bit(HL, DE)
    XA = load_table()
    buf0_16bit[0:3] = {0, A, X, 0}
    add_16bit(&buf1_16bit, &buf0_16bit)
    if buf1_16bit[3] == 0xF {
        XA = 0
    } else {
        X = buf1_16bit[2]
        A = buf1_16bit[1]
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
    XA = data_indices_1078[encoder1_readout]
    DE = 0
    var_60w = XA
}

fn load_table() -> XA {
    // MB = unknown
    XA = data_table_1000[var_60w]
    var_60w += 1
}

fn get_7seg_encoding(A) -> XA {
    return seg7_encoding_table[A]
}

// this is a table of hex value to 7 segment encoding
// the relevant bits are as follows
//  00
// 5  1
// 5  1
//  66
// 4  2
// 4  2
//  33  7
static seg7_encoding_table: u8[16] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x27,
    0x7f, 0x6f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
}


