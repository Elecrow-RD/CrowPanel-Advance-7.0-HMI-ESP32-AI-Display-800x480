#include <TCA9534.h>

TCA9534 ioex;
const uint8_t IOEX_ADDR = 0x30; // A0 = A1 = A2 = 0
uint8_t count = 0;
bool dir = false;

void setup()
{
    Serial.begin(115200);
    delay(2000);

    Wire.begin();
    ioex.attach(Wire);
    ioex.setDeviceAddress(IOEX_ADDR);
    ioex.config(TCA9534::Config::OUT); // set all port to output
    ioex.polarity(TCA9534::Polarity::ORIGINAL); // set all port polarity to original

    Serial.print("config  : ");
    Serial.println(ioex.config(), HEX);
    Serial.print("polarity: ");
    Serial.println(ioex.polarity(), HEX);
}

void loop()
{
    uint8_t port = count++ % 8;
    if (port == 0) dir = !dir;

    if (dir)
        ioex.output(port, TCA9534::Level::H);
    else
        ioex.output(port, TCA9534::Level::L);

    Serial.println(ioex.input(), BIN);

    delay(1000);
}
