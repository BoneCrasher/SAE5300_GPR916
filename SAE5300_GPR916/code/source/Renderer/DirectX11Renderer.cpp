#include "Renderer/DirectX11Renderer.h"

#include "Platform/Timer.h"

namespace SAE {
  namespace Rendering {
    using namespace SAE::DirectX11;

    Renderer::Renderer(
      std::shared_ptr<DirectX>                  const& environment,
      std::shared_ptr<DirectX11ResourceManager> const& resourceManager)
      : m_dx11Environment(environment)
      , m_resourceManager(resourceManager)
    {}

    bool Renderer::initialize() {
      D3D11_RASTERIZER_DESC desc={};
      desc.FillMode              = D3D11_FILL_WIREFRAME;
      desc.CullMode              = D3D11_CULL_BACK;
      desc.FrontCounterClockwise = false;
      desc.AntialiasedLineEnable = true;
      desc.MultisampleEnable     = false;
      desc.ScissorEnable         = false;
      desc.DepthClipEnable       = true;

      uint64_t rasterizerStateId = m_resourceManager->create<ID3D11RasterizerState>(desc);

      return true;
    }

    bool Renderer::deinitialize() {
      return true;
    }

    void Renderer::render(SAE::Timing::Timer::State const&time) {

      ID3D11DeviceContextPtr context = m_dx11Environment->getImmediateContext();

      FLOAT color[4] ={ 0.09, 0.76f, 0.76f, 1.0f};
      context->ClearRenderTargetView(m_dx11Environment->getMainRenderTarget().get(), color);

      m_dx11Environment->getSwapChain()->Present(0, 0);
    }

  }
}