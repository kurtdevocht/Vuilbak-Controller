#include <Arduino.h>
#include "Player.h"
#include "MQTTProxy.h"
#include "main.h"
#include "Settings.h"
#include <WiFi.h>
#include <sstream>


auto m_gameRunning( false );
auto m_gameRunningChecked( false );
auto m_startButton = Button( "StartButton", Settings::Pins::StartButton );
auto m_player1 = Player( "P1", Settings::Pins::P1_Left, Settings::Pins::P1_Right );
auto m_player2 = Player( "P2", Settings::Pins::P2_Left, Settings::Pins::P2_Right );
auto m_wiFiClient = WiFiClient();
auto m_mqttClient = PubSubClient(m_wiFiClient);
auto m_mqtt = MQTTProxy("Vuilbak-Controller", m_mqttClient, Settings::MQTT::ServerName, Settings::MQTT::ServerPort);
unsigned long m_gameStartedMillis;

void setup() {
  Log( "*** Starting Vuilbak Controller ***" );
  InitSerial();
  InitWiFi();
  InitMQTT();

  m_player1.Init();
  m_player2.Init();
  m_startButton.Init();

  m_mqttClient.publish(Settings::MQTT::Topics::ControllerIPAddress.c_str(), WiFi.localIP().toString().c_str());
}

void loop() {
  auto time_ms = millis();
  if( !m_gameRunning )
  {
    if( !m_gameRunningChecked )
    {
      Log( "Game not yet running - Press the start button" );
      m_gameRunningChecked = true;
    }
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

    auto p1CPS = m_player1.GetClicksPerSecond();
    auto p2CPS = m_player2.GetClicksPerSecond();
    
    auto p1Deksel = ClicksPerSecondToDekselValue( p1CPS, time_ms - m_gameStartedMillis );
    auto p2Deksel = ClicksPerSecondToDekselValue( p2CPS, time_ms - m_gameStartedMillis );

    m_mqtt.PublishRunningGameState( p1Deksel, p2Deksel, p1CPS, p2CPS );
  }
  else
  {
    Log( "Time's up!" );
    m_gameRunning = false;
    m_gameRunningChecked = false;
    m_mqtt.PublishEndGameState();
    m_mqtt.Update( time_ms );
    delay( 5000 );
    Log( "Game done & score published" );
  }
}

bool IsStartButtonPressed( unsigned long time_ms )
{
  m_startButton.Update( time_ms );
  if( !m_startButton.IsFallingEdge() )
  {
    return false;
  }

  Serial.println();
  return true;
}

void InitNewGame( unsigned long time_ms )
{
    Log( "New game started!" );

    m_mqtt.CloseVuilbakken();

    m_mqtt.DisplayCountdown( Settings::Game::CountdownTime );
    delay( 1000 * Settings::Game::CountdownTime );

    m_gameRunning = true;
    m_gameStartedMillis = time_ms;
    m_mqtt.DisplayCountdown( Settings::Game::PlayTime_s );  
    m_mqtt.DisplayMessage( Settings::Game::Messages::Start );
    m_mqtt.AnnounceGameStart( Settings::Game::PlayTime_s );
}

void InitSerial()
{
  Log( "Starting serial port initialization..." );
  Serial.begin(115200);
  while(!Serial);
  Log( "Serial port initialized!" );
}

void InitWiFi()
{
  Log( "Starting WiFi connection..." );
  WiFi.mode( WIFI_STA );
  WiFi.setAutoReconnect( true );
  WiFi.begin( Settings::WiFi::SSID.c_str(), Settings::WiFi::Password.c_str() );

  while( WiFi.status() != WL_CONNECTED )
  {
    delay( 500 );
  }

  Log( "WiFi connected!" );

  //Log( "IP address: " + WiFi.localIP());
}

void InitMQTT()
{
  Log( "Starting MQTT initialization..." );
  m_mqtt.Init();
  m_mqtt.Update( 0 ); // Force reconnect at startup
  Log( "MQTT Initialized!");
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

void Log( std::string message )
{
  if( !Serial )
  {
    return;
  }
  
  Serial.printf( "%u -- %s -- %s\n",
    millis(),
    "main",
    message.c_str() );
}