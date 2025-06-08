#include <Arduino.h>
#include "Player.h"
#include "Publisher.h"
#include "main.h"
#include "Settings.h"
#include <WiFi.h>

auto m_player1 = Player( "P1", Settings::Pin_P1_Left, Settings::Pin_P1_Right );
auto m_player2 = Player( "P2", Settings::Pin_P2_Left, Settings::Pin_P2_Right );
auto m_wiFiClient = WiFiClient();
auto m_mqttClient = PubSubClient(m_wiFiClient);
auto m_publisher = Publisher( "Vuilbak-Controller", m_mqttClient, "test.mosquitto.org");

void setup() {
  InitSerial();
  InitWiFi();

  m_player1.Init();
  m_player2.Init();
  m_publisher.Init();  
}

void loop() {
  auto time_ms = millis();
  m_player1.Update( time_ms );
  m_player2.Update( time_ms );
  m_publisher.Update( time_ms );
  
  Serial.printf( "P1.CPS = %f - P2.CPS = %f\n", m_player1.GetClicksPerSecond(), m_player2.GetClicksPerSecond() );
  m_publisher.PublishGameState(
    (int)( m_player1.GetClicksPerSecond() * 1000 ),
    (int)( m_player2.GetClicksPerSecond() * 1000 )
  );
}

void InitSerial()
{
  Serial.begin(115200);
  while(!Serial);
}

void InitWiFi()
{
  Serial.print("WiFifying...");
  WiFi.mode(WIFI_STA);
  WiFi.begin("****", "***");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}