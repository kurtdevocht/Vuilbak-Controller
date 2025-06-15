#pragma once

#include <string.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "Component.h"

class MQTTProxy
  : Component
{
    private:
        std::string m_mqttServerName;
        uint32_t m_mqttServerPort;
        PubSubClient& m_mqttClient;
        unsigned long m_lastPublish = 0;        

    public:
        MQTTProxy( std::string id, PubSubClient& mqttClient, std::string mqttServerName, uint32_t mqttServerPort );
        void DisplayMessage( std::string message, int time );
        void DisplayCountdown( int value );
        void AnnounceGameStart( int playTime );
        void PublishRunningGameState( uint32_t p1Power, uint32_t p2Power );
        void PublishEndGameState();

    public:
        void Init() override;
        void Update( unsigned long time_ms ) override;

    private:
        void PublishScore();
        void Reconnect();
        std::string BuildDekselString( int value );
};