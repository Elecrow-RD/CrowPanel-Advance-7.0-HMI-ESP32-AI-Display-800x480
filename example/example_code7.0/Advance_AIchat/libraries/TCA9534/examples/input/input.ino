#include <TCA9534.h>

TCA9534 ioex;
const uint8_t IOEX_ADDR = 0x30; // A0 = A1 = A2 = 0

void setup()
{
    Serial.begin(115200);
    delay(2000);

    Wire.begin();
    ioex.attach(Wire);
    ioex.setDeviceAddress(IOEX_ADDR);
    ioex.config(TCA9534::Config::IN); // set all port to input
    ioex.polarity(TCA9534::Polarity::INVERSE); // set all port polarity to inverse

    Serial.print("config  : ");
    Serial.println(ioex.config(), HEX);
    Serial.print("polarity: ");
    Serial.println(ioex.polarity(), HEX);
}

void loop()
{
    uint8_t raw = ioex.input();
    Serial.println(raw, BIN);

    delay(1000);
}
