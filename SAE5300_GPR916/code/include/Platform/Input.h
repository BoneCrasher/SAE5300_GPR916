#ifndef __SAE5300_GPR916_INPUT_H__
#define __SAE5300_GPR916_INPUT_H__

#include <map>
#include <Windows.h>

namespace SAE {
  namespace Input {

    enum class KeyCode 
      : uint8_t
    {
      A  = 'A',
      B  = 'B', 
      C  = 'C',
      D  = 'D',
      E  = 'E',
      F  = 'F', 
      G  = 'G', 
      H  = 'H', 
      I  = 'I', 
      J  = 'J',
      K  = 'K', 
      L  = 'L', 
      M  = 'M', 
      N  = 'N', 
      O  = 'O',
      P  = 'P', 
      Q  = 'Q', 
      R  = 'R', 
      S  = 'S', 
      T  = 'T',
      U  = 'U', 
      V  = 'V', 
      W  = 'W', 
      X  = 'X', 
      Y  = 'Y',
      Z  = 'Z',
      D0 = '0', 
      D1 = '1', 
      D2 = '2', 
      D3 = '3',
      D4 = '4', 
      D5 = '5', 
      D6 = '6', 
      D7 = '7',
      D8 = '8', 
      D9 = '9',
      PLUS  = VK_OEM_PLUS,
      MINUS = VK_OEM_MINUS,
      MUL   = 0, 
      DIV   = 0,
      MOD   = 0,
      ARROW_UP    = VK_UP, 
      ARROW_DOWN  = VK_DOWN,
      ARROW_LEFT  = VK_LEFT,
      ARROW_RIGHT = VK_RIGHT,
      DNUM0    = 0x60 + 0, 
      DNUM1    = 0x60 + 1, 
      DNUM2    = 0x60 + 2, 
      DNUM3    = 0x60 + 3,
      DNUM4    = 0x60 + 4, 
      DNUM5    = 0x60 + 5, 
      DNUM6    = 0x60 + 6, 
      DNUM7    = 0x60 + 7,
      DNUM8    = 0x60 + 8, 
      DNUM9    = 0x60 + 9, 
      NUMPLUS  = VK_ADD, 
      NUMMINUS = VK_SUBTRACT,
      NUMMUL   = VK_MULTIPLY, 
      NUMDIV   = VK_DIVIDE
    };

    enum class KeyState {
      Released            =  0,
      Pressed             =  1,
      Strg                =  4,
      StrgPressed         =  5,
      Alt                 =  8,
      AltPressed          =  9,
      StrgAltPressed      = 13,
      Shift               = 16,
      ShiftPressed        = 17,
      StrgShiftPressed    = 21,
      AltShiftPressed     = 25,
      StrgAltShiftPressed = 29,
    };

    // 0:  0000 0000
    // 1:  0000 0001
    // 2:  0000 0010
    // 4:  0000 0100
    // 8:  0000 1000
    // 16: 0001 0000
    // ...
    // Pressed + Alt: 1 + 8 = 0000 1001
        
    static uint8_t MapKeyCodeToWinApiVKey(const KeyCode &keyCode) {
      return (uint8_t) static_cast<std::underlying_type_t<KeyCode>>(keyCode);
    }

    static KeyCode MapWinApiVKeyToKeyCode(const uint8_t &keyCode) {
      return (KeyCode) static_cast<std::underlying_type_t<KeyCode>>(keyCode);
    }

    class InputState {
    public:
      InputState();

      void setPressed(
        KeyCode const &keyCode, 
        bool           pressed, 
        bool           isAlt = false);
      void setPressed(
        uint8_t const &keyCode, 
        bool           pressed, 
        bool           isAlt = false);
      bool getPressed(
        KeyCode const &keyCode) const;

      bool reset();

    private:
      std::map< KeyCode, KeyState> m_keyStates;
    };

  }
}

#endif