#include <Arduino.h>
#include "Player.h"
#include "MQTTProxy.h"
#include "main.h"
#include "Settings.h"
#include <WiFi.h>

auto m_gameRunning = false;
auto m_startButton = Button( "StartButton", Settings::Pins::StartButton );
auto m_player1 = Player( "P1", Settings::Pins::P1_Left, Settings::Pins::P1_Right );
auto m_player2 = Player( "P2", Settings::Pins::P2_Left, Settings::Pins::P2_Right );
auto m_wiFiClient = WiFiClient();
auto m_mqttClient = PubSubClient(m_wiFiClient);
auto m_mqtt = MQTTProxy("Vuilbak-Controller", m_mqttClient, Settings::MQTT::ServerName, Settings::MQTT::ServerPort);
unsigned long m_gameStartedMillis;

void setup() {
  InitSerial();
  InitWiFi();

  m_player1.Init();
  m_player2.Init();
  m_startButton.Init();
  m_mqtt.Init();

  m_mqttClient.publish(Settings::MQTT::Topics::ControllerIPAddress.c_str(), WiFi.localIP().toString().c_str());

  Serial.print( "Press start..." );
}

void loop() {
  auto time_ms = millis();
  if( !m_gameRunning )
  {
    if( IsStartButtonPressed(time_ms) )
    {
      InitNewGame( time_ms );
    }
  }
  else if( time_ms <= m_gameStartedMillis + Settings::Game::PlayTime_s * 1000 )
  {
    m_player1.Update( time_ms );
    m_player2.Update( time_ms );
    m_mqtt.Update( time_ms );
    
    auto p1Deksel = ClicksPerSecondToDekselValue( m_player1.GetClicksPerSecond(), time_ms - m_gameStartedMillis );
    auto p2Deksel = ClicksPerSecondToDekselValue( m_player2.GetClicksPerSecond(), time_ms - m_gameStartedMillis );

    m_mqtt.PublishRunningGameState( p1Deksel, p2Deksel );
  }
  else
  {
    m_gameRunning = false;
    m_mqtt.PublishEndGameState();
    m_mqtt.Update( time_ms );
    delay( 5000 );
  }
}

bool IsStartButtonPressed( unsigned long time_ms )
{
  m_startButton.Update( time_ms );
  if( !m_startButton.IsFallingEdge() )
  {
    Serial.print( "." );
    return false;
  }

  Serial.println();
  return true;
}

void InitNewGame( unsigned long time_ms )
{
    Serial.print( "New game started!" );
    
    m_gameRunning = true;
    m_gameStartedMillis = time_ms;

    m_mqtt.DisplayCountdown( Settings::Game::CountdownTime );
    delay( 1000 * Settings::Game::CountdownTime );

    m_mqtt.DisplayCountdown( Settings::Game::CountdownTime + Settings::Game::PlayTime_s );
    m_mqtt.DisplayMessage( Settings::Game::StartMessage, 2 );
}

void InitSerial()
{
  Serial.begin(115200);
  while(!Serial);
}

void InitWiFi()
{
  Serial.print( "WiFifying... (Did you change SSID & Password in Settings.h or Secrets.h?)" );
  WiFi.mode( WIFI_STA );
  WiFi.begin( Settings::WiFi::SSID.c_str(), Settings::WiFi::Password.c_str() );

  while( WiFi.status() != WL_CONNECTED )
  {
    delay( 500 );
    Serial.print( "." );
  }

  Serial.println( "" );
  Serial.print( "WiFi connected! " );
  Serial.print( "IP address: " );
  Serial.println( WiFi.localIP() );
}

int ClicksPerSecondToDekselValue( float clicksPerSecond, unsigned long gameRunTime_ms )
{
  if( clicksPerSecond <= 0.0f )
  {
    return 0;
  }

  unsigned long gamePlayTime_ms = Settings::Game::PlayTime_s * 1000;
  if( gameRunTime_ms >= gamePlayTime_ms )
  {
    return 0;
  }

  float neededCpsForMax =
    Settings::Game::ClicksPerSecondNeededForMaxDekselValueAtBeginOfGame
    + gameRunTime_ms * ( Settings::Game::ClicksPerSecondNeededForMaxDekselValueAtEndOfGame - Settings::Game::ClicksPerSecondNeededForMaxDekselValueAtBeginOfGame ) / gamePlayTime_ms;

  int value = (int)(
    Settings::Game::MinDekselValue
    + clicksPerSecond * ( Settings::Game::MaxDekselValue - Settings::Game::MinDekselValue ) / neededCpsForMax
    + 0.5f
  );

  if( value > Settings::Game::MaxDekselValue )
  {
    return Settings::Game::MaxDekselValue;
  }

  return value;
}