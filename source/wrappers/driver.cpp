#include "driver.hpp"

#include <cstring>

namespace Wrappers {
    Driver::Driver(Mode mode, Driver::ScanDirection scanDir)
        : _mode{ mode }
    {
        if (not Hardware::Init()) {
            throw std::runtime_error("failed to initialize hardware");
        }

        if (mode == Mode::SH1106) {
            Hardware::EnablePin(Pins::KeyUpPin);
            Hardware::EnablePin(Pins::KeyDownPin);
            Hardware::EnablePin(Pins::KeyLeftPin);
            Hardware::EnablePin(Pins::KeyRightPin);
            Hardware::EnablePin(Pins::KeyPressPin);
            Hardware::EnablePin(Pins::Key1Pin);
            Hardware::EnablePin(Pins::Key2Pin);
            Hardware::EnablePin(Pins::Key3Pin);
        }

        Reset();
        InitRegistry();

        //Set the display scan and color transfer modes
        SetScanDirection(scanDir);
        Hardware::DelayMS(200);

        Clear();
        Display();
        SetPanelPower(true);
    }

    Driver::~Driver()
    {
        switch (_mode) {
            case Mode::SH1106:
                WriteRegistry(SH1106::Registry::PanelOff);
                break;
            case Mode::SSD1305:
                WriteRegistry(SSD1305::Registry::PanelOff);
                break;
            case Mode::SSD1322:
                WriteRegistry(SSD1322::Registry::PanelOff);
                break;
        }

        Hardware::Exit();
    }

    void Driver::SetScanDirection(ScanDirection scanDir)
    {
        _state.scanDir = scanDir;

        if (_mode == Mode::SSD1322) {
            if (scanDir == ScanDirection::LeftRight_UpDown || scanDir == ScanDirection::LeftRight_DownUp
                || scanDir == ScanDirection::RightLeft_UpDown || scanDir == ScanDirection::RightLeft_DownUp) {
                _state.width  = SSD1322::Width;
                _state.height = SSD1322::Height;
            }
            else {
                _state.width  = SSD1322::Height;
                _state.height = SSD1322::Width;
            }
        }
        else if (_mode == Mode::SH1106) {
            if (scanDir == ScanDirection::LeftRight_UpDown || scanDir == ScanDirection::LeftRight_DownUp
                || scanDir == ScanDirection::RightLeft_UpDown || scanDir == ScanDirection::RightLeft_DownUp) {
                _state.width  = SH1106::Width;
                _state.height = SH1106::Height;
            }
            else {
                _state.width  = SH1106::Height;
                _state.height = SH1106::Width;
            }
        }
        else if (_mode == Mode::SSD1305) {
            if (scanDir == ScanDirection::LeftRight_UpDown || scanDir == ScanDirection::LeftRight_DownUp
                || scanDir == ScanDirection::RightLeft_UpDown || scanDir == ScanDirection::RightLeft_DownUp) {
                _state.width  = SSD1305::Width;
                _state.height = SSD1305::Height;
            }
            else {
                _state.width  = SSD1305::Height;
                _state.height = SSD1305::Width;
            }
        }
    }

    void Driver::SetCursor(uint8_t x, uint8_t y)
    {
        if ((x > _state.width) || (y > _state.height)) {
            return;
        }

        // set page address
        WriteRegistry(SH1106::Registry::Page + (y / 8));
        // set low column address
        WriteRegistry(SH1106::Registry::SelectColumnLow + (x & 0x0f));
        // set high column address
        WriteRegistry(SH1106::Registry::SelectColumnHigh + (x >> 4));

        /*
         * Cursor position is now set... but what does it do?
         */
    }

    void Driver::SetColor(uint8_t x, uint8_t y, ColorT color)
    {
        // TODO: handle color == black

        if (x >= _state.width || y >= _state.height) {
            return;
        }

        _buffer[x + (y / 8) * _state.width] |= static_cast<uint8_t>(color != 0) << (y % 8);
    }

    void Driver::Clear(ColorT color)
    {
        std::memset(_buffer, color, sizeof(_buffer));
    }

    void Driver::Display()
    {
        uint8_t *buffer{ _buffer };

        if (_mode == Mode::SSD1322) {
            WriteRegistry(SSD1322::Registry::SetColumnAddress);
            const auto ColOffset{ 0x1C };
            WriteDataByte(static_cast<uint8_t>(ColOffset + 0x00));
            WriteDataByte(static_cast<uint8_t>(ColOffset + 0x3F));

            WriteRegistry(SSD1322::Registry::SetRowAddress);
            WriteDataByte(0x00);
            WriteDataByte(0x3F);

            WriteRegistry(SSD1322::Registry::WriteRam);

            WriteData(buffer, SSD1322::Size / 2);

            Hardware::DelayMS(0);
        }
        else {
            for (uint8_t page{ 0 }; page < 8; page++, buffer += _state.width) {
                // set page address
                WriteRegistry(SH1106::Registry::Page + page);

                // reset column address
                if (_mode == Mode::SH1106) {
                    WriteRegistry(SH1106::Registry::SelectColumnLow);
                }
                else if (_mode == Mode::SSD1305) {
                    WriteRegistry(0x04);
                }
                WriteRegistry(SH1106::Registry::SelectColumnHigh);

                // write display buffer
                WriteData(buffer, _state.width);
            }
        }
    }

    void Driver::SetPanelPower(bool on)
    {
        if (on) {
            if (_mode == Mode::SSD1322) {
                WriteRegistry(SSD1322::Registry::PanelOn);
            }
            else if (_mode == Mode::SSD1305) {
                WriteRegistry(SSD1305::Registry::PanelOn);
            }
            else if (_mode == Mode::SH1106) {
                WriteRegistry(SH1106::Registry::PanelOn);
            }
        }
        else {
            if (_mode == Mode::SSD1322) {
                WriteRegistry(SSD1322::Registry::PanelOff);
            }
            else if (_mode == Mode::SSD1305) {
                WriteRegistry(SSD1305::Registry::PanelOff);
            }
            else if (_mode == Mode::SH1106) {
                WriteRegistry(SH1106::Registry::PanelOff);
            }
        }
    }

    void Driver::CopyGLBuffer(const uint8_t *glBuffer)
    {
        // gl buffer is formatted in RGB, so multiply index by 3 to use R channels
        if (_mode == Mode::SSD1322) {
            for (unsigned y{ 0 }; y < 64; y++) {
                for (unsigned x{ 0 }; x < 256; x += 2) {
                    _buffer[(y * _state.width + x) / 2] = 0;

                    // This is what I used for openGL
                    // this doesn't work for my ffmpeg output
                    // TODO: figure out why this even worked in the first place (was opengl output flipped horizontally?)
                    //_buffer[(y * _state.width + x) / 2] |= glBuffer[((63 - y) * _state.width + x) * 3] & 0xF0;
                    //_buffer[(y * _state.width + x) / 2] |= glBuffer[((63 - y) * _state.width + x + 1) * 3] >> 4;

                    // left pixel is 4 high bits
                    _buffer[(y * _state.width + x) / 2] |= glBuffer[(y * _state.width + x) * 3] & 0xF0;
                    // right pixel is 4 low bits
                    _buffer[(y * _state.width + x) / 2] |= glBuffer[(y * _state.width + x + 1) * 3] >> 4;
                }
            }
        }
        else {
            for (int page{ 0 }; page < 8; page++) {
                for (int x{ 0 }; x < _state.width; x++) {
                    const auto bufferIndex{ x + page * _state.width };
                    _buffer[bufferIndex] = 0;
                }

                for (int y{ 0 }; y < 8; y++) {
                    for (int x{ 0 }; x < _state.width; x++) {
                        // set pixel on if at least 50% bright
                        const uint8_t bitValue{ glBuffer[(page * _state.width * 8 + y * _state.width + x) * 3] > 0x7F };
                        _buffer[page * _state.width + x] |= bitValue << y;
                    }
                }
            }
        }
    }

    uint8_t Driver::GetKeyUp()
    {
        return Hardware::ReadPin(Pins::KeyUpPin);
    }

    uint8_t Driver::GetKeyDown()
    {
        return Hardware::ReadPin(Pins::KeyDownPin);
    }

    uint8_t Driver::GetKeyLeft()
    {
        return Hardware::ReadPin(Pins::KeyLeftPin);
    }

    uint8_t Driver::GetKeyRight()
    {
        return Hardware::ReadPin(Pins::KeyRightPin);
    }

    uint8_t Driver::GetKeyPress()
    {
        return Hardware::ReadPin(Pins::KeyPressPin);
    }

    uint8_t Driver::GetKey1()
    {
        return Hardware::ReadPin(Pins::Key1Pin);
    }
    uint8_t Driver::GetKey2()
    {
        return Hardware::ReadPin(Pins::Key2Pin);
    }
    uint8_t Driver::GetKey3()
    {
        return Hardware::ReadPin(Pins::Key3Pin);
    }

    int Driver::GetWidth() const
    {
        return _state.width;
    }

    int Driver::GetHeight() const
    {
        return _state.height;
    }

    void Driver::Reset()
    {
        Hardware::RST1();
        Hardware::DelayMS(100);
        Hardware::RST0();
        Hardware::DelayMS(400);
        Hardware::RST1();
        Hardware::DelayMS(100);
    }

    void Driver::InitRegistry()
    {
        if (_mode == Mode::SSD1322) {
            WriteRegistry(SSD1322::Registry::SetCommandLock);
            WriteDataByte(0x12); // Unlock OLED driver IC

            WriteRegistry(SSD1322::Registry::PanelOff);

            WriteRegistry(SSD1322::Registry::SetClockDivider);
            WriteDataByte(0x91);

            WriteRegistry(SSD1322::Registry::SetMultiplexRatio);
            WriteDataByte(0x3F); // duty = 1/64

            WriteRegistry(SSD1322::Registry::SetDisplayOffset);
            WriteDataByte(0x00);

            WriteRegistry(SSD1322::Registry::SetStartLine);
            WriteDataByte(0x00);

            WriteRegistry(SSD1322::Registry::SetRemap);
            WriteDataByte(0x14); //Horizontal address increment,Disable Column Address Re-map,Enable Nibble Re-map,Scan from COM[N-1] to COM0,Disable COM Split Odd Even
            WriteDataByte(0x11); //Enable Dual COM mode

            WriteRegistry(SSD1322::Registry::SetGPIO);
            WriteDataByte(0x00); // Disable GPIO Pins Input

            WriteRegistry(SSD1322::Registry::FunctionSelect);
            WriteDataByte(0x01); // selection external vdd

            WriteRegistry(SSD1322::Registry::DisplayEnhance);
            WriteDataByte(0xA0); // enables the external VSL
            WriteDataByte(0xFD); // 0xfFD,Enhanced low GS display quality;default is 0xb5(normal),

            WriteRegistry(SSD1322::Registry::SetContrastCurrent);
            WriteDataByte(0xFF); // 0xFF - default is 0x7f

            WriteRegistry(SSD1322::Registry::MasterCurrentControl);
            WriteDataByte(0x0F); // default is 0x0F

            WriteRegistry(SSD1322::Registry::SelectDefaultGrayscale);

            WriteRegistry(SSD1322::Registry::SetPhaseLength);
            WriteDataByte(0xE2); // default is 0x74

            WriteRegistry(SSD1322::Registry::DisplayEnhanceB);
            WriteDataByte(0x82); // Reserved;default is 0xa2(normal)
            WriteDataByte(0x20); //

            WriteRegistry(SSD1322::Registry::SetPrechargeVoltage);
            WriteDataByte(0x1F); // 0.6xVcc

            WriteRegistry(SSD1322::Registry::SetSecondPrechargePeriod);
            WriteDataByte(0x08); // default

            WriteRegistry(SSD1322::Registry::SetVComH);
            WriteDataByte(0x04); // 0.86xVcc;default is 0x04

            WriteRegistry(SSD1322::Registry::DisableInverseDisplay);

            WriteRegistry(SSD1322::Registry::ExitPartialDispaly);
        }
        else if (_mode == Mode::SH1106) {
            WriteRegistry(SH1106::Registry::PanelOff);

            WriteRegistry(SH1106::Registry::SelectColumnLow + 0x0);
            WriteRegistry(SH1106::Registry::SelectColumnHigh + 0x0);

            WriteRegistry(SH1106::Registry::SelectLine + 0x0);

            WriteRegistry(SH1106::Registry::SetContrastControl);
            WriteRegistry(SH1106::Registry::ValueContrastControlReset);

            WriteRegistry(SH1106::Registry::SegmentRemapNormal);
            WriteRegistry(SH1106::Registry::ComRowScanDirection + 0x0);

            WriteRegistry(SH1106::Registry::SetMultiplexRatio);
            WriteRegistry(SH1106::Registry::ValueMultiplexRatioDefault);

            WriteRegistry(SH1106::Registry::SetDisplayOffset);
            WriteRegistry(SH1106::Registry::ValueDisplayOffsetDefault);

            WriteRegistry(SH1106::Registry::SetDisplayClockFreq);
            WriteRegistry(SH1106::Registry::ValueDisplayClockFreqDefault);

            WriteRegistry(SH1106::Registry::SetChargePeriod);
            WriteRegistry(0xF1);

            WriteRegistry(SH1106::Registry::SetComPinsHWConf);
            WriteRegistry(SH1106::Registry::ValueComPinsHWConfDefault);

            WriteRegistry(SH1106::Registry::SetVCOMH);
            WriteRegistry(0x40); //Set VCOM Deselect Level

            WriteRegistry(SH1106::Registry::SetPageAddressingMode);
            WriteRegistry(0x02);

            WriteRegistry(SH1106::Registry::DisableForceDisplayOn);
            WriteRegistry(SH1106::Registry::DisableInverseDisplay);
        }
        else if (_mode == Mode::SSD1305) {
            WriteRegistry(SSD1305::Registry::PanelOff);
            WriteRegistry(SSD1305::Registry::SelectColumnLow + 0x04);
            WriteRegistry(SSD1305::Registry::SelectColumnHigh + 0x00);
            WriteRegistry(SSD1305::Registry::SetDisplayStartLine);
            WriteRegistry(SSD1305::Registry::SetContrastControl);
            WriteRegistry(0x80); // ?
            WriteRegistry(SSD1305::Registry::SetSegmentRemap + 0x1);
            WriteRegistry(SSD1305::Registry::DisableInverseDisplay);
            WriteRegistry(SSD1305::Registry::SetMultiplexRatio);
            WriteRegistry(0x1F);
            WriteRegistry(SSD1305::Registry::SetComOutputScanDir);
            WriteRegistry(SSD1305::Registry::SetDisplayOffset);
            WriteRegistry(0x00);
            WriteRegistry(SSD1305::Registry::SetDisplayOscillatorFrequency);
            WriteRegistry(0xF0);
            WriteRegistry(SSD1305::Registry::SetAreaColorMode);
            WriteRegistry(0x05);
            WriteRegistry(SSD1305::Registry::SetPrechargePeriod);
            WriteRegistry(0xC2); //Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
            WriteRegistry(SSD1305::Registry::SetCOMPinsHardwareConfiguration);
            WriteRegistry(0x12);
            WriteRegistry(SSD1305::Registry::SetVComH);
            WriteRegistry(0x08);
        }
    }

    void Driver::WriteRegistry(uint8_t reg)
    {
        if constexpr (Hardware::UseSPI) {
            Hardware::DC0();
            Hardware::SPIWriteByte(reg);
        }
        else {
            Hardware::I2CWriteByte(reg, Pins::IICCMD);
        }
    }

    void Driver::WriteDataByte(uint8_t data)
    {
        if constexpr (Hardware::UseSPI) {
            Hardware::DC1();
            Hardware::SPIWriteByte(data);
        }
        else {
            Hardware::I2CWriteByte(data, Pins::IICRAM);
        }
    }

    void Driver::WriteData(uint8_t *buffer, uint32_t length)
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
