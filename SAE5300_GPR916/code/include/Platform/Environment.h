#ifndef __SAE5300_GPR916_ENVIRONMENT_H__
#define __SAE5300_GPR916_ENVIRONMENT_H__

#include <memory>
#include <stdint.h>
#include <Windows.h>

namespace SAE {
  namespace Environment {

    struct DisplayEnvironment {
      int32_t width;
      int32_t height;
    };

    class Environment {
    public:
      static std::shared_ptr<Environment> Create(HINSTANCE instanceHandle);

      inline const HINSTANCE&          instanceHandle()   const { return _instanceHandle;   }
      inline const DisplayEnvironment& display()          const { return _display;          }
      inline const HWND&               mainWindowHandle() const { return _mainWindowHandle; }

      inline void setMainWindowHandle(const HWND &handle) { _mainWindowHandle = handle; }

    protected:
      Environment(
        HINSTANCE          instanceHandle,
        DisplayEnvironment display);

    private:
      HINSTANCE          _instanceHandle;
      HWND               _mainWindowHandle;
      DisplayEnvironment _display;
    };

  }
}

#endif