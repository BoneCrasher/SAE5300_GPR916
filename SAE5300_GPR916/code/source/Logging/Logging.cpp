#include "Logging/Logging.h"

namespace SAE {
  namespace Log {

    HANDLE& getOutputHandle() {
      static HANDLE outputHandle = 0;

      return outputHandle;
    }

  }
}