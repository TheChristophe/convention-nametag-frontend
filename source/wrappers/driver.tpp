#include "driver.hpp"

#include <cstring>

namespace Wrappers {
    template <class DeviceType>
    Driver<DeviceType>::Driver()
    {
        if (not Hardware::Init()) {
            throw std::runtime_error("failed to initialize hardware");
        }

        Reset();
    }

    template <class DeviceType>
    Driver<DeviceType>::~Driver()
    {
        SetPanelPower(false);
        Hardware::Exit();
    }

    template <class DeviceType>
    void Driver<DeviceType>::SetCursor(uint8_t x, uint8_t y)
    {
        if ((x > _width) || (y > _height)) {
            return;
        }

        // set page address
        WriteRegistry(DeviceType::Registry::Page + (y / 8));
        // set low column address
        WriteRegistry(DeviceType::Registry::SelectColumnLow + (x & 0x0f));
        // set high column address
        WriteRegistry(DeviceType::Registry::SelectColumnHigh + (x >> 4));

        /*
         * Cursor position is now set... but what does it do?
         */
    }

    template <class DeviceType>
    void Driver<DeviceType>::SetColor(uint8_t x, uint8_t y, ColorT color)
    {
        // TODO: handle color == black

        if (x >= _width || y >= _height) {
            return;
        }

        _buffer[x + (y / 8) * _width] |= static_cast<uint8_t>(color != 0) << (y % 8);
    }

    template <class DeviceType>
    void Driver<DeviceType>::Clear(ColorT color)
    {
        std::memset(_buffer, color, sizeof(_buffer));
    }

    template <class DeviceType>
    void Driver<DeviceType>::SetPanelPower(bool on)
    {
        if (on) {
            WriteRegistry(DeviceType::Registry::PanelOn);
        }
        else {
            WriteRegistry(DeviceType::Registry::PanelOff);
        }
    }

    template <class DeviceType>
    int Driver<DeviceType>::Driver::GetWidth() const
    {
        return _width;
    }

    template <class DeviceType>
    int Driver<DeviceType>::GetHeight() const
    {
        return _height;
    }

    template <class DeviceType>
    void Driver<DeviceType>::Reset()
    {
        Hardware::RST1();
        Hardware::DelayMS(100);
        Hardware::RST0();
        Hardware::DelayMS(400);
        Hardware::RST1();
        Hardware::DelayMS(100);
    }

    template <class DeviceType>
    void Driver<DeviceType>::WriteRegistry(uint8_t reg)
    {
        if constexpr (Hardware::UseSPI) {
            Hardware::DC0();
            Hardware::SPIWriteByte(reg);
        }
        else {
            Hardware::I2CWriteByte(reg, Pins::IICCMD);
        }
    }

    template <class DeviceType>
    void Driver<DeviceType>::WriteDataByte(uint8_t data)
    {
        if constexpr (Hardware::UseSPI) {
            Hardware::DC1();
            Hardware::SPIWriteByte(data);
        }
        else {
            Hardware::I2CWriteByte(data, Pins::IICRAM);
        }
    }

    template <class DeviceType>
    void Driver<DeviceType>::WriteData(uint8_t *buffer, uint32_t length)
    {
        if constexpr (Hardware::UseSPI) {
            Hardware::DC1();
            Hardware::SPIWriteBytes(reinterpret_cast<char *>(buffer), length);
        }
        else {
            for (uint32_t i{ 0 }; i < length; buffer++, i++) {
                Hardware::I2CWriteByte(*buffer, Pins::IICRAM);
            }
        }
    }
} // namespace Wrappers
