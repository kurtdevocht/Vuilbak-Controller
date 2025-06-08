#include <Arduino.h>
#include "Player.h"
#include "main.h"
#include "Settings.h"

auto m_player1 = Player( "P1", Settings::Pin_P1_Left, Settings::Pin_P1_Right );
auto m_player2 = Player( "P2", Settings::Pin_P2_Left, Settings::Pin_P2_Right );

void setup() {
  InitSerial();
  m_player1.Init();
  m_player2.Init();
}

void loop() {
  auto time_ms = millis();
  m_player1.Update( time_ms );
  m_player2.Update( time_ms );
  
  Serial.printf( "P1.CPS = %f - P2.CPS = %f\n", m_player1.GetClicksPerSecond(), m_player2.GetClicksPerSecond() );
}

void InitSerial()
{
  Serial.begin(115200);
  while(!Serial);
}