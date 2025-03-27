#pragma once
#ifndef ARDUINO_TCA9534_H
#define ARDUINO_TCA9534_H

#include <Arduino.h>
#include <Wire.h>

namespace arduino
{
    template <typename WireType, uint8_t N>
    class IoEx8bit
    {
        uint8_t I2C_ADDR = 0b0100000;

        WireType* wire;
        uint8_t sts;

    public:

        enum class Reg { INPUT_PORT, OUTPUT_PORT, POLARITY, CONFIG };
        enum class Config { OUT, IN };
        enum class Polarity { ORIGINAL };
        enum Level { L = 0x00, H = 0xFF };

        void attach(WireType& w) { wire = &w; }

        void setDeviceAddress(const uint8_t addr) { I2C_ADDR = addr; }

        uint8_t input(const uint8_t port)
        {
            return readBit(I2C_ADDR, (uint8_t)Reg::INPUT_PORT, port);
        }
        uint8_t input()
        {
            return readByte(I2C_ADDR, (uint8_t)Reg::INPUT_PORT);
        }

        uint8_t output(const uint8_t port, const uint8_t v)
        {
            return writeBit(I2C_ADDR, (uint8_t)Reg::OUTPUT_PORT, port, v & 0x01);
        }
        uint8_t output(const uint8_t v)
        {
            return writeByte(I2C_ADDR, (uint8_t)Reg::OUTPUT_PORT, v);
        }
        uint8_t output()
        {
            return readByte(I2C_ADDR, (uint8_t)Reg::OUTPUT_PORT);
        }

        uint8_t polarity(const uint8_t port, const Polarity pol)
        {
            return writeBit(I2C_ADDR, (uint8_t)Reg::POLARITY, port, (uint8_t)pol);
        }
        uint8_t polarity(const Polarity pol)
        {
            uint8_t d = (pol == Polarity::ORIGINAL) ? 0x00 : 0xFF;
            return writeByte(I2C_ADDR, (uint8_t)Reg::POLARITY, (uint8_t)d);
        }
        uint8_t polarity()
        {
            return readByte(I2C_ADDR, (uint8_t)Reg::POLARITY);
        }

        uint8_t config(const uint8_t port, const Config cfg)
        {
            return writeBit(I2C_ADDR, (uint8_t)Reg::CONFIG, port, (uint8_t)cfg);
        }
        uint8_t config(const Config cfg)
        {
            uint8_t d = (cfg == Config::OUT) ? 0x00 : 0xFF;
            return writeByte(I2C_ADDR, (uint8_t)Reg::CONFIG, (uint8_t)d);
        }
        uint8_t config()
        {
            return readByte(I2C_ADDR, (uint8_t)Reg::CONFIG);
        }

        uint8_t status() const { return sts; }

    private:

        uint8_t readBit(uint8_t dev, uint8_t reg, uint8_t bit)
        {
            uint8_t b = readByte(dev, reg);
            b &= (1 << bit);
            return b;
        }

        uint8_t readByte(uint8_t dev, uint8_t reg)
        {
            uint8_t data;
            readBytes(dev, reg, 1, &data);
            return data;
        }

        int8_t readBytes(uint8_t dev, uint8_t reg, uint8_t size, uint8_t *data)
        {
            wire->beginTransmission(dev);
            wire->write(reg);
            wire->endTransmission();
            wire->requestFrom(dev, size);
            int8_t count = 0;
            while (wire->available()) data[count++] = wire->read();
            return count;
        }

        bool writeBit(uint8_t dev, uint8_t reg, uint8_t bit, uint8_t data)
        {
            uint8_t b = readByte(dev, reg);
            b = (data != 0) ? (b | (1 << bit)) : (b & ~(1 << bit));
            return writeByte(dev, reg, b);
        }

        bool writeByte(uint8_t dev, uint8_t reg, uint8_t data)
        {
            return writeBytes(dev, reg, 1, &data);
        }

        bool writeBytes(uint8_t dev, uint8_t reg, uint8_t size, uint8_t* data)
        {
            wire->beginTransmission(dev);
            wire->write(reg);
            for (uint8_t i = 0; i < size; i++)
                wire->write(data[i]);
            sts = wire->endTransmission();
            if (sts != 0)
            {
                Serial.print("I2C ERROR : ");
                Serial.println(sts);
            }
            return (sts == 0);
        }

    };

}

using TCA9534 = arduino::IoEx8bit<TwoWire, 8>;

#endif // ARDUINO_TCA9534_H
