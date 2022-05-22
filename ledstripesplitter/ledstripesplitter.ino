#include <avr/io.h>

// port index of the input pin being used
#define INPUT_PORT PE

// pin index of the input pin being used
#define INPUT_PIN 2

// amount of cycles that the input has to be low to register a reset condition. Supports values up to 1500.
// should be at most 1000 (50us), and at least 8us
#define RESET_CYCLES 600

// storage for segment bytecode. Don't change this.
#define MAX_SEGMENT_COUNT 127
uint8_t segment_bytecode[2 * MAX_SEGMENT_COUNT] = {0};

void setup() {
    configure_input();
    configure_segment(0, PE, 1);
    configure_segment(1, PE, 0);
    configure_segment(2, PB, 1);
    configure_segment(3, PB, 0);
    configure_segment(4, PE, 3);
    configure_segment(5, PA, 1);
    configure_segment(6, PF, 4);
    configure_segment(7, PB, 2);
    configure_segment(8, PC, 6);
    configure_segment(9, PF, 5);
    configure_segment(10, PA, 0);
    configure_segment(11, PC, 4);
    configure_segment(12, PC, 5);
}

void loop() {
    cli();
    execute_bytecode(4, 4);
}

void configure_input() {
    // configure input pin as input, enable pullup so it doesn't float.
    PORT_t& port_ref = (&PORTA)[INPUT_PORT];
    port_ref.DIRCLR = 1 << INPUT_PIN;
    (&port_ref.PIN0CTRL)[INPUT_PIN] = 0x08;
}

void configure_segment(uint8_t index, uint8_t port, uint8_t pin) {
    // set bytecode entry.
    segment_bytecode[index * 2] = 0x07 + 0x20 * port;
    segment_bytecode[index * 2+ 1] = 1 << pin;

    // pin init. set direction and initialize low.
    PORT_t& port_ref = (&PORTA)[port];
    port_ref.DIRSET = 1 << pin;
    port_ref.OUTCLR = 1 << pin;
}

void execute_bytecode(uint8_t segment_count, uint8_t segment_length) {
    asm volatile (
        // register allocation
        // Z (r30:r31) contains a pointer to the current entry in the bytecode
        // X (r26:r27) contains a pointer to the toggle register for the current output port
        // r25:r24 contains the bits remaining in the current segment
        // r18 contains either the reset counter or the segment counter
        // r0 contains the bitmask for the current output port
        "entry_point: \n"
        "movw r30, %A[bytecode_addr] \n" // load segment bytecode address into Z register
        "ldi r27, %[port_reg_hibyte] \n" // X high byte, high byte of all port register addresses

        // reset await loop prologue
        "restart_reset_loop: \n"
        "ldi r18, %[reset_loop_cycles] \n"
        "reset_loop: \n" // this takes 6 cycles to loop, which at 20MHz means ~0.3us per loop
            "sbic %[input_port], %[input_pin] \n" // 1 cycle + 2 if next is skipped
            "rjmp restart_reset_loop \n" // 2 cycles
            "dec r18 \n" // 1 cycle
            "brne reset_loop \n" // 2 cycles if taken, 1 if not

        // segment loop prologue
        "mov r18, %[segment_count] \n" // amount of segments to go through

        "segment_loop: \n"
            // setup for the current segment. need to load the right address in X, and the right bitmask in R20.
            // both are read from a small datastructure. said data structure should be in the Z pointer. also iterate through it here
            "ld r26, Z+ \n" // 2 cycles, load low byte of X from bytecode
            "ld r0, Z+ \n" // 2 cycles, load the correct bitmask from bytecode

            "movw r24, %A[bitcount_per_segment] \n" // ; 1 cycle, load amount of bits in bitcount to counter
            "bit_loop: \n"

                "rising_edge: \n"
                    "sbis %[input_port], %[input_pin] \n" // 1 cycle, + 2 if next skipped
                    "rjmp rising_edge \n" // 2 cycles

                // rising edge detected, toggle current output pin
                "st X, r0 \n" // 1 cycle
                "nop \n"

                "falling_edge: \n"
                    "sbic %[input_port], %[input_pin] \n" // 1 cycle, + 2 if next skipped
                    "rjmp falling_edge \n" // ; 2 cycles

                // falling edge detected, toggle current output pin
                "st X, r0 \n" // 1 cycle

                "sbiw r24, 1 \n" // 2 cycles
                "brne bit_loop \n" // 2 cycles if taken, 1 if not 

            "dec r18 \n" // 1 cycle
            "brne segment_loop \n" // 2 cycles if taken, 1 if not 

        "rjmp entry_point \n"
        // if this proves too slow (or a variable bitcount is needed)
        // unrolling segment_loop so there's just one special loop for each segment
        // would probably be the best idea
        : /* no outputs */
        : [segment_count]"r"(segment_count)
        , [bitcount_per_segment]"r"(24*(uint16_t)segment_length)
        , [reset_loop_cycles]"M"(RESET_CYCLES / 6)
        , [bytecode_addr]"r"((uint8_t *)segment_bytecode)
        , [input_port]"I"(INPUT_PORT * 4 + 2)
        , [input_pin]"I"(INPUT_PIN)
        , [port_reg_hibyte]"I"(0x04)
        // clobbers
        : "r0", "r18", "r24", "r25", "r26", "r27", "r30", "r31"
    );
}
