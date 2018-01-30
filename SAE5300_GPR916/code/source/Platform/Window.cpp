#include "Errors/ErrorHandling.h"
#include "Logging/Logging.h"

#include "Platform/Window.h"

namespace SAE {
  namespace Window {

    using namespace SAE::Error;
    using namespace SAE::Log;

    // Window Procedures
    // Reference: typedef LRESULT (CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
    LRESULT CALLBACK __sae5300_gpr916_defaultClass_wndProc(
      HWND   windowHandle,
      UINT   message,
      WPARAM wParam,
      LPARAM lParam)
    {
      return DefWindowProc(windowHandle, message, wParam, lParam);
    }
    
    bool Window::CreateMainWindow(
      const std::string            &applicationName,
      std::shared_ptr<Environment> &environment)
    {
      std::string windowClassName = "SAE5300_GPR916_DefaultClass";

      WNDCLASSEX windowClass ={};
      windowClass.cbSize        = sizeof(WNDCLASSEX);
      windowClass.lpszClassName = windowClassName.c_str();
      windowClass.lpszMenuName  = nullptr;
      windowClass.hInstance     = environment->instanceHandle();
      windowClass.lpfnWndProc   = __sae5300_gpr916_defaultClass_wndProc;
      windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
      windowClass.style         = (CS_VREDRAW | CS_HREDRAW | CS_DROPSHADOW);

      if(!RegisterClassEx(&windowClass)) {
        Log("RegisterClassEx failed: " << GetLastError());
        return false;
      }

      DWORD windowStyle = 0;
      windowStyle |= WS_OVERLAPPEDWINDOW;
      windowStyle |= WS_VISIBLE;

      HWND windowHandle = CreateWindowEx(
        0,
        windowClassName.c_str(), 
        applicationName.c_str(),
        windowStyle, 
        0,    0,
        environment->display().width, environment->display().height, 
        NULL, 
        NULL, 
        environment->instanceHandle(), 
        nullptr);
      if(!windowHandle) {
        DWORD lastError = GetLastError();
        Log("CreateWindow failed (" << lastError << ")." << std::endl);
        return false;
      }

      environment->setMainWindowHandle(windowHandle);

      return true;
    }

  }
}