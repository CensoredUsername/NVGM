#ifndef _SIMPLE_GPIO_
#define _SIMPLE_GPIO_

inline PORT_t& get_port(uint8_t port_id) {
    return (&PORTA)[port_id];
}

inline void set_mode(uint8_t port, uint8_t pin, bool output) {
    if (output) {
        get_port(port).DIRSET = 1 << pin;
    } else {
        get_port(port).DIRCLR = 1 << pin;
    }
}

inline void set_pin(uint8_t port, uint8_t pin, bool high) {
    if (high) {
        get_port(port).OUTSET = 1 << pin;
    } else {
        get_port(port).OUTCLR = 1 << pin;
    }
}

inline void set_pullup(uint8_t port, uint8_t pin, bool enabled) {
    if (enabled) {
        (&get_port(port).PIN0CTRL)[pin] |= 1 << 3;
    } else {
        (&get_port(port).PIN0CTRL)[pin] &= ~(1 << 3);
    }
}

inline bool read_pin(uint8_t port, uint8_t pin) {
    return (get_port(port).IN & (1 << pin)) != 0;
}

inline void toggle_pin(uint8_t port, uint8_t pin) {
    get_port(port).OUTTGL = 1 << pin;
}


#endif
