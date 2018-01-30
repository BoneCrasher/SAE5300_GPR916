#include "Platform/Environment.h"

namespace SAE {
  namespace Environment {

    std::shared_ptr<Environment>
      Environment::Create(HINSTANCE instanceHandle)
    {
      DisplayEnvironment display={};

      display.width  = GetSystemMetrics(SM_CXSCREEN);
      display.height = GetSystemMetrics(SM_CYSCREEN);

      std::shared_ptr<Environment> env
        = std::shared_ptr<Environment>(
          new Environment(instanceHandle, display));

      return env;
    }

    Environment::Environment(
      HINSTANCE          instanceHandle,
      DisplayEnvironment display) 
      : _instanceHandle(instanceHandle)
      , _mainWindowHandle(0)
      , _display(display)
    {
    }

  }
}