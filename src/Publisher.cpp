#include "Publisher.h"

Publisher::Publisher( std::string id, WiFiClient wiFiClient, std::string mqttServerName )
    : Component( id )
    , m_mqttClient( wiFiClient )
    , m_mqttServerName( mqttServerName )
{

}

void Publisher::Init()
{
    m_mqttClient.setServer( m_mqttServerName.c_str(), 1883 );
    // m_mqttClient.setCallback(callback);
}

void Publisher::Update(unsigned long now_ms)
{
    if (!m_mqttClient.connected())
    {
        this->Reconnect();
    }
    m_mqttClient.loop();
}

void Publisher::PublishGameState( uint32_t p1Power, uint32_t p2Power )
{
    m_mqttClient.publish( "Supervlieg2025/P1Power", std::to_string( p1Power ).c_str() );
    m_mqttClient.publish( "Supervlieg2025/P2Power", std::to_string( p2Power ).c_str() );
}

void Publisher::Reconnect()
{
    // Loop until we're reconnected
    while ( !m_mqttClient.connected() )
    {
        this->Log( "Attempting MQTT connection..." );
   
        // Attempt to connect
        if ( m_mqttClient.connect( m_id.c_str() ) )
        {
            this->Log( "MQTT connected" );
            break;
        } 
    
        this->Log( "MQTT Connection failed, retry in 5 seconds..." );
        // Error code is in m_mqttClient.state()
        delay( 5000 );
    }
}