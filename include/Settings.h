#pragma once

#include <cstdint>

namespace Settings
{
    namespace WiFi
    {
        static const std::string SSID( "******" );
        static const std::string Password( "******" );
    }

    namespace MQTT
    {
        // The time [ms] between publishing values to the MQTT broker
        // e.g.: 200 ms = values are sent to the MQTT broker 5 times per second
        static const uint32_t Message_Interval_ms(200);
        static const std::string ServerName("test.mosquitto.org");
        static const uint32_t ServerPort(1883);
    }

    namespace Pins
    {
        // Connect this digital pin to the left switch of joystick of player 1. The other side of the switch must go to GND.
        static const uint8_t P1_Left(13);

        // Connect this digital pin to the right switch of joystick of player 1. The other side of the switch must go to GND.
        static const uint8_t P1_Right(12);

        // Connect this digital pin to the left switch of joystick of player 2. The other side of the switch must go to GND.
        static const uint8_t P2_Left(27);

        // Connect this digital pin to the right switch of joystick of player 2. The other side of the switch must go to GND.
        static const uint8_t P2_Right(14);
    }
}