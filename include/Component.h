#pragma once

#include <string>

class Component
{
  protected:
    const std::string m_id;

  public:
    Component( std::string id );
    virtual void Init() = 0;
    virtual void Update( unsigned long now_ms ) = 0;

  protected:
    void Log( std::string message );
/*
    virtual ~Component() = default;
*/
};