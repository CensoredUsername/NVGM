
// reset handler
fn RESET() {
    disable_interrupts()
    start_basic_timer(0)
    enable_watchdog_timer()
    WM = 6 // enabled, IRQW at 3.91ms intervals

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
    for (u8 HL = 8; HL != 0; HL += 1) {
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

00bd: // start of the main loop
    loop {
        disable_memory_bank()
        select_memory_bank(0)
        suppress_eeprom_sync = 0

        // test if we go into debug mode or operation mode
        if !PORT0.0 && !PORT0.0 && !PORT0.0 && !PORT0.0 {
            goto 01de; // debug functionality

        } else {

            // poke the detector to get ready
            detector_flags_2 = 0 // !both_oscs, !top_osc_going_down, !initialized, !all_data_gathered
            detector_flags_1 = 4

            loop {
                reset_watchdog_next_tick()
                if (port1_readout.3 || !PORT0.0) {
                    // if photosense is getting triggered, or debug mode is being triggered, go to cleanup
                    goto lbl_0177

                } else if detector_await_detection {
                    // detection is ready
                    break
                }
            }

            // figure out if the coin is ok
            if !match_coin_to_config() {
                goto 0177
            }

            reset_watchdog_next_tick()
            // check for IO in error or 
            if !PORT11.2 || !subtract_coin_value_from_cost_remaining() {
                goto 0177
            }

            // trigger the solenoid to try accepting a coin
            reset_watchdog_next_tick()
            solenoid_uptime_remaining = 0x64
            solenoid_active = 1

            // wait for either: detector_detecting_top_osc and 400ms of no photosense, photosense trigger, or 200ms after the solenoid deactivates
            loop {
                if detector_detecting_top_osc {
                    reset_watchdog_next_tick()
                    solenoid_uptime_remaining = 0
                    counter_ticks_remaining = 0x64
                    counter_active = 1
                    while !port1_readout.3 {
                        reset_watchdog_next_tick()
                        if !counter_active {
                            goto 0177
                        }
                    }
                    break
                } else if port1_readout.3 {
                    break
                } else if !solenoid_active {
                    reset_watchdog_next_tick()
                    counter_ticks_remaining = 0x32
                    counter_active = 1
                    while !port1_readout.3 {
                        reset_watchdog_next_tick()
                        if !counter_active {
                            goto 0177
                        }
                    }
                    break
                }
            }

            solenoid_uptime_remaining = 0
            counter_ticks_remaining = 0
            // wait for photosense to go down
            loop {
                reset_watchdog_next_tick()
                if !port1_readout.3 {
                    break
                }
            }
            // disable syncing until enough coins have been thrown in, as to prevent it from resetting cost_remaining
            disable_encoder_sync = 1
            cost_remaining = temps[4:5]
            if temps[4:5] != 0 {
                goto 0177
            }

            // enough money has been entered, activate the relay, and pulse IO out
            IO_OUT_pulses_remaining = 1
            reset_watchdog_next_tick()
            relay_uptime_remaining = 0x32
            relay_active = 1

            // wait until the relay has gone down + 250ms
            loop {
                reset_watchdog_next_tick()
                if !relay_active {
                    break
                }
            }
            wait_250_ms()

            // refresh the encoder readout settings and let tick sync them again
            X = encoder_2_readout
            A = encoder_1_readout
            cost_remaining = XA
            total_cost = XA
            disable_encoder_sync = 0

            // wait for IO_IN to go high again to indicate we're done dispensing cards
            loop {
                reset_watchdog_next_tick()
                if PORT11.2 {
                    break
                }
            }
            goto 0177
        }
    }

0177:
    // cleanup
    // if nothing to do, we can go back
    if !detector_detecting_top_osc {
        detector_flags_2 = 0 // !both_oscs, !top_osc_going_down, !initialized, !all_data_gathered
        detector_flags_1 = 0
        goto 00bd
    }

    // else: wait for detector_detecting_top_osc to go low for at least 400ms
    loop {
        loop {
            reset_watchdog_next_tick()
            if !detector_detecting_top_osc {
                break
            } 
        }

        counter_ticks_remaining = 0x64
        counter_active == 1

        while !detector_detecting_top_osc {
            reset_watchdog_next_tick()
            if !counter_active {
                detector_flags_2 = 0 // !both_oscs, !top_osc_going_down, !initialized, !all_data_gathered
                detector_flags_1 = 0
                goto 00bd
            }
        }
    }



01de:
    // if debug mode disabled, go back to normal functioning
    if PORT0.0 || PORT0.0 || PORT0.0 || PORT0.0 {
        goto 00bd
    }
    // learning mode
    if PORT1.1 || PORT1.1 || PORT1.1 {
        loop {
            if !collect_samples_for_new_config() {
                goto 00a3 // ??????
            }
            disable_interrupts()
            if build_new_config() {
                send_eeprom_ewen()
                reset_watchdog()
                write_eeprom_from_bank_1()
                send_eeprom_ewds()
                reset_watchdog()
                CY = validate_eeprom_bank_1()
                IRQBT = 0
                enable_interrupts()
                if !CY {
                    goto 00bd
                }
                WM = 4 // enable, to 2^14, fx/128, so rollover in 1.9 sec
                IRQW = 0
                port3_gpio_output.0 = 0
                port3_gpio_output.1 = 0
                loop {
                    reset_watchdog_next_tick()
                    if IRQW {
                        break
                    }
                }
                IRQW = 0
                port3_gpio_output.0 = 1
                port3_gpio_output.1 = 1
                loop {
                    reset_watchdog_next_tick()
                    if IRQW {
                        break
                    }
                }
                IRQW = 0
                port3_gpio_output.0 = 0
                port3_gpio_output.1 = 0
                WM = 0 // turn watch mode off

            } else {
                IRQBT = 0
                enable_interrupts()
            }
        }

    // debug mode
    } else {
        A = PORT4 // state of encoder 1 is used to select debug functionality
        A &= 3
        if A == 0 {
            // writes the sample results of a single coin to the serial output
            loop {
                if !wait_for_coin_detected() {
                    goto 00bd
                }
                solenoid_uptime_remaining = 0x64
                match_coin_to_config()
                switch_to_serial_mode()
                PORT2.0 = 1 // activate the solenoid unconditionally
                serial_send(coin_slot_matched)
                serial_send_space()
                serial_send_space_and_8bit(coin_sample_1)
                serial_send_space_and_8bit(coin_sample_2)
                reset_watchdog()
                serial_send_space_and_8bit(coin_sample_3)
                serial_send_space_and_8bit(coin_sample_4)
                serial_send_space_and_8bit(coin_sample_5)
                reset_watchdog()
                serial_send_space_and_8bit(coin_sample_6)
                serial_send_space_and_8bit(detection_tick_count)
                serial_send_space_and_8bit(bottom_osc_went_down_flag | 0x00)
                reset_watchdog()
                serial_send_space_and_8bit(top_osc_went_up_flag | 0x00)
                serial_send_space()
                reset_watchdog()
                serial_send_space_and_8bit(top_osc_max)
                serial_send_space_and_8bit(bottom_osc_max)
                serial_send_return()
                reset_watchdog()
                serial_wait_until_free()
                switch_from_serial_mode()
            }

        } else if A == 1 {
            // writes the contents of  bank 1 (eeprom copy) to the serial output
            switch_to_serial_mode()
            serial_send_bank_1()
            reset_watchdog()
            serial_wait_until_free()
            switch_from_serial_mode()
            loop {
                reset_watchdog()
                if PORT0.0 {
                    goto 00bd
                }
            }
            
        } else if A == 2 {
            // writes the sample results of a single coin to the eeprom at address 0xE0
            loop {
                if !wait_for_coin_detected() {
                    goto 00bd
                }
                solenoid_uptime_remaining = 0x64
                loop {
                    reset_watchdog_next_tick()
                    if port1_readout.3 {
                        break
                    }
                }
                enable_memory_bank()
                select_memory_bank(0)
                HL = 0xE0
                write_bank1(HL, coin_sample_1)
                write_bank1(HL + 2, coin_sample_2)
                write_bank1(HL + 4, coin_sample_3)
                write_bank1(HL + 6, coin_sample_4)
                write_bank1(HL + 8, coin_sample_5)
                write_bank1(HL + 10, coin_sample_6)
                write_bank1(HL + 12, detection_tick_count)
                disable_memory_bank()
                disable_interrupts()
                send_eeprom_ewen()
                enable_memory_bank()
                HL = 0xE0
                write_eeprom_four_writes()
                send_eeprom_ewds()
                disable_memory_bank()
                IRQBT = 0
                BTM.3 = 1
                enable_interrupts()
            }

        } else {
            // writes the contents of memory bank 0 (basically all globals) out over serial
            switch_to_serial_mode()
            select_memory_bank(0)
            HL = 0
            {
                C = 7
                loop {
                    reset_watchdog()
                    enable_memory_bank()
                    XA = *HL
                    disable_memory_bank()
                    serial_send_8bit_reversed(XA)
                    L += 2
                    if C == 0 {
                        break
                    } else {
                        C -= 1
                    }
                }
                serial_send_return()
                if HL == 0 {
                    break
                } else {
                    HL += 16
                }
            }
            serial_wait_until_free()
            switch_from_serial_mode()
            loop {
                reset_watchdog()
                if PORT0.0 {
                    goto 00bd
                }
            }
        }
    }
}

// matches the data collected by the detection routines to config data stored in the eeprom
// it will check 7 eeprom slots for valid data. if it finds a valid one that the coin
// matches, it will return true and assign the value of the coin to coin_value
fn match_coin_to_config() -> skip {
    enable_memory_bank()
    select_memory_bank(0)

    // ascii space
    coin_slot_matched = 0x20
    HL = 0
    B = 6
    // loop through the 7 available slots
    loop {
        'outer: {
            let temp_hl = HL
            // skip slots starting with 0xFF (not a valid coin value)
            if read_from_bank_1(HL) == 0xFF {
                break 'outer
            }
            // hmm we do nothing with this one
            unused = read_from_bank_1(HL + 2)
            coin_matching_flags = read_from_bank_1(HL + 4)

            HL += 6
            C = 5
            XA = &coin_sample_1
            // check if the 6 samples are within range of the 6 pairs of bounds
            loop {
                coin_sample_ptr = XA
                XA = *XA
                if !is_sample_between_config_values(HL, XA) {
                    continue 'outer
                }
                HL += 4
                XA = coin_sample_ptr + 2
                if C == 0 {
                    break;
                } else {
                    C -= 1
                }
            }
            // flags trigger these specific checks
            if coin_matching_flags.0 && detection_tick_count <= 0x2C {
                break 'outer
            }
            if coin_matching_flags.1 && bottom_osc_went_down_flag != 0x0 {
                break 'outer
            }
            if coin_matching_flags.2 && top_osc_went_up_flag != 0xF {
                break 'outer
            }
            // this seems to be related to some functionality not on our board.
            // PORT1.0 should be always high
            if coin_matching_flags.7 {
                if (PORT0.0 || PORT0.0 || PORT0.0) {
                    loop {
                        if (PORT1.0) {
                            if (PORT1.0 && PORT1.0 && PORT1.0) {
                                break 'outer
                            }
                        } else if !PORT1.0 && !PORT1.0 && !PORT1.0 {
                            break
                        }
                    }
                }
            }
            HL = temp_hl
            // convert slot index to ascii 1-based
            coin_slot_matched = 0x30 || ((H >> 1) + 1)
            coin_value = read_from_bank_1(HL)
            return true
        }
        HL = temp_hl + 0x20
        if B == 0 {
            return false
        } else {
            B -= 1
        }
    }
}

// waits for 250 interrupts to happen
fn wait_250_ms() {
    counter_ticks_remaining = 0xFA
    counter_active =1
    loop {
        reset_watchdog_next_tick() 
        if !counter_active {
            break
        }
    }
}

// bcd subtracts coin_value from cost_remaining returns true if this doesn't generate a carry.
// the result is stored in temps[4:5]
fn subtract_coin_value_from_cost_remaining() -> skip {
    if encoder_1_readout == 0 && encoder_2_readout == 0 {
        return false
    }
    temps[4:5] = cost_remaining
    HL = &temps[4]
    A = coin_value[0]
    CY = 0
    HL, CY = bcd_subtract_1digit(HL, A, CY)
    A = coin_value[1]
    HL, CY = bcd_subtract_1digit(HL, A, CY)
    if !CY {
        return true
    } else {
        return false
    }
}

// records all the samples for learning a config
// returns true if successful.
fn collect_samples_for_new_config() -> skip {
    B = 0xF
    HL = 0
    loop {
        if !wait_for_coin_detected() {
            port3_gpio_output.0 = 0 // relay
            port3_gpio_output.1 = 0 // digit 2
            PORT3 = port3_gpio_output
            return false
        }
        solenoid_uptime_remaining = 0x64 // keep the solenoid open for 400ms
        loop {
            reset_watchdog_next_tick()
            if port1_readout.3 {
                break
            }
        }
        port3_gpio_output.0 = 1 // relay
        port3_gpio_output.1 = 1 // digit 2

        write_bank1(HL, coin_sample_1)
        write_bank1(HL + 2, coin_sample_2)
        write_bank1(HL + 4, coin_sample_3)
        write_bank1(HL + 8, coin_sample_4)
        write_bank1(HL + 10, coin_sample_5)
        write_bank1(HL + 12, coin_sample_6)

        HL += 16
        if B == 0 {
            break
        } else {
            B -= 1
        }
    }
    port3_gpio_output.0 = 0 // relay
    port3_gpio_output.1 = 0 // digit 2
    PORT3 = port3_gpio_output
    return true
}

// triggers coin detection mode, and waits for either PORT0.0 to go high (stopping it)
// or the coin detection to happen.
fn wait_for_coin_detected() -> skip {
    detector_flags_2 = 0
    detector_flags_1 = 4

    loop {
        reset_watchdog_next_tick();
        if PORT0.0 && PORT0.0 && PORT0.0 && PORT0.0 {
            detector_flags_1 = 0
            return false
        }
        if !detector_await_detection {
            return true 
        }
    }
}


// returns true if XA is above the value at 1HL, and below the value at 1HL + 2
fn is_sample_between_config_values(HL, XA) -> skip {
    if (XA > read_from_bank_1(HL)) {
        if (read_from_bank_1(HL + 2) > XA) {
            return true
        }
    }
    return false
}

// checks the IO for sanity
// 12V needs to be above  10.2 V, both oscillators above 3.8V amplitude, and the photoswitch must not be blocked
fn check_environment() {
    loop {
        disable_memory_bank()
        restart_basic_timer()
        loop {
            adc_start_read(3)
            if adc_finish_read(3) >= 0xA7 {
                break // testing if the 12V line has gotten high enough (11.2V @ 5.5V)
            }
        }

        C = 0
        adc_start_read(0)
        if adc_finish_read(0) < 0xC3 {
            C |= 1 // testing if both oscillators have reached a sufficiencly high value (4.2V @ 5.5V)
        }

        adc_start_read(1)
        if adc_finish_read(1) < 0xC3 {
            C |= 2 // testing if both oscillators have reached a sufficiencly high value (4.2V @ 5.5V)
        }

        if (PORT1.3 && PORT1.3 && PORT1.3) {
            // photoswitch
            C |= 4
        }
        if !C {
            break
        }
    }
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
    WM = 0 // disable watch timer
    TMOD0 = 0xFA // set max value to 250. That translates to an interrupt every 1.004ms (nice off by one error. lol)
    TM0 = 0x7C // fx/2^4, run, reset.
}

// sets a flag that causes the watchdog to be reset in the next INTT0 call
// sub_06b1
fn reset_watchdog_next_tick() {
    reset_watchdog_next_tick_flag = 5
}

// resets the watchdog timer
// sub_06b7
fn reset_watchdog() {
    BTM.3 = 1 // resets the watchdog timer
}
