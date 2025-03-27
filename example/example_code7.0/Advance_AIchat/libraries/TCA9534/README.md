# TCA9534

Arduino library for [TCA9534](https://www.ti.com/product/TCA9534) Low Voltage 8-Bit I2C and SMBUS Low-Power I/O Expander with Interrupt Output and Configuration Registers

## Usage

``` C++
#include <TCA9534.h>

TCA9534 ioex;
const uint8_t IOEX_ADDR = 0x30; // A0 = A1 = A2 = 0

void setup()
{
    Wire.begin();
    ioex.attach(Wire);
    ioex.setDeviceAddress(0x30); // A0 = A1 = A2 = 0
    ioex.config(TCA9534::Config::IN); // set all port to output
    ioex.config(1, TCA9534::Config::OUT); // schange port P1 to output
    ioex.polarity(TCA9534::Polarity::ORIGINAL); // set all port polarity to original
    ioex.polarity(1, TCA9534::Polarity::INVERSE); // change port P1 polarity

    Serial.print("config  : ");
    Serial.println(ioex.config(), HEX);
    Serial.print("polarity: ");
    Serial.println(ioex.polarity(), HEX);
}

void loop()
{
    static int count = 0;
    uint8_t port = count++ % 8;

    if (port % 2 == 0)
        ioex.output(1, TCA9534::Level::L);
    else
        ioex.output(1, TCA9534::Level::H);

    Serial.println(ioex.input(), BIN);

    delay(1000);
}
```


## APIs

```C++
enum class Config { OUT, IN };
enum class Polarity { ORIGINAL, INVERSE };
enum class Level { L, H };

void attach(WireType& w);
void setDeviceAddress(const uint8_t addr);

uint8_t input(const uint8_t port);
uint8_t input();
uint8_t output(const uint8_t port, const uint8_t v);
uint8_t output(const uint8_t v);
uint8_t output();
uint8_t polarity(const uint8_t port, const Polarity pol);
uint8_t polarity(const Polarity pol);
uint8_t polarity();
uint8_t config(const uint8_t port, const Config cfg);
uint8_t config(const Config cfg);
uint8_t config();
uint8_t status() const; // i2c status
```

## License

MIT
