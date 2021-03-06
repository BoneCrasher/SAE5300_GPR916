#include <string>
#include <sstream>
#include <iostream>

#include <Windows.h>

#include "Logging/Logging.h"
#include "Errors/ErrorHandling.h"

#include "Platform/Environment.h"
#include "Platform/Window.h"
#include "Platform/Input.h"
#include "Platform/Timer.h"
#include "Platform/DirectX11/DirectX11Environment.h"
#include "Platform/DirectX11/DirectX11ResourceManager.h"
#include "Platform/DirectX11/DirectX11Mesh.h"

#include "Renderer/DirectX11Renderer.h"
#include "Renderer/RendererDTO.h"


#include "Engine/Engine.h"

using namespace SAE::Log;
using namespace SAE::Error;
using namespace SAE::Input;
using namespace SAE::Timing;
using namespace SAE::DTO;

/**********************************************************************************************//**
 * \fn  int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
 *
 * \brief Window main
 *
 * \param hInstance     The instance.
 * \param hPrevInstance The previous instance.
 * \param lpCmdLine     The command line.
 * \param nCmdShow      The command show.
 *
 * \return  A WINAPI.
 **************************************************************************************************/
int WINAPI WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR     lpCmdLine,
  int       nCmdShow)
{
  try {
    __initLog();

    HRESULT result = S_OK;

    using SAE::Environment::Environment;
    using SAE::Window::Window;
    using SAE::Engine::Engine;

    std::shared_ptr<Environment> environment
      = Environment::Create(hInstance);

    std::string applicationName = "SAE5300_GPR916_Demo";

    if(!Window::CreateMainWindow(applicationName, environment)) {
      Log("Failed to create main window.");
      throw std::exception("Failed to create main window.");
    }

    // Create and initialize DirectX!
    using DirectXInstance = SAE::DirectX11::DirectX;
    DirectXInstance directXInstance;
    if(!directXInstance.initialize(environment)) {
      Log("Failed to initialize DirectX11.");
      throw std::exception("Failed to initialize DirectX11.");
    }

    // Wrap it.
    std::shared_ptr<DirectXInstance> directXInstancePtr
      = std::shared_ptr<DirectXInstance>(
        &directXInstance,
        [] (DirectXInstance *p) { return; });

    using ResourceManager = SAE::DirectX11::DirectX11ResourceManager;
    // Create resource manager with device!
    ResourceManager
      resourceManager(directXInstance.getDevice());
    // Wrap it.
    std::shared_ptr<ResourceManager> directXResourceManagerPtr
      = std::shared_ptr<ResourceManager>(
        &resourceManager,
        [] (ResourceManager *p) { return; });

    SAE::Rendering::Renderer
      directXRenderer(directXInstancePtr, directXResourceManagerPtr);
    if(!directXRenderer.initialize()) {
      // Handle errors
    }

    ShowWindow(environment->mainWindowHandle(), true);

    std::shared_ptr<Engine> pEngineInstance = std::make_shared<Engine>();
    pEngineInstance->initialize(directXResourceManagerPtr);
    // Windows (SYS-)KEYDOWN messages contain a "keycode" in the wParam-field.
    // The keycode is a value in the interval [0;255] and identifies the key
    // pressed.
    // SYSKEYDOWN tells the system, that the alt key was pressed in conjunction.
    //  
    // We are interested in whether the key is simply pressed or not.
    // Therefore, store it in a bool-array of size 256.
    InputState inputState ={};

    std::shared_ptr<Timer> pTimer = std::make_shared<Timer>();
    pTimer->initialize();

    MSG msg ={};

    // "Game Loop"
    static bool running = true;
    while(running) {
      inputState.reset();

      while(PeekMessage(&msg, environment->mainWindowHandle(), 0, 0, PM_REMOVE)) {
        // msg.message:
        //   contains a value identified by the WM_-defines for each 
        //   message type of the windows message system.
        if(WM_KEYDOWN == msg.message) {
          inputState.setPressed(msg.wParam, true);
        }

        if(WM_KEYUP == msg.message) {
          inputState.setPressed(msg.wParam, false);
        }

        if(WM_SYSKEYDOWN == msg.message) {
          inputState.setPressed(msg.wParam, true, true);
        }

        if(WM_SYSKEYUP == msg.message) {
          inputState.setPressed(msg.wParam, false, true);
        }

        // Handle window close events to have the application shut 
        // down on "x"-Button press or any other WM_QUIT event!
        if(WM_QUIT == msg.message) {
          running = false;
          break;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }

      if(!running)
        break;

      // "Real Game Loop"
      pTimer->update();
      pEngineInstance->update(pTimer->state(), inputState);
      RenderScene
        mainScene={},
        shadowMapScene={};
      // Get high level render representation
      for(uint32_t i=0; i<4; ++i) 
        for(uint32_t k=0; k < 6; ++k) {
          pEngineInstance->render(PassType::ShadowMap, shadowMapScene, i, k);
          directXRenderer.renderPass(pTimer->state(), shadowMapScene, PassType::ShadowMap);
        }

      pEngineInstance->render(PassType::Main, mainScene);
      directXRenderer.renderPass(pTimer->state(), mainScene,  PassType::Main);
    }

    pTimer->deinitialize();

    pEngineInstance->deinitialize();
    pEngineInstance = nullptr;

    __shutDownLog();

    system("PAUSE");

    return 0;
  }
  // Handle our WINAPI-Errors thrown by the helper
  catch(WINAPIError winApiError) {
    Log(winApiError.message << std::endl);
    __shutDownLog();
    system("PAUSE");
    return winApiError.errorCode;
  }
  // Catch all std::exception derivates
  catch(std::exception ex) {
    Log(ex.what() << std::endl);
    __shutDownLog();
    system("PAUSE");
    return -1;
  }
  // Catch-All for anything catchable, which was not handled before
  catch(...) {
    Log("Unknown error occurred." << std::endl);
    __shutDownLog();
    system("PAUSE");
    return -1337;
  }
}