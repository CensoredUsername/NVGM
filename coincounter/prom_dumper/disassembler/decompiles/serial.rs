// 0x4c
static mut serial_busy: u4;

// probably prepares for serial comminucation by reconfiguring clocks
// sub_06bc
fn switch_to_serial_mode() {
    disable_interrupts()
    disable_interrupt(IEBT) // disable watchdog interrupt
    disable_interrupt(IET0) // disable T0 interrupt
    RELT = 1 // drive SO
    CSIM = 0x85 // enable serial interface, use T0 output as clock input, 3 line serial IO mode, LSB first
    TMOD0 = 0x6C // T0 max value register set to 108, i.e. divide by 109.
    TM0 = 0x7C // fx/2^4, reset counter, start counting. so serial clock is 4e6 / 2^4 / 109 = ~2300 baud. assume intention as 2400?
    enable_interrupt(IECSI) // enable serial interrupt
    enable_interrupts()
}

// porbably switches back to normal operation clocks from serial comms mode
// sub_06d5
fn switch_from_serial_mode() {
    disable_interrupts()
    disable_interupt(IECSI) // disable serial interrupt
    CSIM = 0x01 // disable serial interface, use T0 output as clock input
    reconfigure_timers()
    IRQBT = 0 // clear any asserted watchdog interrupt
    enable_interrupt(IET0) // enable T0 interrupt
    enable_interrupt(IEBT) // enable watchdog interrupt
    enable_interrupts()
}

// writes the entire contents of bank 1 out over serial, in hexadecimal.
// sub_02e4
fn serial_send_bank_1() {
    select_memory_bank(1)
    HL = 0

    B = 0xF
    loop {
        C = 7
        loop {
            reset_basic_timer()
            XA = read_from_bank_1(HL)
            serial_send_space_and_8bit(XA)

            L += 2
            if C == 0 {
                break
            }
            C -= 1
        }
        serial_send_return()
        H += 1
        if B == 0 {
            break
        }
        B -= 1
    }
}

// serial interrupt. seems to not really be used except to set a single var?
// sub_05cb
fn INTCSI() {
    serial_busy = 0
    enable_interrupt(IECSI)
    enable_interrupts()
}

// sub_0580
fn serial_send_space_and_8bit(XA) {
    BC = XA
    XA = 0x20
    serial_send(XA)
    A = B
    XA = convert_to_ascii(A)
    serial_send(XA)
    A = C
    XA = convert_to_ascii(A)
    serial_send(XA)
}

// sub_0598
fn serial_send_8bit_reversed(XA) {
    BC = XA
    A = C
    XA = convert_to_ascii(A)
    serial_send(XA)
    A = B
    XA = convert_to_ascii(A)
    serial_send(XA)
}

// sub_05ab
fn serial_send_return() {
    XA = 0xD
    serial_send(XA)
}

// sub_05b1
fn serial_send_space() {
    XA = 0x20
    serial_send(XA)
}

// sub_05b7
fn serial_send() {
    XA *= 2
    serial_send_raw(XA)
    XA = 0xFF
}

// writes to the serial output register
// sub_05c1
fn serial_send_raw(XA) {
    while serial_busy;
    serial_busy = 1;
    SIO = XA
    reset_watchdog_next_tick()
}

// sub_05d2
fn serial_wait_until_free() {
    while serial_busy;
}

// converts the hex number stored in A
// to the relevant ascii code in XA
// sub_05d6
fn convert_to_ascii(A) -> XA {
    if (A >= 10) {
        A -= 9
        X = 4
        return
    } else {
        X = 3
        return
    }
}
