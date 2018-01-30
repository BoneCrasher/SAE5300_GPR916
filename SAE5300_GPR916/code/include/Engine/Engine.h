#ifndef __SAE5300_GPR916_ENGINE_H__
#define __SAE5300_GPR916_ENGINE_H__

#include <memory>

#include "Platform/Input.h"
#include "Platform/Timer.h"

#include "Platform/DirectX11/DirectX11Camera.h"

namespace SAE {
  namespace Engine {
    using namespace SAE::Input;
    using namespace SAE::Timing;
    using Camera = SAE::DirectX11::Camera;

    class Engine {
    public:
      bool initialize();
      bool update(
        const Timer::State &time,
        const InputState   &inputState);
      bool deinitialize();

    private:
      Camera m_defaultCamera;
    };

  }
}

#endif