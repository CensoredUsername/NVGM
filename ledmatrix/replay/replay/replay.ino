#include <stdint.h>

typedef struct {
    uint32_t time;
    const char* msg;
} TimedMessage;

#define INITIAL_LENGTH 2
TimedMessage initial_sequence[INITIAL_LENGTH] = {
    {0, "\x18""\x01""C3\x01""H\x01""E5"},
    {1309, "\x01""D1@8S\x01""D1H8T\x01""D1P8A\x01""D1X8R\x01""D1`8T"}
};

#define REPEATING_LENGTH 110
TimedMessage repeating[REPEATING_LENGTH] = {
    {0, "\x1b""\x01""C3\x01""H\x01""E5FO0\x01""B05T0"},
    {50, "FI7"},
    {1674, "FO9"},
    {2524, "\x01""C3FI0"},
    {2574, "\x01""B0650"},
    {2824, "\x01""B0660"},
    {3074, "\x01""B0670"},
    {3324, "\x01""B0680"},
    {3574, "\x01""B0650"},
    {3824, "\x01""B0660"},
    {4074, "\x01""B0670"},
    {4324, "\x01""B0680"},
    {4574, "\x01""B0650"},
    {4824, "\x01""B0660"},
    {5074, "\x01""B0670"},
    {5324, "\x01""B0680"},
    {5574, "\x01""B0650"},
    {5824, "\x01""B0660"},
    {6074, "\x01""B0670"},
    {6324, "\x01""B0680"},
    {6574, "\x01""B0690"},
    {6824, "\x01""B06:0"},
    {7074, "\x01""B06;0"},
    {7324, "\x01""B06<0"},
    {7574, "\x01""B06=0"},
    {7824, "\x01""B06>0"},
    {8074, "\x01""B06?0"},
    {8323, "\x01""B06@0"},
    {8574, "\x01""B06A0"},
    {8824, "\x01""B06B0"},
    {9074, "\x01""B06C0"},
    {9324, "\x01""B06D0"},
    {9573, "\x01""B06E0"},
    {9824, "\x01""B06F0"},
    {10074, "\x01""B06E0"},
    {10323, "\x01""B06F0"},
    {10574, "\x01""B06E0"},
    {10824, "\x01""B06F0"},
    {11073, "\x01""B06E0"},
    {11323, "\x01""B06F0"},
    {11573, "\x01""B06G0"},
    {12073, "\x01""B06H0"},
    {12573, "\x01""B06I0"},
    {12824, "\x01""B07P0"},
    {13073, "\x01""B07O0"},
    {13323, "\x01""B07P0"},
    {13573, "\x01""B07O0"},
    {13823, "\x01""B07P0"},
    {14073, "\x01""U10112XL0\x01""U10012`L0\x01""U10012hL0\x01""U112320K0\x01""U11512PK0\x01""U11612pK0\x01""U10112HL0"},
    {14323, "\x01""P0000\x01""P0100"},
    {14573, "\x01""P0004\x01""P0104"},
    {14823, "\x01""P0008\x01""P0108"},
    {15073, "\x01""P000<\x01""P010<"},
    {15323, "\x01""P000@\x01""P010@"},
    {15573, "\x01""P000D\x01""P010D"},
    {15823, "\x01""P000H\x01""P010H"},
    {16074, "\x01""U11112X50\x01""U11012`50\x01""U10012h50\x01""U11232040\x01""U11512P40\x01""U11612p40\x01""U10212H50"},
    {16823, "\x01""P010H"},
    {17073, "\x01""P010L"},
    {17323, "\x01""P010P"},
    {17573, "\x01""P010T"},
    {17823, "\x01""P010X"},
    {18073, "\x01""P010\\"},
    {18323, "\x01""P010`"},
    {18573, "\x01""U11112XL0\x01""U10812`L0\x01""U10012hL0\x01""U112320K0\x01""U11512PK0\x01""U11612pK0\x01""U10312HL0"},
    {19323, "\x01""P0100"},
    {19573, "\x01""P0104"},
    {19823, "\x01""P0108"},
    {20073, "\x01""P010<"},
    {20323, "\x01""P010@"},
    {20573, "\x01""P010D"},
    {20823, "\x01""P010H"},
    {21073, "\x01""U11112X50\x01""U10712`50\x01""U10012h50\x01""U11232040\x01""U11512P40\x01""U11612p40\x01""U10412H50"},
    {21823, "\x01""P010H"},
    {22073, "\x01""P010L"},
    {22323, "\x01""P010P"},
    {22573, "\x01""P010T"},
    {22823, "\x01""P010X"},
    {23073, "\x01""P010\\"},
    {23323, "\x01""P010`"},
    {23573, "\x01""U11112XL0\x01""U10612`L0\x01""U10012hL0\x01""U11232040\x01""U11512P40\x01""U11612p40\x01""U10512HL0"},
    {24323, "\x01""P0100"},
    {24573, "\x01""P0104"},
    {24823, "\x01""P0108"},
    {25073, "\x01""P010<"},
    {25323, "\x01""P010@"},
    {25573, "\x01""P010D"},
    {25823, "\x01""P010H"},
    {26073, "\x01""P010H"},
    {26823, "\x01""C3\x01""U13541X00\x01""U11112`80\x01""U10812h80\x01""U10212p80\x01""U11112D80\x01""U10212L80\x01""E5\x01""U0>063000"},
    {27135, "\x01""U22072@80\x01""U0?063000"},
    {27385, "\x01""C2\x01""U0>063000"},
    {27635, "\x01""U22072@80\x01""U0?063000"},
    {27885, "\x01""C2\x01""U0>063000"},
    {28135, "\x01""U22072@80\x01""U0?063000"},
    {28385, "\x01""C2\x01""E2\x01""U0>063000"},
    {29435, "\x01""U11112`80\x01""U10812h80\x01""U10112p80\x01""U11112D80\x01""U10112L80"},
    {29565, "\x01""U11112`80\x01""U10812h80\x01""U10012p80\x01""U11112D80\x01""U10012L80\x01""E5\x01""U0>063000"},
    {29865, "\x01""U22072@80\x01""U0?063000"},
    {30115, "\x01""C2\x01""U0>063000"},
    {30365, "\x01""U22072@80\x01""U0?063000"},
    {30615, "\x01""C2\x01""U0>063000"},
    {30865, "\x01""U22072@80\x01""U0?063000"},
    {31115, "\x01""C2\x01""E2\x01""U0>063000"},
    {32165, "\x01""C3\x01""H\x01""B0600"},
    {33165, "\x01""B0610"},
    {33415, "\x01""B0620"},
    {33665, "\x01""B0630"},
    {33915, "\x01""B0640"},
    {34165, "\x01""B06T0"}
};

#define INITIAL_REPEATING_DELAY 7539
#define AFTER_REPEATING_DELAY 34416


// used pins
#define TX_PLUS PA, 0
#define TX_MINUS PF, 5

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

// timing stuff follows

#if F_CPU != 20000000
#error "F_CPU is not set to 20MHz, this code will not generate the correct signal"
#endif

void configure_timer() {
    cli();
    // we need a recurring interrupt at 9600 ticks per second,
    // which is a division by 2083.333

    TCB0.CTRLA = 0x0; // disabled
    TCB0.CTRLB = 0x0; // periodic interrupt mode
    TCB0.EVCTRL = 0; // no events used
    TCB0.INTCTRL = 0; // interrupt disabled
    TCB0.CCMP = 2083; // in periodic interrupt mode this sets the TOP value at which the counter is reset to 0. This sets us to ~9596 bps, which is close enough.
    TCB0.CNT = 0; // start at 0
    TCB0.INTFLAGS = 1; // reset interrupt flag
    TCB0.CTRLA = 1 | (0 << 1); // enable, use CLK_PER as source

    sei();
}

// wait for the flag to go high, then reset it.
void wait_for_tick() {
    while (!TCB0.INTFLAGS);
    TCB0.INTFLAGS = 1;
}

// reset the tick flag. Needed if significant (>1ms) time might have passed since the last wait_for_tick
void reset_wait() {
    TCB0.INTFLAGS = 1;
}

// application logic

void write_byte(uint8_t value) {
    // start bit
    wait_for_tick();
    set_pin(TX_PLUS, false);
    set_pin(TX_MINUS, true);
    // values, least to most significant
    for (uint8_t i = 0; i < 8; i++) {
        bool bit = value & 1;
        value >>= 1;
        wait_for_tick();
        set_pin(TX_PLUS, bit);
        set_pin(TX_MINUS, !bit);
    }
    // stop bit
    wait_for_tick();
    set_pin(TX_PLUS, true);
    set_pin(TX_MINUS, false);
}

void write_string(const char* value) {
    reset_wait();
    while (*value) {
        write_byte((uint8_t)*value);
        value++;
    }
}



void setup() {
    Serial.begin(38400);

    configure_timer();

    // initialize pins
    set_mode(TX_PLUS, true);
    set_mode(TX_MINUS, true);
    set_pin(TX_PLUS, true);
    set_pin(TX_MINUS, false);


    // wait for a second
    while (millis() < 1000);

    uint32_t start_time = millis();
    for (size_t i = 0; i < INITIAL_LENGTH; i++) {
        TimedMessage& msg = initial_sequence[i];
        while (millis() < (start_time + msg.time));
        write_string(msg.msg);
    }

    while (millis() < (start_time + INITIAL_REPEATING_DELAY));
}

void loop() {
    uint32_t start_time = millis();
    for (size_t i = 0; i < REPEATING_LENGTH; i++) {
        TimedMessage& msg = repeating[i];
        while (millis() < (start_time + msg.time));
        write_string(msg.msg);
    }

    while (millis() < (start_time + AFTER_REPEATING_DELAY));
}
