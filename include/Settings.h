#pragma once

#include <cstdint>

namespace Settings
{
    namespace WiFi
    {
        #if __has_include("secrets.h")
        # include "secrets.h"
        #else
        static const std::string SSID( "*****" );
        static const std::string Password( "*****" );
        #endif
    }

    namespace MQTT
    {
        // The time [ms] between publishing values to the MQTT broker
        // e.g.: 200 ms = values are sent to the MQTT broker 5 times per second
        static const uint32_t Message_Interval_ms(200);
        
        #if __has_include("secrets.h")
        # include "secrets.h"
        #else
        static const std::string ServerName("test.mosquitto.org");
        static const uint32_t ServerPort(1883);
        #endif
        
        namespace Topics
        {
            static const std::string ControllerIPAddress( "Supervlieg2025/vuilbakkenrace/controller/ipaddress" );
            static const std::string Vuilbak1Score( "Supervlieg2025/vuilbakkenrace/vuilbak/1/score" );
            static const std::string Vuilbak2Score( "Supervlieg2025/vuilbakkenrace/vuilbak/2/score" );
            static const std::string Vuilbak1Message( "Supervlieg2025/vuilbakkenrace/vuilbak/1/message" );
            static const std::string Vuilbak2Message( "Supervlieg2025/vuilbakkenrace/vuilbak/2/message" );
            static const std::string DisplayMessage( "Supervlieg2025/vuilbakkenrace/matrixdisplay/message" );
        }
    }

    namespace Pins
    {
        // Connect this digital pin to the left switch of joystick of player 1. The other side of the switch must go to GND.
        static const uint8_t StartButton(26);
        
        // Connect this digital pin to the left switch of joystick of player 1. The other side of the switch must go to GND.
        static const uint8_t P1_Left(13);

        // Connect this digital pin to the right switch of joystick of player 1. The other side of the switch must go to GND.
        static const uint8_t P1_Right(12);

        // Connect this digital pin to the left switch of joystick of player 2. The other side of the switch must go to GND.
        static const uint8_t P2_Left(27);

        // Connect this digital pin to the right switch of joystick of player 2. The other side of the switch must go to GND.
        static const uint8_t P2_Right(14);
    }

    namespace Game
    {
        static const uint32_t CountdownTime( 5 );
        static const uint32_t PlayTime( 60 );
        static const std::string StartMessage( "! Go Go Gooi !" );
    }
}