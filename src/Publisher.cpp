#include "Publisher.h"
#include "Settings.h"

void callback2(char* topic, byte* payload, unsigned int length) {
    /*
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
*/
}

Publisher::Publisher( std::string id, PubSubClient& mqttClient, std::string mqttServerName )
    : Component( id )
    , m_mqttClient( mqttClient )
    , m_mqttServerName( mqttServerName )
{
    
}

void Publisher::Init()
{
    m_mqttClient.setServer( m_mqttServerName.c_str(), 1883 );
    m_mqttClient.setCallback(callback2);
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
    auto now = millis();
    if( now - m_lastPublish < Settings::MQTT_Message_Interval_ms)
    {
        return;
    }

    m_lastPublish = now;
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