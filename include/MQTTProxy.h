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
        void DisplayMessage( std::string message );
        void DisplayMessage( std::string message, int time );
        void DisplayCountdown( int value );
        void AnnounceGameStart( int playTime );
        void PublishRunningGameState( uint32_t p1Power, uint32_t p2Power, float p1CPS, float p2CPS );
        void PublishEndGameState();

    public:
        void Init() override;
        void Update( unsigned long time_ms ) override;

    private:
        void CheckConnection();
        void Reconnect();    
        void PublishScore();
        void CheckConnectionAndPublish( std::string topic, std::string value);
        std::string BuildDekselString( int value );
        std::string BuildCPSString( float value );
};