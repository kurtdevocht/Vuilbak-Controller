#include "MQTTProxy.h"
#include "Settings.h"
#include <sstream>

// Quick imlementation: just some globals... Deadline is approaching 8-|
int g_Vuilbak1Score ( 0 );
int g_Vuilbak2Score ( 0 );
bool g_ScoreUpdated ( false );

int BytesToInt( byte * bytes, unsigned int length )
{
    char buffer [length + 1];
    memcpy( buffer, bytes, length );
    buffer[length] = '\0';

    char * pEnd;
    int i = (int)strtol ( buffer, &pEnd, 10 );
    return i;
}

void Callback( char* topic, byte* payload, unsigned int length ) {
  
  Serial.print("MQTT Callback | ");
  Serial.print(topic);
  Serial.print( " | " );
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if( strcmp(topic, Settings::MQTT::Topics::Vuilbak1Score.c_str()) == 0 )
  {
     g_Vuilbak1Score = BytesToInt( payload, length );
     Serial.printf( "vuilbak_1_score = %d", g_Vuilbak1Score);
     Serial.println();
     g_ScoreUpdated = true;     
  }

  if( strcmp(topic, Settings::MQTT::Topics::Vuilbak2Score.c_str()) == 0 )
  {
     g_Vuilbak2Score = BytesToInt( payload, length );
     Serial.printf( "vuilbak_2_score = %d", g_Vuilbak2Score);
     Serial.println();
     g_ScoreUpdated = true;
  }
}

MQTTProxy::MQTTProxy( std::string id, PubSubClient& mqttClient, std::string mqttServerName, uint32_t mqttServerPort )
    : Component( id )
    , m_mqttClient( mqttClient )
    , m_mqttServerName( mqttServerName )
    , m_mqttServerPort(mqttServerPort)
{
    
}

void MQTTProxy::Init()
{
    m_mqttClient.setServer( m_mqttServerName.c_str(),  m_mqttServerPort );
    m_mqttClient.setCallback( Callback );
    this->Reconnect();
}

void MQTTProxy::Update(unsigned long now_ms)
{
    if (!m_mqttClient.connected())
    {
       this->Reconnect();
    }
    m_mqttClient.loop();
}

void MQTTProxy::DisplayMessage( std::string message, int timeSeconds )
{
    std::ostringstream json;
    json << "{\"bericht\":\"" << message << "\", \"tijd\":" << timeSeconds << "}";
    m_mqttClient.publish(
        Settings::MQTT::Topics::DisplayMessage.c_str(),
        json.str().c_str()
    );
}

void MQTTProxy::DisplayCountdown( int value )
{
    std::ostringstream json;
    json << "{\"tijd\":" << value << ", \"mode\":1}";
    m_mqttClient.publish(
        Settings::MQTT::Topics::DisplayMessage.c_str(),
        json.str().c_str()
    );
}

void MQTTProxy::AnnounceGameStart( int playTime )
{
    std::ostringstream json;
    json << "{\"start\":" << playTime << "}";
    m_mqttClient.publish(
        Settings::MQTT::Topics::Vuilbak1Message.c_str(),
        json.str().c_str()
    );
     m_mqttClient.publish(
        Settings::MQTT::Topics::Vuilbak2Message.c_str(),
        json.str().c_str()
    );
}

void MQTTProxy::PublishScore()
{
    std::ostringstream json;
    json
        << "{\"bericht\":\"  "
        << (g_Vuilbak1Score < 10 ? " " : "")
        << g_Vuilbak1Score
        << "  -  "
        << (g_Vuilbak2Score < 10 ? " " : "")
        << g_Vuilbak2Score
        << "\", \"tijd\":1";

    m_mqttClient.publish(
        Settings::MQTT::Topics::DisplayMessage.c_str(),
        json.str().c_str()
    );
}

void MQTTProxy::PublishRunningGameState( uint32_t p1Power, uint32_t p2Power )
{
    auto now = millis();
    if( now - m_lastPublish < Settings::MQTT::Message_Interval_ms)
    {
        return;
    }

    m_lastPublish = now;
    m_mqttClient.publish( Settings::MQTT::Topics::Vuilbak1Message.c_str(), this->BuildDekselString( p1Power ).c_str() );
    m_mqttClient.publish( Settings::MQTT::Topics::Vuilbak2Message.c_str(), this->BuildDekselString( p2Power ).c_str() );

    if( g_ScoreUpdated )
    {
        this->PublishScore();
    }
}

void MQTTProxy::PublishEndGameState()
{
    m_mqttClient.publish( Settings::MQTT::Topics::Vuilbak1Message.c_str(), this->BuildDekselString( 0 ).c_str() );
    m_mqttClient.publish( Settings::MQTT::Topics::Vuilbak2Message.c_str(), this->BuildDekselString( 0 ).c_str() );

    // https://rgbcolorpicker.com/565
    std::string jsonGreen = "{\"strip\":60, \"result\":1, \"kleur\": 2016";
    std::string jsonRed = "{\"strip\":60, \"result\":1, \"kleur\": 63488";
    std::string jsonPurple = "{\"strip\":60, \"result\":1, \"kleur\": 63775";

    if( g_Vuilbak1Score > g_Vuilbak2Score )
    {
        this->DisplayMessage( "<< Team 1 wint!", 5 );
        m_mqttClient.publish( Settings::MQTT::Topics::Vuilbak1Message.c_str(), jsonGreen.c_str() );
        m_mqttClient.publish( Settings::MQTT::Topics::Vuilbak2Message.c_str(), jsonRed.c_str() );
    }
    else if( g_Vuilbak2Score > g_Vuilbak1Score )
    {
        this->DisplayMessage( "Team 2 wint! >>", 5 );
        m_mqttClient.publish( Settings::MQTT::Topics::Vuilbak1Message.c_str(), jsonRed.c_str() );
        m_mqttClient.publish( Settings::MQTT::Topics::Vuilbak2Message.c_str(), jsonGreen.c_str() );
    }
    else
    {
        this->DisplayMessage( "Iedereen wint!", 5 );
        m_mqttClient.publish( Settings::MQTT::Topics::Vuilbak1Message.c_str(), jsonPurple.c_str() );
        m_mqttClient.publish( Settings::MQTT::Topics::Vuilbak2Message.c_str(), jsonPurple.c_str() );
    }
}

void MQTTProxy::Reconnect()
{
    // Loop until we're reconnected
    while ( !m_mqttClient.connected() )
    {
        this->Log( "Attempting MQTT connection..." );
   
        // Attempt to connect
        if ( m_mqttClient.connect( m_id.c_str() ) )
        {
            this->Log( "MQTT connected" );
            m_mqttClient.subscribe( Settings::MQTT::Topics::Vuilbak1Score.c_str() );
            m_mqttClient.subscribe( Settings::MQTT::Topics::Vuilbak2Score.c_str()  );
            break;
        } 
    
        this->Log( "MQTT Connection failed, retry in 5 seconds..." );
        // Error code is in m_mqttClient.state()
        delay( 5000 );
    }
}

std::string MQTTProxy::BuildDekselString( int value )
{
    std::ostringstream json;
    json << "{\"deksel\":" << value << "}";
    return json.str();
}