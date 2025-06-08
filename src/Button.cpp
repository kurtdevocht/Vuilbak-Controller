#include "Button.h"
#include <Arduino.h>
#include <sstream>

Button::Button(std::string id, uint8_t pin)
  : Component(id)
  , m_pin(pin)
{}

void Button::Init()
{
    pinMode(m_pin, INPUT_PULLUP);
}

void Button::Update(unsigned long now_ms)
{
  m_isRisingEdge = false;
  m_isFallingEdge = false;

  // Still in debounce period => Nothing to do
  if( now_ms < m_ignoreEdgeUntil )
  {
    return;
  }

  auto pinIsHigh = digitalRead( m_pin ) == HIGH;

  if( m_isFirstUpdate )
  {
    m_wasHighOnPreviousUpdate = pinIsHigh;
    m_isFirstUpdate = false;
    return;
  }

  m_isRisingEdge = pinIsHigh && !m_wasHighOnPreviousUpdate;
  if (m_isRisingEdge)
  {
    m_risingEdgeCount++;
  }

  m_isFallingEdge = !pinIsHigh && m_wasHighOnPreviousUpdate;
  if (m_isFallingEdge)
  {
    m_fallingEdgeCount++;
  }

  if (m_isRisingEdge || m_isFallingEdge)
  {
    SetDebounceIgnoreTime( now_ms );
    LogStatus();
  }

  m_wasHighOnPreviousUpdate = pinIsHigh;
}

unsigned long Button::GetFallingEdgesCount()
{
  return m_fallingEdgeCount;
}

unsigned long Button::GetRisingEdgesCount()
{
  return m_risingEdgeCount;
}

bool Button::IsFallingEdge()
{
  return m_isFallingEdge;
}

bool Button::IsRisingEdge()
{
  return m_isRisingEdge;
}

void Button::SetDebounceIgnoreTime(unsigned long now_ms)
{
  m_ignoreEdgeUntil = now_ms + c_debounceTimeMillis;
}

void Button::LogStatus()
{
  std::ostringstream message;
  message << "↓ " << m_fallingEdgeCount << " -- ↑ " << m_risingEdgeCount;
  Log( message.str() );
}