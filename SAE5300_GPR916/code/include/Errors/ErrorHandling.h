#ifndef __SAE5300_GPR916_ERRORHANDLING_H__
#define __SAE5300_GPR916_ERRORHANDLING_H__

#include <string>
#include <sstream>
#include <Windows.h>

namespace SAE {
  namespace Error {

    // WINAPI Helper
    struct WINAPIError {
      HRESULT     errorCode;
      std::string message;
    };

    static void HandleWINAPIError(
      const HRESULT     &error,
      const std::string &message)
    {
      if(!SUCCEEDED(error)) {
        std::stringstream ss;
        ss << "WINAPI-Error (" << error << "): " << message << std::endl;

        WINAPIError err={};
        err.errorCode = error;
        err.message   = ss.str();

        throw err;
      }
    }

  }
}

#endif