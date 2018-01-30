#ifndef __SAE5300_GPR916_LOGGING_H__
#define __SAE5300_GPR916_LOGGING_H__

#include <string>
#include <sstream>
#include <exception>
#include <Windows.h>

namespace SAE {
  namespace Log {

    HANDLE& getOutputHandle();

    static bool __initLog() {
      if(!AllocConsole()) {
        throw std::exception("Failed to alloc console");
      }

      getOutputHandle() = GetStdHandle(STD_OUTPUT_HANDLE);
      return true;
    }

    static bool __shutDownLog() {
      if(getOutputHandle())
        getOutputHandle() = 0;
      
      FreeConsole();
      return true;
    }

    static bool __log(
      const std::string &message, 
      HANDLE            &stdOutHandle)
    {
      if(!stdOutHandle)
        return false;

      WriteConsole(stdOutHandle, message.c_str(), message.length(), nullptr, nullptr);
      return true;
    }

    #define Log(message_expression)       \
    {                                     \
      std::stringstream ss;               \
      ss << message_expression;           \
      __log(ss.str(), getOutputHandle()); \
    }

  }
}

#endif