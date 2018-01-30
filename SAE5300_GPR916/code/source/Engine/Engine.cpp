#include <map>

#include "Logging/Logging.h"

#include "Engine/Engine.h"

namespace SAE {
  namespace Engine {
    using namespace SAE::Log;

    bool Engine
      ::initialize()
    {
      Camera::Properties cameraProperties ={};
      cameraProperties.aspectRatio  = 1920.0/1080.0;
      cameraProperties.fieldOfViewY = 45.0;
      cameraProperties.nearPlane    = 0.05;
      cameraProperties.farPlane     = 1000.0;

      m_defaultCamera = Camera(cameraProperties);
      m_defaultCamera.initialize();

      return true;
    }

    bool Engine
      ::update(
        const Timer::State &time,
        const InputState   &inputState)
    {
      m_defaultCamera.update();
      
      return true;
    }

    bool Engine
      ::deinitialize()
    {
      m_defaultCamera.deinitialize();

      return true;
    }

  }
}