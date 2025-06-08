#pragma once

#include <cstdint>

class Settings
{
    public:
        static const uint8_t Pin_P1_Left = 13;
        static const uint8_t Pin_P1_Right = 12;
        static const uint8_t Pin_P2_Left = 27;
        static const uint8_t Pin_P2_Right = 14;

        static const uint32_t MQTT_Message_Interval_ms = 200;
};