#ifndef __SAE5300_GPR916_RENDERER_H__
#define __SAE5300_GPR916_RENDERER_H__

#include "Platform/DirectX11/DirectX11Environment.h"
#include "Platform/DirectX11/DirectX11ResourceManager.h"
#include "Platform/Timer.h"
#include "RendererDTO.h"

namespace SAE {
  namespace Rendering {
    using SAE::DirectX11::DirectX;
    using SAE::DirectX11::DirectX11ResourceManager;
    using namespace SAE::DTO;

    class Renderer {
    public:
      Renderer(
        std::shared_ptr<DirectX>                  const& environment,
        std::shared_ptr<DirectX11ResourceManager> const& resourceManager);

      bool initialize();
      bool deinitialize();

      void render(
        SAE::Timing::Timer::State const&time,
        RenderScene               const&scene);

    private:
      std::shared_ptr<DirectX> 
        m_dx11Environment;
      std::shared_ptr<DirectX11ResourceManager> 
        m_resourceManager;

      uint64_t m_rasterizerStateId;
      uint64_t m_dsvTexHandle;
      uint64_t m_dsvViewHandle;
      uint64_t m_dssHandle;

      D3D11_VIEWPORT m_viewPort;
    };

  }
}

#endif