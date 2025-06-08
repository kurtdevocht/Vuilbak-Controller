#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Component.h"
#include "Button.h"

class Player
  : Component
{
  private:
    const unsigned long c_clicksPerSecondsWindow_ms = 3000;
    std::vector<unsigned long> m_clickTimeStamps;
    Button m_buttonLeft;
    Button m_buttonRight;
    bool m_lastClickWasRight;

  public:
    Player( std::string id, uint8_t buttonPinLeft, uint8_t buttonPinRight);
    float GetClicksPerSecond();

  public:
    void Init() override;
    void Update( unsigned long time_ms ) override;

  private:
    void ForgetOldClicks( unsigned long now_ms );
};