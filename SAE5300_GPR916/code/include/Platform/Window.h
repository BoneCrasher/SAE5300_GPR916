#ifndef __SAE5300_GPR916_WINDOW_H__
#define __SAE5300_GPR916_WINDOW_H__

#include <string>
#include "Platform/Environment.h"

namespace SAE {
  namespace Window {

    using SAE::Environment::Environment;

    class Window {
    public:
      static bool CreateMainWindow(
        const std::string            &applicationName,
        std::shared_ptr<Environment> &environment);
    };

  }
}

#endif