#include <string>
#include <algorithm>
#include "Player.h"

Player::Player( std::string id, uint8_t buttonPinLeft, uint8_t buttonPinRight )
  : Component(id)
  , m_buttonLeft( id + ".Button.Left", buttonPinLeft)
  , m_buttonRight( id + ".Button.Right", buttonPinRight)
{}

void Player::Init()
{
    m_buttonLeft.Init();
    m_buttonRight.Init();
}

float Player::GetClicksPerSecond()
{
  return (1000.0f * m_clickTimeStamps.size()) / c_clicksPerSecondsWindow_ms;
}

void Player::Update(unsigned long now_ms)
{
  m_buttonLeft.Update( now_ms );
  m_buttonRight.Update( now_ms );

  auto buttonToCheck = m_lastClickWasRight ? m_buttonLeft : m_buttonRight;
 
  if( buttonToCheck.IsFallingEdge())
  {
    m_clickTimeStamps.emplace_back(now_ms);
    m_lastClickWasRight = !m_lastClickWasRight;
  }

  ForgetOldClicks( now_ms );
}

void Player::ForgetOldClicks(unsigned long now_ms)
{
  /*
   * C++20 and up only... => Not supported by current platform.io (?)
   *
  std::erase_if(
    m_clickTimeStamps,
    [this, now_ms](unsigned long timeStamp)
    {
      return timeStamp < now_ms - c_clicksPerSecondsWindow_ms;
    }
  );
  */

  m_clickTimeStamps.erase(
    std::remove_if(
        m_clickTimeStamps.begin(),
        m_clickTimeStamps.end(),
        [this, now_ms](unsigned long timeStamp) {
            return timeStamp < now_ms - c_clicksPerSecondsWindow_ms;
        }
    ),
    m_clickTimeStamps.end()
  );
}
