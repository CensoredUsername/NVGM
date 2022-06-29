#include <avr/io.h>

#define SERIAL_PIN PC, 5

#define DEBUG_MODE_PIN PA, 0

#define LEARNING_MODE_PIN PC, 6

#define LED_PIN PE, 2

void setup() {
    // put your setup code here, to run once:

    // divide our clock by an extra two cause the baud rate doesn't go down enough otherwise
    CCP = 0xD8;
    CLKCTRL.MCLKCTRLB = 1;


    // set_mode(SERIAL_PIN, false);
    set_mode(PF, 5, false);
    set_mode(DEBUG_MODE_PIN, false);
    set_mode(LEARNING_MODE_PIN, false);

    set_pin(DEBUG_MODE_PIN, false);
    set_pin(LEARNING_MODE_PIN, false);

    set_mode(LED_PIN, true);
    set_pin(LED_PIN, false);

    Serial.begin(38400 * 2, SERIAL_8N1);
    Serial1.begin(1150 * 2, SERIAL_7N1);
}

void loop() {
    // put your main code here, to run repeatedly:

    // wait for us to get a command
    while (!Serial.find('\n'));
    Serial.print("[");

    // get rid of any remaining data
    while (Serial1.read() != -1);

    // poke the thing into debug mode
    set_pin(LED_PIN, true);
    set_mode(LEARNING_MODE_PIN, true);
    set_mode(DEBUG_MODE_PIN, true);

    // wait for data to start coming
    while (!Serial1.available());

    // log until 100ms without data
    uint32_t time = millis();
    while ((time + 50) > millis()) {
        int c = Serial1.read();
        if (c != -1) {
            time = millis();
            Serial.write(c);
        }
    }

    // remove debug triggers
    set_mode(DEBUG_MODE_PIN, false);
    set_mode(LEARNING_MODE_PIN, false);
    set_pin(LED_PIN, false);

    Serial.println("]");
    Serial.flush();
}


// pin stuff follows

static PORT_t& get_port(uint8_t port_id) {
    return (&PORTA)[port_id];
}

static void set_mode(uint8_t port, uint8_t pin, bool output) {
    if (output) {
        get_port(port).DIRSET = 1 << pin;
    } else {
        get_port(port).DIRCLR = 1 << pin;
    }
}

static void set_pin(uint8_t port, uint8_t pin, bool high) {
    if (high) {
        get_port(port).OUTSET = 1 << pin;
    } else {
        get_port(port).OUTCLR = 1 << pin;
    }
}

static bool read_pin(uint8_t port, uint8_t pin) {
    return (get_port(port).IN & (1 << pin)) != 0;
}

static void toggle_pin(uint8_t port, uint8_t pin) {
    get_port(port).OUTTGL = 1 << pin;
}
