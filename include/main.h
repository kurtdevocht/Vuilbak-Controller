#pragma once

void InitSerial();
void InitWiFi();
void InitMQTT();
bool IsStartButtonPressed( unsigned long time_ms );
void InitNewGame( unsigned long time_ms );
int ClicksPerSecondToDekselValue( float clicksPerSecond, unsigned long gameRunTime_ms );
void Log( std::string message );