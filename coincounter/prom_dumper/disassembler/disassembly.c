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
    sub_067e
    sub_050f
    sub_0563
    sub_069c
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

    todo...
}

// checks the IO for sanity
// 12V needs to be above  10.2 V, both oscillators above 3.8V amplitude, and the photoswitch must not be blocked
fn sub_050f() {
    do {
        disable_memory_bank()
        restart_basic_timer()
        do {
            sub_0d2e(3)
            XA = sub_0d3b(3)
        } while XA < 0xA7;

        C = 0
        sub_0d2e(0)
        XA = sub_0d3b(0)
        if XA < 0xC3 {
            C |= 1
        }

        sub_0d2e(1)
        XA = sub_0d3b(1)
        if XA < 0xC3 {
            C |= 2
        }

        if (PORT1.3 && PORT1.3 && PORT1.3) {
            C |= 4
        }

    } while C;
}

fn sub_0563() {
    do {
        disable_memory_bank()
        sub_0bd8
        sub_0c06
    } while CY;
    disable_memory_bank()
    var_02C = 0xFF
}

// sets GPIO settings, disables all interrupts
fn sub_067e() {
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
fn sub_069c() {
    configure_watch_mode(0)
    TMOD0 = 0xFA
    TM0 = 0x7C
}

// resets the basic timer
fn sub_06b7() {
    disable_memory_bank()
    reset_basic_timer()
}

// returns what's stored at var_30
fn sub_06e9() -> A: u4 { // returns in A. skips instruction after returning
    // MB = unknown
    return var_30;
}

// reads the eeprom and copies over its contents to memory bank 1
fn sub_0bd8() {
    B, C, D, E = sub_0d07()
    enable_memory_bank()
    HL = 0
    do {
        sub_06b7()
        sub_0ca7(6, HL, B, D)
        sub_0ce9(6, HL, B, C, E)
        select_memory_bank(1)
        *HL = BSB[0:1]
        HL += 2
        *HL = BSB[2:3]
        HL += 2
    } while HL != 0
}

// bitbangs 9? microwire symbols using the bit shuffle buffer. 
fn sub_0ca7(X, H, L, B, D) {
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

// more microwire bitbanging, this time it's taking input for 16 cycles
fn sub_0ce9(X, H, L, B, C, E) {
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
fn sub_0d07() -> B, C, D, E {
    E = var_2e & 1
    B = E | 2
    C = E | 6
    D = E | 0xA
}

// Queues up an ADC read
fn sub_0d2e(X) -> X {
    disable_memory_bank();
    ADM = 0x88 | (X << 4);
    while (EOC);
    return X;
}

// Waits until an ADC read has completed, then returns the resulting sample
fn sub_0d3b(X) -> XA {
    disable_memory_bank();
    while (!EOC);
    return SA;
}

// literally just increments XA
fn sub_0f6c(XA) XA {
    return XA + 1
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

fn sub_1080() {
    // MB = unknown
    XA = data_1078[var_30]
    DE = 0
    var_60 = XA
}

fn sub_1094() {
    // MB = unknown
    XA = var_60
    XA = data_1000[XA]
    swap(var_60, XA)
    sub_0f6c()
    swap(var_60, XA)
}

fn sub_109f(A: u4) -> XA {
    return data_10A8[A]
}
