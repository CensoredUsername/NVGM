#define OUTPUT_PORT     PD
#define OUTPUT_PIN_CS   0
#define OUTPUT_PIN_CLK  1
#define OUTPUT_PIN_MOSI 2

#define INPUT_PORT      PD
#define INPUT_PIN_MISO  3

#define COMMAND_ERASE 0xC0
#define COMMAND_ERAL  0x20
#define COMMAND_EWDS  0x00
#define COMMAND_EWEN  0x30
#define COMMAND_READ  0x80
#define COMMAND_WRITE 0x40
#define COMMAND_WRAL  0x10
#define ADDRESS_NONE  0x00

void setup() {
    // initial pin state configuration
    set_pin(OUTPUT_PORT, OUTPUT_PIN_CS, false);
    set_pin(OUTPUT_PORT, OUTPUT_PIN_CLK, false);
    set_pin(OUTPUT_PORT, OUTPUT_PIN_MOSI, false);

    set_mode(INPUT_PORT, INPUT_PIN_MISO, false);
    set_mode(OUTPUT_PORT, OUTPUT_PIN_CS, true);
    set_mode(OUTPUT_PORT, OUTPUT_PIN_CLK, true);
    set_mode(OUTPUT_PORT, OUTPUT_PIN_MOSI, true);

    // serial port
    Serial.begin(38400, SERIAL_8N1);
}

void loop() {
    eeprom_ewds();

    // put your main code here, to run repeatedly:
    // wait for the starting character
    while (!Serial.find(0xA5));

    // buffer to receive into
    uint8_t command_buf[4];
    if (Serial.readBytes(command_buf, 1) < 1) {
        return;
    }

    if (command_buf[0] != 0xC3) {
        return;
    }

    // header confirmed, read the full command
    if (Serial.readBytes(command_buf, 4) < 4) {
        return;
    }

    uint8_t command = command_buf[0];
    uint8_t address = command_buf[1];
    uint16_t data = uint16_t(command_buf[3]) << 8 | uint16_t(command_buf[2]);

    if (address > 0x3F) {
        return;
    }

    switch (command) {
        case 'R':
            data = eeprom_read(address);
            command_buf[2] = data;
            command_buf[3] = data >> 8;
            break;

        case 'W':
            eeprom_ewen();
            eeprom_write(address, data);
            break;

        case 'E':
            eeprom_ewen();
            eeprom_erase(address);
            break;

        default:
            return;
    }

    Serial.write(0x5A);
    Serial.write(0x3C);
    Serial.write(command_buf, 4);
    Serial.write('\n');
    Serial.flush();
}

// high level protocol stuff follows

void eeprom_write(uint8_t address, uint16_t data) {
    eeprom_start_command(COMMAND_WRITE, address);
    eeprom_send_data(data);
    eeprom_stop_command();
    eeprom_await_completion();
}

uint16_t eeprom_read(uint8_t address) {
    eeprom_start_command(COMMAND_READ, address);
    uint16_t data = eeprom_receive_data();
    eeprom_stop_command();
    return data;
}

void eeprom_erase(uint8_t address) {
    eeprom_start_command(COMMAND_ERASE, address);
    eeprom_stop_command();
    eeprom_await_completion();
}

void eeprom_ewds() {
    eeprom_start_command(COMMAND_EWDS, ADDRESS_NONE);
    eeprom_stop_command();
}

void eeprom_ewen() {
    eeprom_start_command(COMMAND_EWEN, ADDRESS_NONE);
    eeprom_stop_command();
}

// low level protocol stuff follows

static void wait() {
    // delays 1us
    asm volatile (
        "nop;nop;nop;nop;nop;"
        "nop;nop;nop;nop;nop;"
        "nop;nop;nop;nop;nop;"
        "nop;nop;nop;nop;nop;"
    );
}

static void eeprom_await_completion() {
    // raise CS, then check every microsecond if MISO has gone high.
    // if it has, we're done.
    set_pin(OUTPUT_PORT, OUTPUT_PIN_CS, true);
    do {
        wait();
    } while (!read_pin(INPUT_PORT, INPUT_PIN_MISO));
    set_pin(OUTPUT_PORT, OUTPUT_PIN_CS, false);
    wait();
}

static void eeprom_start_command(uint8_t command, uint8_t address) {
    uint8_t bits = command | (address & 0x3F);

    // start condition
    set_pin(OUTPUT_PORT, OUTPUT_PIN_CS, true);
    set_pin(OUTPUT_PORT, OUTPUT_PIN_MOSI, true);
    wait();
    set_pin(OUTPUT_PORT, OUTPUT_PIN_CLK, true);
    wait();
    set_pin(OUTPUT_PORT, OUTPUT_PIN_CLK, false);

    // 8 bits of command + address
    for (uint8_t i = 0; i < 8; i++) {
        set_pin(OUTPUT_PORT, OUTPUT_PIN_MOSI, (bits & 0x80) != 0);
        wait();
        set_pin(OUTPUT_PORT, OUTPUT_PIN_CLK, true);
        wait();
        set_pin(OUTPUT_PORT, OUTPUT_PIN_CLK, false);
        bits <<= 1;
    }
}

static void eeprom_send_data(uint16_t data) {
    for (uint8_t i = 0; i < 16; i++) {
        set_pin(OUTPUT_PORT, OUTPUT_PIN_MOSI, (data & 0x8000) != 0);
        wait();
        set_pin(OUTPUT_PORT, OUTPUT_PIN_CLK, true);
        wait();
        set_pin(OUTPUT_PORT, OUTPUT_PIN_CLK, false);
        data <<= 1;
    }
}

static uint16_t eeprom_receive_data() {
    uint16_t data = 0;
    for (uint8_t i = 0; i < 16; i++) {
        data <<= 1;
        wait();
        set_pin(OUTPUT_PORT, OUTPUT_PIN_CLK, true);
        wait();
        data |= read_pin(INPUT_PORT, INPUT_PIN_MISO);
        set_pin(OUTPUT_PORT, OUTPUT_PIN_CLK, false);
    }
    return data;
}

static void eeprom_stop_command() {
    // stop
    wait();
    set_pin(OUTPUT_PORT, OUTPUT_PIN_MOSI, false);
    set_pin(OUTPUT_PORT, OUTPUT_PIN_CS, false);
    wait();
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
