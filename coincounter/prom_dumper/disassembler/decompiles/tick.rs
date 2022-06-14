// set this flag to 5 to reset the watchdog on the next INTT0 event
// 0x0e
static mut reset_watchdog_next_tick_flag: u4;

// 0x3c
static mut interrupts_remaining: u8;


// timer 0 interrupt. Seems to do most of the work. Gets called every millisecond
fn INTT0() {
    disable_memory_bank()
    TMOD0 = 0xFA  // set max value to 250. That translates to an interrupt every 1.004ms (nice off by one error. lol)

    // handle watchdog reset requests
    if reset_watchdog_next_tick_flag == 5 {
        reset_watchdog_next_tick_flag = 0
        BTM = 8 // reset watchdog
    }

    task_scheduling_counter += 1
    update_io()
    udpate_detector()
    if !task_scheduling_counter.0 { // evaluated every 2 calls
        update_solenoid_state()
        update_IO_OUT()
        update_relay()
        read_encoder_setting()
        decrement_counter()
    } else if !task_scheduling_counter.1 { // evaluated every 4 calls
        sync_eeprom_to_bank_1()
    } else { // evaluated every 4 calls
        if (!toggle_display_if_external_error()) {
            update_display()
        }
    }

    // re-enable the interrupt
    enable_interrupt(IET0)
    enable_interrupts()
}


fn update_detector() {
    if detector_await_detection {
        if detector_detecting_both_oscs {
            // timeout check
            increment_osc_count_and_reset_on_rollover()
            // if all data has been gathered, just wait until both oscillators return to their normal values
            if detector_all_data_gathered && !both_oscs_near_or_above_max() {
                return
            }
            // coin detection finished, calculate final values and return
            if both_oscs_near_or_above_max() {
                // sample 4 is the saturated value of sample_2 - sample_1
                if coin_sample_2 > coin_sample_1 {
                    coin_sample_4 = 0
                } else {
                    coin_sample_4 = coin_sample_1 - coin_sample_2
                }
                // sample 6 is the saturated value of sample_5 - sample_1
                if coin_sample_1 > coin_sample_5 {
                    coin_sample_6 = 0
                } else {
                    coin_sample_6 = coin_sample_5 - coin_sample_1
                }
                detector_flags_1 = 3
                detector_flags_2 = 0  // !both_oscs, !top_osc_going_down, !initialized, !all_data_gathered
                suppress_eeprom_sync = 0
                if !PORT0.0 {
                    detector_flags_1 = 0
                }
                return
            }
            // sample 3 is the minimum value reached by the bottom osc
            if bottom_osc_readout < coin_sample_3 {
                coin_sample_3 = bottom_osc_readout
            }
            if bot_osc_going_up_coarse_copy {
                // sample 2: the minimum value reached by the top osc while bot osc is going up
                if top_osc_readout < coin_sample_2 {
                    coin_sample_2 = top_osc_readout
                }
            } else {
                // sample 5: the maximum value reached by the top osc while bot osc is going down
                if top_osc_readout > coin_sample_5 {
                    coin_sample_5 = top_osc_readout
                }
            }
            // update direction trackers
            track_bottom_osc_direction_coarse()
            track_bottom_osc_direction_fine()
            increment_detection_tick_count()
            // max top osc value
            if top_osc_readout > top_osc_max_2 {
                top_osc_max_2 = top_osc_readout
                detector_initialized = 0 // this flag is abused here to be something else
            } else if !detector_initialized && (top_osc_readout - top_osc_max_2) & 0xF >= 8 {
                detector_initialized = 1
                unused_24 = top_osc_max_2
                top_osc_below_max_events += 1
                if top_osc_below_max_events == 3 {
                    detector_all_data_gathered = 1
                    return
                }
            }
            if bottom_osc_readout > last_bottom_osc_rough {
                if !top_osc_going_up {
                    if bottom_osc_readout > last_bottom_osc_rough + 0x18 {
                        top_osc_going_up = 1
                        last_bottom_osc_rough = bottom_osc_readout
                    }
                } else {
                    last_bottom_osc_rough = bottom_osc_readout
                }
            } else {
                if top_osc_going_up {
                    if bottom_osc_readout < last_bottom_osc_rough - 0x18 {
                        top_osc_going_up = 0
                        detector_all_data_gathered = 1
                        last_bottom_osc_rough = bottom_osc_readout
                    }
                } else {
                    last_bottom_osc_rough = bottom_osc_readout
                }
            }
            return

        } else if detect_incoming_coins() {
            detector_flags_2 = 1 // both_oscs, !top_osc_going_down, !initialized, !all_data_gathered
            suppress_eeprom_sync = 1
            // sample 1: the readout of top osc after a coin has been detected
            coin_sample_1 = top_osc_readout
            coin_sample_5 = top_osc_readout
            top_osc_max_2 = top_osc_readout
            unused_24 = top_osc_readout
            unused_74 = top_osc_readout
            previous_top_osc_readout = top_osc_readout

            last_bottom_osc_rough = bottom_osc_readout
            last_bottom_osc_coarse = bottom_osc_readout

            coin_sample_2 = top_osc_max
            coin_sample_3 = bottom_osc_max
            detection_tick_count = 0
            osc_going_up_flags_1 = 0
            osc_going_up_flags_2 = 0
            bottom_osc_went_down_flag = 0
            top_osc_below_max_events = 0
            osc_count[0:2] = 0x448 // 3000 ticks till rollover
            return

        }

    } else if detector_guard_additional_coins {
        // 
        if detect_incoming_coins() {
            detector_guard_additional_coins = 0
            detector_flags_2 = 0 // !both_oscs, !top_osc_going_down, !initialized, !all_data_gathered
            osc_count[0:2] = 0x448 // 3000 ticks till rollover
        }

    } else if detector_guard_await_timeout {
        // possible bug in the code: this seems to be intended to branch but the function returns negative unconditionally
        increment_osc_count_and_reset_on_rollover()
        detector_flags_1 = 3
        detector_flags_2 = 0 // !both_oscs, !top_osc_going_down, !initialized, !all_data_gathered
        suppress_eeprom_sync = 0
        if !PORT0.0 {
            detector_flags_1 = 0 
        }
    }
}

// sets 46.1 and 46.2 based on bottom_osc_readout being larger or smaller than last_bottom_osc_coarse
// with some debouncing
fn track_bottom_osc_direction_coarse() {
    if (bottom_osc_readout > last_bottom_osc_coarse) {
        if !bot_osc_going_up_coarse {
            if bottom_osc_readout <= last_bottom_osc_coarse + 8 {
                return
            }
            bot_osc_going_up_coarse = 1
            bot_osc_going_up_coarse_copy = 1
            coin_sample_2 = top_osc_max
        }
        last_bottom_osc_coarse = bottom_osc_readout
        return
    } else {
        if bot_osc_going_up_coarse {
            if bottom_osc_readout > last_bottom_osc_coarse - 8 {
                return;
            }
            bot_osc_going_up_coarse = 0
            bot_osc_going_up_coarse_copy = 0
        }
        last_bottom_osc_coarse = bottom_osc_readout
        return
    }

}

// sets 47.2 and bottom_osc_went_down_flag based on if last_bottom_osc_fine > bottom_osc_readout
// with some debouncing
fn track_bottom_osc_direction_fine() {
    if (bottom_osc_readout > last_bottom_osc_fine) {
        if !bot_osc_going_up_fine {
            if (bottom_osc_readout < last_bottom_osc_fine) {
                return
            }
            bot_osc_going_up_fine = 1
        }
        last_bottom_osc_fine = bottom_osc_readout
        return
    } else {
        if bot_osc_going_up_fine {
            if (bottom_osc_readout > last_bottom_osc_fine - 2) {
                return
            }
            bot_osc_going_up_fine = 0
            bottom_osc_went_down_flag = 0xF
        }
        last_bottom_osc_fine = bottom_osc_readout
    }
}

// saturating increment to detection_tick_count
fn increment_detection_tick_count() {
    if (detection_tick_count != 0xFF) {
        detection_tick_count += 1
    }
}

// increments the 12-bit value in 4d.4e.4f. if it rolls over, sets a bunch of values
fn increment_osc_count_and_reset_on_rollover() {
    osc_count[0:2] += 1
    if osc_count[0:2] != 0 {
        return
    }

    detector_flags_1 = 4 // !?, !?, await_detection, !top_osc
    detector_flags_2 = 0 // !both_oscs, !top_osc_going_down, !initialized, !all_data_gathered
    osc_going_up_flags = 0
    suppress_eeprom_sync = 0
}

// definitely the core of some signal processing
fn detect_incoming_coins() -> skip {
    // if this flag hasn't been set, initialize initial values
    if !detector_initialized {
        top_osc_max = top_osc_readout
        bottom_osc_max = bottom_osc_readout
        detector_initialized = 1
        detector_top_osc_going_down = 0
        detector_detecting_top_osc = 0
        osc_count[0:2] = 0xE00 // 512 till rollover
        return false

    }

    if detector_top_osc_going_down {
        // if the top osc got back to it's normal value before anything else: reset
        if top_osc_near_or_above_max() {
            detector_initialized = 0
            return false
        }

        // saturated count of the amount of ticks since top osc went down
        if osc_count[0:1] != 0xFF {
            osc_count[0:1] += 1
        }

        // track if top osc is going up or down (mainly used to keep track of top_osc_went_up_flag)
        track_top_osc_direction()

        // wait for the bottom osc to start going down as well
        if (bottom_osc_readout > bottom_osc_max) {
            return false
        }
        if (bottom_osc_readout > bottom_osc_max - 0xC) {
            return false
        }

        // if the bottom osc went down in less than 6 cycles from top_osc going down reset,
        // otherwise, we've started detecting a coin
        if osc_count[0:1] < 6 {
            detector_initialized = 0
            return false
        }
        return true

    } else {
        // if the top osc value is still rising,
        // we're not ready yet
        if top_osc_readout > top_osc_max {
            top_osc_max = top_osc_readout
            bottom_osc_max = bottom_osc_readout
            return false
        }

        // if top_osc_readout drops 12 below the max
        // we might be starting to detect a coin, 
        if top_osc_readout + 0xC <= top_osc_max {
            detector_top_osc_going_down = 1
            detector_detecting_top_osc = 1
            previous_top_osc_readout = top_osc_readout
            top_osc_went_up_flag = 0
            osc_going_up_flags = 0
            osc_count[0:1] = 0x00
            return false
        }

        // this counts the time that the top_osc_readout has been between 0, -12 from the max
        // if it lasts more than 512 ms (as it was initialized at 0xE00) flag a reset
        osc_count[0:2] += 1
        if osc_count[0:2] == 0 {
            detector_initialized = 0
        }
        return false

    }
}

// compares the value in 0a to the value in 2a.
// if it's more than 4 larger than the one in 2a it sets 46.0 and assigns 0xF to 1F
// if it's more than 4 smaller than the one in 2a it resets 46.0
// then it sets 2a to 0a
// so essentially it detects if the value in 0a is descending across multiple calls
// or ascending over multiple calls 
fn track_top_osc_direction() {
    if (top_osc_readout > previous_top_osc_readout) {
        if (!top_osc_going_up) {
            if top_osc_readout <= 4 + previous_top_osc_readout {
                return
            }

            top_osc_going_up = 1
            top_osc_went_up_flag = 0xF
        }
        previous_top_osc_readout = top_osc_readout
    } else {
        if (top_osc_going_up) {
            if top_osc_readout > previous_top_osc_readout - 4 {
                return
            }

            top_osc_going_up = 0
        } else {
            previous_top_osc_readout = top_osc_readout
        }
    }
}

fn both_oscs_near_or_above_max() -> skip {
    return bottom_osc_near_or_above_max() && top_osc_near_or_above_max()
}

// returns false when bottom_osc_readout drops below the threshold
fn bottom_osc_near_or_above_max() -> skip {
    if (bottom_osc_readout > bottom_osc_max) {
        return true;
    } else if bottom_osc_readout > bottom_osc_max - 6 {
        return true
    }
    return false
}

// returns false when top_osc_readout drops below the threshold
fn top_osc_near_or_above_max() -> skip {
    if (top_osc_readout > top_osc_max) {
        return true;
    } else if top_osc_readout > top_osc_max - 6 {
        return true
    }
    return false
}

// hmm? basically just decrements 3cw and sets 4a.2 to 0 if it reaches 0
// used to occasionally wait until x interrupts have happened it seems
fn decrement_counter() {
    if (counter_ticks_remaining) {
        counter_ticks_remaining -= 1
    } else {
        counter_active = 0
    }
}

// reads the encoder setting into cost_remaining and temp
fn read_encoder_setting() {
    if disable_encoder_sync {
        return;
    }
    X = encoder_2_readout
    A = encoder_1_readout
    cost_remaining = XA
    total_cost = XA
}
