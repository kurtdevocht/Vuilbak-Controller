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
    
    m_mqttClient.loop();
}

void MQTTProxy::DisplayMessage( std::string message )
{
    std::ostringstream json;
    json << "{\"bericht\":\"" << message << "\", \"kleur\":255}";

    this->CheckConnectionAndPublish(
        Settings::MQTT::Topics::DisplayMessage,
        json.str()
    );
}

void MQTTProxy::DisplayMessage( std::string message, int timeSeconds )
{
    std::ostringstream json;
    json << "{\"bericht\":\"" << message << "\", \"tijd\":" << timeSeconds << ", \"kleur\":255}";

    this->CheckConnectionAndPublish(
        Settings::MQTT::Topics::DisplayMessage,
        json.str()
    );
}

void MQTTProxy::DisplayCountdown( int value )
{
    std::ostringstream json;
    json << "{\"tijd\":" << value << ", \"mode\":1, \"kleur\": 64252}";
    
    this->CheckConnectionAndPublish(
        Settings::MQTT::Topics::DisplayMessage,
        json.str()
    );
}

void MQTTProxy::AnnounceGameStart( int playTime )
{
    std::ostringstream json;
    json << "{\"start\":" << playTime << "}";

    this->CheckConnectionAndPublish(
        Settings::MQTT::Topics::Vuilbak1Message,
        json.str()
    );

    this->CheckConnectionAndPublish(
        Settings::MQTT::Topics::Vuilbak2Message.c_str(),
        json.str()
    );
}

void MQTTProxy::CheckConnectionAndPublish( std::string topic, std::string value)
{
    this->CheckConnection();
     m_mqttClient.publish( topic.c_str(), value.c_str() );
}

void MQTTProxy::PublishScore()
{
    std::ostringstream json;
    json
        << "{\"bericht\":\""
        << (g_Vuilbak1Score < 10 ? " " : "")
        << g_Vuilbak1Score
        << "-"
        << (g_Vuilbak2Score < 10 ? " " : "")
        << g_Vuilbak2Score
        << "\", \"tijd\":1, \"kleur\":64639}";

    this->CheckConnectionAndPublish(
        Settings::MQTT::Topics::DisplayMessage,
        json.str()
    );
}

void MQTTProxy::PublishRunningGameState( uint32_t p1Power, uint32_t p2Power, float p1CPS, float p2CPS )
{
    auto now = millis();
    if( now - m_lastPublish < Settings::MQTT::Message_Interval_ms)
    {
        return;
    }

    m_lastPublish = now;
    this->CheckConnectionAndPublish( Settings::MQTT::Topics::Vuilbak1Message, this->BuildDekselString( p1Power ) );
    this->CheckConnectionAndPublish( Settings::MQTT::Topics::Vuilbak2Message, this->BuildDekselString( p2Power ) );

    this->CheckConnectionAndPublish( Settings::MQTT::Topics::Vuilbak1CPS, this->BuildCPSString( p1CPS ) );
    this->CheckConnectionAndPublish( Settings::MQTT::Topics::Vuilbak2CPS, this->BuildCPSString( p2CPS ) );

    if( g_ScoreUpdated )
    {
        this->PublishScore();
    }
}

void MQTTProxy::PublishEndGameState()
{
    this->CheckConnection();

    this->CheckConnectionAndPublish( Settings::MQTT::Topics::Vuilbak1Message, this->BuildDekselString( 0 ) );
    this->CheckConnectionAndPublish( Settings::MQTT::Topics::Vuilbak2Message, this->BuildDekselString( 0 ) );

    // https://rgbcolorpicker.com/565
    std::string jsonGreen = "{\"strip\":60, \"result\":1, \"kleur\": 2016";
    std::string jsonRed = "{\"strip\":60, \"result\":1, \"kleur\": 63488";
    std::string jsonPurple = "{\"strip\":60, \"result\":1, \"kleur\": 63775";

    if( g_Vuilbak1Score > g_Vuilbak2Score )
    {
        this->DisplayMessage( Settings::Game::Messages::Team1Wins );
        this->CheckConnectionAndPublish( Settings::MQTT::Topics::Vuilbak1Message, jsonGreen.c_str() );
        this->CheckConnectionAndPublish( Settings::MQTT::Topics::Vuilbak2Message, jsonRed.c_str() );
    }
    else if( g_Vuilbak2Score > g_Vuilbak1Score )
    {
        this->DisplayMessage( Settings::Game::Messages::Team2Wins );
        this->CheckConnectionAndPublish( Settings::MQTT::Topics::Vuilbak1Message, jsonRed );
        this->CheckConnectionAndPublish( Settings::MQTT::Topics::Vuilbak2Message, jsonGreen );
    }
    else
    {
        this->DisplayMessage( Settings::Game::Messages::BothWin );
        this->CheckConnectionAndPublish( Settings::MQTT::Topics::Vuilbak1Message, jsonPurple );
        this->CheckConnectionAndPublish( Settings::MQTT::Topics::Vuilbak2Message, jsonPurple );
    }
}

void MQTTProxy::CheckConnection()
{
    if (!m_mqttClient.connected())
    {
       this->Reconnect();
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

std::string MQTTProxy::BuildCPSString( float value )
{
    std::ostringstream raw;
    raw << "Currently wiggling @ " << value << " CPS";
    return raw.str();
}