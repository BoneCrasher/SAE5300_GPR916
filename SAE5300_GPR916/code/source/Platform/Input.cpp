#include <memory>

#include "Platform/Input.h"

namespace SAE {
  namespace Input {

    InputState
      ::InputState()
      : m_keyStates()
    {}

    bool InputState
      ::reset()
    {
      for(std::map<KeyCode, KeyState>::value_type &entry : m_keyStates) {
        entry.second = KeyState::Released;
      }

      return true;
    }

    void InputState
      ::setPressed(
        KeyCode const &keyCode,
        bool           pressed, 
        bool           isAlt)
    {
      if(pressed) {
        m_keyStates[keyCode] = KeyState::Pressed;
        if(isAlt)
          m_keyStates[keyCode] = KeyState::AltPressed;
      }
      else {
        m_keyStates[keyCode] = KeyState::Released;
      }
    }

    void InputState
      ::setPressed(
        uint8_t const &keyCode, 
        bool           pressed,
        bool           isAlt)
    {
      setPressed(MapWinApiVKeyToKeyCode(keyCode), pressed, isAlt);
    }

    static uint8_t stateValue(const KeyState &state) {
      return static_cast<std::underlying_type_t<KeyState>>(state);
    }

    bool InputState
      ::getPressed(KeyCode const &keyCode) const
    {
      if(m_keyStates.find(keyCode) == m_keyStates.end())
        return false;

      return (stateValue(m_keyStates.at(keyCode)) & stateValue(KeyState::Pressed));
    }
  }
}