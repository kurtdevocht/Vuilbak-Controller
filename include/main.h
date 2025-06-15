#pragma once

void InitSerial();
void InitWiFi();
bool IsStartButtonPressed( unsigned long time_ms );
void InitNewGame( unsigned long time_ms );
int ClicksPerSecondToDekselValue( float clicksPerSecond, unsigned long gameRunTime_ms );
