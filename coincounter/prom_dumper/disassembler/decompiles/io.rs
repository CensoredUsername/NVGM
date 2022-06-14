// 0x0a
static mut top_osc_readout: u8;
// 0x0c
static mut bottom_osc_readout: u8;

// 0x2e
static mut port2_gpio_output: u4;
// 0x2f
static mut port3_gpio_output: u4;
// 0x2c
static mut port0_readout: u4;
// 0x2d
static mut port1_readout: u4;
// 0x30
static mut encoder1_readout: u4;
// 0x31
static mut encoder2_readout: u4;
// 0x32
static mut port6_readout: u4;

// 0x3e
static mut cost_remaining: u8;

// 0x88
static mut IO_OUT_pulses_remaining: u8;
// 0x8a
static mut IO_OUT_pulse_time_remaining: u8;
// 0x8c
static mut IO_OUT_pulse_state: u4;


// synchronizes the state of all digital IO with the values in memory
// sub_0632
fn update_gpio() {
    PORT2 = port2_gpio_output
    PORT3 = port3_gpio_output
    port0_readout = PORT0
    port1_readout = PORT1
    encoder1_readout = ~PORT4
    encoder2_readout = ~PORT5
    port6_readout = PORT6
}

// synchronizes the state of all digital and analogue io with memory
// sub_0817
fn update_io() {
    adc_start_read(0)
    update_gpio()
    top_osc_readout = adc_finish_read(0)
    adc_start_read(1)
    bottom_osc_readout = adc_finish_read(1)
}

// returns what's stored at encoder1_readout
// sub_06e9
fn get_encoder_readout() -> A, skip { // returns in A. skips instruction after returning
    // MB = unknown
    A = encoder1_readout;
    return true
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
// sub_06ec
fn update_IO_OUT() {
    enable_memory_bank()
    select_memory_bank(0)
    if IO_OUT_pulses_remaining == 0 {
        KR0 = 1

    } else if IO_OUT_pulse_state.1 {
        KR0 = 1

        if IO_OUT_pulse_time_remaining {
            IO_OUT_pulse_time_remaining -= 1
        } else {
            IO_OUT_pulses_remaining -= 1
            IO_OUT_pulse_state = 0
        }

    } else if IO_OUT_pulse_state.0 {
        KR0 = 0

        if IO_OUT_pulse_time_remaining {
            IO_OUT_pulse_time_remaining -= 1
        } else {
            KR0 = 1
            IO_OUT_pulse_time_remaining = 0x32
            IO_OUT_pulse_state.1 = 1
        }

    } else {
        IO_OUT_pulse_time_remaining = 0x32
        IO_OUT_pulse_state.0 = 1
    }
}

// controls the state of the relay
// relay_uptime_remaining is a counter indicating how long it ought to stay activated.
// sub_0661
fn update_relay() {
    if !port0_readout.0 && port1_readout.1 {
        return
    }
    if relay_uptime_remaining {
        relay_uptime_remaining -= 1
        port3_gpio_output.0 = 1 // this is the relay
    } else {
        port3_gpio_output.0 = 0 // this is the relay
        relay_active = 0
    }
}

// controls the state of the solenoid
// solenoid_uptime_remaining is likely a counter indicating how long it ought to stay activated.
// sub_0b77
fn update_solenoid_state() {
    if (solenoid_uptime_remaining) {
        solenoid_uptime_remaining -= 1
        port2_gpio_output |= 1;
    } else {
        solenoid_active = 0
        port2_gpio_output &= 0xE
    }
}

// Queues up an ADC read
// sub_0d2e
fn adc_start_read(X) {
    disable_memory_bank();
    ADM = 0x88 | (X << 4);
    while (EOC);
}

// Waits until an ADC read has completed, then returns the resulting sample
// sub_0d3b
fn adc_finish_read(X) -> XA {
    disable_memory_bank();
    while (!EOC);
    XA = SA;
}
