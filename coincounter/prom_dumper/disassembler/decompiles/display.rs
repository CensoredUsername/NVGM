// this is a table of hex value to 7 segment encoding
// the relevant bits are as follows
//  00
// 5  1
// 5  1
//  66
// 4  2
// 4  2
//  33  7
// data_10a8
static seg7_encoding_table: u8[16] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x27,
    0x7f, 0x6f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// used to keep track of which digit we're driving this cycle
// only has values 0, 1 or 2
// 0x40
static mut current_digit: u4;

// used to keep track of if the second digit should be displayed
// contains the value of the first digit
// 0x41
static mut digit_0_value: u4;

// true if the display is currently being suppressed due to an error
// 0x85
static mut suppress_display_flag: u4;

// time until the display will be toggled if an error is active
// 0x86
static mut display_error_timeout: u8;

// indexes into seg7_encoding_table and returns the result
// sub_109f
fn get_7seg_encoding(A) -> XA {
    return seg7_encoding_table[A];
}

// bitbang 4 bits to the serial IO expander
// sub_07f6
fn bitbang_serial_4bits(A) {
    B = 3
    loop {
        if (A & 8) {
            SDI = 1
        } else {
            SDI = 0
        }
        A <<= 1
        nop(6)
        CLK = 0
        nop(6)
        CLK = 1
        if B == 0 {
            return
        }
        B -= 1
    }
}

// bitbang 8 bits to the serial IO expander
// sub_07f1
fn bitbang_serial_8bits(XA) {
    bitbang_serial_4bits(X -> A)
    bitbang_serial_4bits(A)
}

// updates the coin tester display
// sub_0774
fn update_display() {
    // this stores the configured cost as by the encoders
    temps[0:1] = total_cost

    HL = &temps[0]

    // subtract the current value from the setting on the encoders
    A = cost_remaining[0]
    CY = 0
    HL, CY = bcd_subtract_1digit(HL, A, CY)
    A = cost_remaining[1]
    HL, CY = bcd_subtract_1digit(HL, A, CY)

    // disable driving any of the segments
    PORT3 = PORT3 & 1;

    // increment current digit for the next call
    C = current_digit
    current_digit += 1
    if current_digit == 3 {
        current_digit = 0
    }

    if (C == 0) {
        // digit 0 is passed in temps[1]
        A = temps[1]
        C = 8
        digit_0_value = A
        if (A == 0 && PORT11.2) { // keep the display zero'd as long IO_IN is set
            XA = 0x00
        } else {
            XA = get_7seg_encoding(A)
        }

    } else if (C == 1) {
        // digit 1 is passed in temps[0]
        A = temps[0]
        C = 4
        if A == 0 && PORT11.2 { // keep the display zero'd as long IO_IN is set
            if digit_0_value == 0 {
                XA = 0x00
            } else {
                XA = get_7seg_encoding(0)
            }
        } else {
            XA = get_7seg_encoding(A)
        }
    } else {
        // digit 2 is always 0
        XA = 0x3F
        C = 2
    }

    // write the data to the serial IO expander
    bitbang_serial_8bits(XA)
    // pulse the STROBE to latch it over
    nop(6)
    STROBE = 1
    nop(6)
    STROBE = 0
    // and drive the correct digit
    port3_gpio_output = (port3_gpio_output & 1) | C
    PORT3 = port3_gpio_output
}

// should be called to determine if update_display should be called.
// if IO in is high, i.e. stuff is fine it'll always return false
// otherwise, it'll toggle the result every 24 calls (it's called every 4 ms)
// sub_0741
fn toggle_display_if_external_error() -> skip {
    enable_memory_bank()
    select_memory_bank(0)

    if PORT11.2 { // IO in high, i.e. no error
        suppress_display_flag = 0

        display_error_timeout = 0xE7
        return false
    }

    // toggle the suppress_display_flag every 0xE7 calls to this function
    display_error_timeout += 1
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
