#include <avr/io.h>




// THE FOLLOWING VALUES CAN BE CHANGED TO SUIT THE WIRING


// port index of the input pin being used
#define INPUT_PORT PA

// pin index of the input pin being used
#define INPUT_PIN 0

// the amount of segments that the signal will be split over. Maxes out at 127
#define SEGMENT_COUNT 12

// the amount of LEDs in a single segment
#define SEGMENT_SIZE (2 * 4 * 5)

// the actual segment pin assignment is located in setup() below.


// END OF USER CONFIG




#if F_CPU != 20000000
#error "F_CPU is not set to 20MHz, this code will not generate the correct signal. check boards.txt"
#endif

// amount of cycles that the input has to be low to register a reset condition. Supports values up to 1500.
// should be at most 1000 (50us), and at least 8us
#define RESET_CYCLES 600

// storage for segment bytecode. Don't change this.
#define MAX_SEGMENT_COUNT 127
uint8_t segment_bytecode[2 * MAX_SEGMENT_COUNT] = {0};


void setup() {
    cli();
    configure_input();

    // THE FOLLOWING CODE CONFIGURES THE PINS THAT THE SIGNAL WILL BE SPLIT OVER.
    // The arguments represent segment index, port, and pin index within said port.

    configure_segment(0, PD, 5);
    configure_segment(1, PD, 4);
    configure_segment(2, PA, 3);
    configure_segment(3, PA, 2);
    configure_segment(4, PD, 0);   

    configure_segment(5, PF, 5);
    configure_segment(6, PC, 6);
    configure_segment(7, PB, 2);
    configure_segment(8, PF, 4);
    configure_segment(9, PA, 1);
    configure_segment(10, PE, 3);
    configure_segment(11, PB, 0);
}

void loop() {
    // wait 30us so we ensure the reset signal is propagated properly the first time around
    // the multiplexer only needs 30us of low signal to detect a reset to handle misbehaving controllers
    // but that means that during bootup it might get confused if the avr itself needed some time to boot (and it boots with pins high)
    wait(30);
    execute_bytecode(SEGMENT_COUNT, SEGMENT_SIZE);
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

void wait(uint8_t us) {
    asm volatile (
        "loop: \n"
            "nop \n nop \n nop \n nop \n nop\n"
            "nop \n nop \n nop \n nop \n nop \n"
            "nop \n nop \n nop \n nop \n nop \n"
            "nop \n nop \n"
            "dec %[count] \n"
            "brne loop \n"
            "nop \n"
        : [count]"+r"(us)
        :
        :
    );
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

        // set segment count
        "mov r18, %[segment_count] \n" // amount of segments to go through

        "next_segment: \n"
            "ld r26, Z+ \n" // 2 cycles, load low byte of X from bytecode
            "ld r0, Z+ \n" // 2 cycles, load the correct bitmask from bytecode

        "next_bit: \n" // start of the loop per bit
        
            // await a rising edge. This loop takes 3 cycles, or terminates 2, 3 or 4 cycles after a rising edge has happened.
            "sbis %[input_port], %[input_pin] \n" // 1 cycle
                "rjmp next_bit \n" // 2 cycles if taken, 1 if skipped (one word instr)

            // 8 cycles remaining
            
            // padding
            "nop \n nop \n nop \n" // 3 cycles

            // toggle the output high.
            "st X, r0 \n" // 1 cycle. now we have either 5 or 13 cycles till we should go down.

            // reset bitcount if it reached zero
            "sbiw r24, 1 \n" // 2 cycles, decrement bitcount. store equality to zero in flags
            "brne notzero \n" // 2 cycles if taken, 1 if not
                "movw r24, %A[bitcount_per_segment] \n" // 1 cycle, load amount of bits in bitcount to counter
            "notzero: \n"

            // read if we should be high or low (6 cycles after the end of the rising_edge loop)
            "sbic %[input_port], %[input_pin] \n" // 1 cycle
            "rjmp one \n" // 2 cycles if taken, 1 if not.
            
            // a zero was detected.
            "zero: \n"
                // go low 6 cycles after the original go high
                "st X, r0 \n" // 1 cycle.

                // if not at the end of the bitcount loop, start scanning again.
                "brne next_bit \n" // 2 cycles if taken, one if not

                // decrement segment counter and either go back to the next segment or the entry point
                "dec r18 \n" // 1 cycle
                "brne next_segment \n" // 2 cycles if taken, one if not.
                "rjmp entry_point \n" // 2 cycles

            // a one was detected. 
            "one: \n"
                // 6 cycles to do something with
                "nop \n nop \n nop \n nop \n" // wait 4 cycles
                "brne simple \n" // 2 cycles if taken, one if not
                "dec r18 \n" // one cycle
            
                // go low 13 cycles after the original go high
                "st X, r0 \n" // 1 cycle.
                "brne next_segment \n" // 2 cycles if taken, one if not.
                "rjmp entry_point \n" // 2 cycles

            "simple: \n"
                // go low and start looking for the next bit
                "st X, r0 \n" // 1 cycle.
                "rjmp next_bit \n" // 2 cycles

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
