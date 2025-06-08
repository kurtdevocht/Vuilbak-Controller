#pragma once

#include <string.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "Component.h"

class Publisher
  : Component
{
    private:
        std::string m_mqttServerName;
        PubSubClient& m_mqttClient;
        unsigned long m_lastPublish = 0;

    public:
        Publisher( std::string id, PubSubClient& mqttClient, std::string mqttServerName );
        void PublishGameState(  uint32_t p1Power, uint32_t p2Power );

    public:
        void Init() override;
        void Update( unsigned long time_ms ) override;

    private:
        void Reconnect();
};