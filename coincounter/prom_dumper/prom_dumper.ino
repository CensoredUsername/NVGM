#include <avr/io.h>

uint8_t input_ports[8] = {PB, PF, PA, PE, PB, PB, PE, PE};
uint8_t input_pins[8] =  {2,  4,  1,  3,  0,  1,  0,  1};

static PORT_t& get_port(uint8_t port_id) {
    return (&PORTA)[port_id];
}

#define MD_PORT PD
uint8_t md_pins[4] = {0,1,2,3};
// MD1 PIN is always shorted to ground to prevent ever entering PROM WRITE mode.

#define X_PORT PD
#define X1_PIN 5
#define X2_PIN 4

#define LED_PORT PE
#define LED_PIN 2

#define RESET_PORT PC
#define RESET_PIN 4

void set_mode(uint8_t port, uint8_t pin, bool output) {
    if (output) {
        get_port(port).DIRSET = 1 << pin;
    } else {
        get_port(port).DIRCLR = 1 << pin;
    }
}

void set_pin(uint8_t port, uint8_t pin, bool high) {
    if (high) {
        get_port(port).OUTSET = 1 << pin;
    } else {
        get_port(port).OUTCLR = 1 << pin;
    }
}

bool read_pin(uint8_t port, uint8_t pin) {
    return (get_port(port).IN & (1 << pin)) != 0;
}

void toggle_pin(uint8_t port, uint8_t pin) {
    get_port(port).OUTTGL = 1 << pin;
}


void setup() {
    for (uint8_t i = 0; i < 8; i++) {
        set_mode(input_ports[i], input_pins[i], false);  
    }
    for (uint8_t i = 0; i < 4; i++) {
        set_mode(MD_PORT, md_pins[i], true);
        set_pin(MD_PORT, md_pins[i], false);
    }
    set_mode(X_PORT, X1_PIN, true);
    set_mode(X_PORT, X2_PIN, true);
    set_pin(X_PORT, X1_PIN, false);
    set_pin(X_PORT, X2_PIN, false);

    set_mode(LED_PORT, LED_PIN, true);
    set_pin(LED_PORT, LED_PIN, false);

    set_mode(RESET_PORT, RESET_PIN, true);
    set_pin(RESET_PORT, RESET_PIN, false);

    Serial.begin(38400, SERIAL_8N1);
}

void await_serial_confirmation() {
    Serial.println("then press enter to continue...");
    for (;;) {
        while (!Serial.available());
        if (Serial.read() == '\n') {
            break;
        }
    }
}

void pulse_x1() {
    get_port(X_PORT).OUTTGL = (1 << X1_PIN) | (1 << X2_PIN);
    delayMicroseconds(20);
    get_port(X_PORT).OUTTGL = (1 << X1_PIN) | (1 << X2_PIN);
    delayMicroseconds(20);
}

void loop() {
    Serial.println("Please enable 5V to Vpp and Vdd,");
    await_serial_confirmation();
    set_pin(RESET_PORT, RESET_PIN, true);
    delayMicroseconds(100);
    set_pin(X_PORT, X2_PIN, true);
    set_pin(MD_PORT, md_pins[0], true);
    set_pin(MD_PORT, md_pins[2], true);
    Serial.println("Set zero clear program memory address mode,");
    Serial.println("Please set Vdd to 6V and Vpp to 12.5V.");
    await_serial_confirmation();
    set_pin(MD_PORT, md_pins[3], true);
    set_pin(MD_PORT, md_pins[0], false);
    Serial.println("Set verify mode.");

    for (uint16_t i = 0; i < 16384; i++) {
        pulse_x1();
        pulse_x1();
        set_pin(LED_PORT, LED_PIN, true);
        uint8_t byte = 0;
        for (uint8_t j = 0; j < 8; j++) {
            if (read_pin(input_ports[j], input_pins[j])) {
                byte |= (1 << j);
            }
        }
        Serial.print(i, HEX);
        Serial.print(": ");
        Serial.println(byte, HEX);
        
        set_pin(LED_PORT, LED_PIN, false);

        pulse_x1();
        pulse_x1();
    }

    Serial.println("Readout complete. Setting back zero clear program memory address mode.");
    set_pin(MD_PORT, md_pins[0], true);
    set_pin(MD_PORT, md_pins[3], false);

    Serial.println("asserting reset");
    set_pin(RESET_PORT, RESET_PIN, false);

    Serial.println("Please reset Vpp and Vdd to 5V");
    await_serial_confirmation();
    Serial.println("It is safe to turn the device off now");
    for (;;);
}
