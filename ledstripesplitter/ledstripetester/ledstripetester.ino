#include <avr/io.h>

#define OUTPUT_PORT PE
#define OUTPUT_PIN 2

#define BYTECODE_LEN 3 * 16
uint8_t bytecode[BYTECODE_LEN] = {
    0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00,
    0xAA, 0xAA, 0xAA,
    0xFF, 0x00, 0xAB,
    0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00,
    0xAA, 0xAA, 0xAA,
    0xFF, 0x00, 0xAA,
    0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00,
    0xAA, 0xAA, 0xAA,
    0xFF, 0x00, 0xAB,
    0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00,
    0xAA, 0xAA, 0xAA,
    0xFF, 0x00, 0xAA
};

// generates a waveform compatible with Ws2812 control

void setup() {
    // initialize output pin
    PORT_t& port_ref = (&PORTA)[OUTPUT_PORT];
    port_ref.DIRSET = 1 << OUTPUT_PIN;
    port_ref.OUTCLR = 1 << OUTPUT_PIN;
}

void loop() {
    // put your main code here, to run repeatedly:
    cli();
    for(;;) {
        wait(50);
        generate_signal(bytecode, BYTECODE_LEN);
    }
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

void generate_signal(uint8_t *bytecode, uint8_t bytecode_len) {
    bytecode_len++;
    asm volatile (
        "rjmp entry_point \n"
        "wave_start_early: \n"
            "nop \n nop \n nop \n nop \n nop \n nop \n nop \n" // 7 cycles delay
        "wave_start: \n"
            "lsl r0 \n" // 1 cycle
            "sbi %[output_port], %[output_pin] \n" // 1 cycle
            "nop \n" // 1 cycle
        "brcs one \n" // 2 cycles if taken, 1 if not
            "nop \n nop \n nop \n nop \n" // 4 cycle delay
            "cbi %[output_port], %[output_pin] \n" // 1 cycle delay
            "nop \n nop \n" // 2 cycle delay
            "rjmp wave_end \n" // 2 cycle delay
        "one: \n"
            "nop \n nop \n nop \n nop \n nop \n" // 5 cycle delay
            "nop \n nop \n nop \n nop \n nop \n" // 5 cycle delay
            "cbi %[output_port], %[output_pin] \n" // 1 cycle delay
        "wave_end: \n"
        // from this point onwards, need 0.50 ns delay (10 cycles) till back to wave_start
            "dec r18 \n" // 1 cycles
            "brne wave_start_early \n" // 2 cycles if taken, 1 if not.
        "entry_point: \n"
            "dec %[bytecode_remaining] \n" // 1 cycle
            "breq done \n" // 2 cycles if taken, 1 if not.
            "ld r0, %a[bytecode]+ \n" // 2 cycles
            "ldi r18, 8 \n" // 1 cycle
            "nop \n" // 1 cycle
            "rjmp wave_start \n" // 2 cycles
        "done: \n"
        : [bytecode]"+e"(bytecode)
        , [bytecode_remaining]"+r"(bytecode_len)
        : [output_port]"I"(OUTPUT_PORT * 4 + 1)
        , [output_pin]"I"(OUTPUT_PIN)
        : "r0", "r18"
    );
}
