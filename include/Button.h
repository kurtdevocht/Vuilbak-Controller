#pragma once

#include <cstdint>
#include <string>

#include "Component.h"

class Button
  : Component
{
  private:
    const unsigned long c_debounceTimeMillis = 20;
    uint8_t m_pin;
    unsigned long m_ignoreEdgeUntil = 0;
    bool m_isFirstUpdate = true;
    bool m_wasHighOnPreviousUpdate;
    unsigned long m_fallingEdgeCount = 0;
    unsigned long m_risingEdgeCount = 0;
    bool m_isFallingEdge = false;
    bool m_isRisingEdge = false;

  public:
    Button( std::string id, uint8_t pin );
    unsigned long GetFallingEdgesCount();
    unsigned long GetRisingEdgesCount();
    bool IsFallingEdge();
    bool IsRisingEdge();

  public:
    void Init() override;
    void Update(unsigned long time_ms) override;

  private:
    void SetDebounceIgnoreTime( unsigned long now_ms );
    void LogStatus();
};