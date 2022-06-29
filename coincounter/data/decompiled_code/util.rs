// often (but not always) used as two u16 values as well
// 0x64
static mut temps: [u8; 4];

// read a 8 bit value from bank 1
// sub_0ef8
fn read_bank1(HL) -> XA {
    enable_memory_bank();
    select_memory_bank(1);
    XA = *HL
}

// write a 8 bit value to bank 1
// sub_0eff
fn write_bank1(HL, XA) {
    enable_memory_bank();
    select_memory_bank(1);
    *HL = XA
}

// copies 16-bit values from HL to DE
// sub_0f06
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
// sub_0f48
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
// sub_0f57
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

// one digit of BCD subtraction
// sub_01cc
fn bcd_subtract_1digit(HL, A, CY) -> HL, CY {
    let temp = *HL - A - CY
    CY = A + CY > HL
    if CY {
        temp += 10
    }
    *HL = temp
    L += 1
}

// increments XA. no rollover handling or anything
// sub_0f6c
fn increment_xa(XA) -> XA {
    XA += 1
}

// pseudo functions, just used to translate otherwise untranslatable assembly instructions
// interrupt handlig
fn disable_interrupts() {
    asm DI;
}

fn enable_interrupts() {
    asm EI;
}

fn enable_interrupt(IExxx) {
    asm EI IExxx
}

fn disable_interrupt(IExxx) {
    asm DI IExxx
}

// memory bank handling
fn disable_memory_bank() {
    asm CLR1 MBE
}

fn enable_memory_bank() {
    asm SET1 MBE
}

fn select_memory_bank(MBn) {
    asm SEL MBn
}

// other
fn enable_watchdog_timer() {
    asm SET1 WDTM.3
}

fn start_basic_timer(setting) {
    BTM = 8 | setting
}

fn restart_basic_timer() {
    asm SET1 BTM.3
}