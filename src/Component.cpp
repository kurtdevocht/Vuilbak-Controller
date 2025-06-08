#include "Component.h"
#include <Arduino.h>

Component::Component(std::string id)
  : m_id(id)
{
}

void Component::Log( std::string message )
{
  if( !Serial )
  {
    return;
  }
  
  Serial.printf( "%u -- %s -- %s\n",
    millis(),
    m_id.c_str(),
    message.c_str() );
}