uint8_t input_pins[8] = {0,1,2,3,4,5,6,7};

#define MD0_PIN 8
// MD1 PIN is always shorted to ground to prevent ever entering PROM WRITE mode.
#define MD2_PIN 9
#define MD3_PIN 10
#define X1_PIN 11
#define X2_PIN 12

#define LED_PIN 13


void setup() {
    for (uint8_t i = 0; i < 8; i++) {
        pinMode(input_pins[i], INPUT);
    }
    pinMode(MD0_PIN, OUTPUT);
    pinMode(MD2_PIN, OUTPUT);
    pinMode(MD3_PIN, OUTPUT);
    pinMode(X1_PIN, OUTPUT);
    pinMode(X2_PIN, OUTPUT);

    digitalWrite(MD0_PIN, LOW);
    digitalWrite(MD2_PIN, LOW);
    digitalWrite(MD3_PIN, LOW);
    digitalWrite(X1_PIN, LOW);
    digitalWrite(X2_PIN, LOW);

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
    digitalWrite(X1_PIN, HIGH);
    digitalWrite(X2_PIN, LOW);
    delayMicroseconds(20);
    digitalWrite(X1_PIN, LOW);
    digitalWrite(X2_PIN, HIGH);
    delayMicroseconds(20);
}

void loop() {
    Serial.println("Please enable 5V to Vpp and Vdd,");
    await_serial_confirmation();
    digitalWrite(X2_PIN, HIGH);
    digitalWrite(MD0_PIN, HIGH);
    digitalWrite(MD2_PIN, HIGH);
    Serial.println("Set zero clear program memory address mode,");
    Serial.println("Please set Vdd to 6V and Vpp to 12.5V.");
    await_serial_confirmation();
    digitalWrite(MD3_PIN, HIGH);
    digitalWrite(MD0_PIN, LOW);
    Serial.println("Set verify mode.");

    for (uint16_t i = 0; i < 16384; i++) {
        pulse_x1();
        pulse_x1();
        uint8_t byte = 0;
        for (uint8_t j = 0; j < 8; j++) {
            if (digitalRead(input_pins[j]) != LOW) {
                byte |= (1 << j);
            }
        }
        Serial.print("Address ");
        Serial.print(i, HEX);
        Serial.print(" = ");
        Serial.println(byte, HEX);

        pulse_x1();
        pulse_x1();
    }

    Serial.println("Readout complete. Setting back zero clear program memory address mode.");
    digitalWrite(MD0_PIN, HIGH);
    digitalWrite(MD3_PIN, LOW);
    Serial.println("Please reset Vpp and Vdd to 5V");
    await_serial_confirmation();
    Serial.println("It is safe to turn the device off now");
    for (;;);
}
