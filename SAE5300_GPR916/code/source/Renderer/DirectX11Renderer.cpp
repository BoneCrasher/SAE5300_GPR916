#include "Renderer/DirectX11Renderer.h"

#include "Platform/Timer.h"
#include "Engine/Mesh.h"

namespace SAE {
  namespace Rendering {
    using namespace SAE::DirectX11;
    using namespace SAE::Engine;

    Renderer::Renderer(
      std::shared_ptr<DirectX>                  const& environment,
      std::shared_ptr<DirectX11ResourceManager> const& resourceManager)
      : m_dx11Environment(environment)
      , m_resourceManager(resourceManager)
    {}

    bool Renderer::initialize() {
      D3D11_RASTERIZER_DESC desc={};
      desc.FillMode              = D3D11_FILL_SOLID;
      desc.CullMode              = D3D11_CULL_NONE;
      desc.FrontCounterClockwise = false;
      desc.AntialiasedLineEnable = false;
      desc.MultisampleEnable     = false;
      desc.ScissorEnable         = false;
      desc.DepthClipEnable       = false;

      m_rasterizerStateId = m_resourceManager->create<ID3D11RasterizerState>(desc);

      m_viewPort.TopLeftX = 0;
      m_viewPort.TopLeftY = 0;
      m_viewPort.Width    = m_dx11Environment->getSelectedMode().width;
      m_viewPort.Height   = m_dx11Environment->getSelectedMode().height;
      m_viewPort.MinDepth = 0.0;
      m_viewPort.MaxDepth = 1.0;
      return true;
    }

    bool Renderer::deinitialize() {
      return true;
    }

    void Renderer::render(
      SAE::Timing::Timer::State const&time,
      RenderScene               const&scene) 
    {
      ID3D11DeviceContextPtr context = m_dx11Environment->getImmediateContext();

      ID3D11RenderTargetView *renderTarget    = m_dx11Environment->getMainRenderTarget().get();
      ID3D11RasterizerState  *rasterizerState = reinterpret_cast<ID3D11RasterizerState*>(m_rasterizerStateId);

      FLOAT color[4] ={ 0.0f, 0.0f, 0.0f, 1.0f };
      context->ClearRenderTargetView(renderTarget, color);
      context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      context->RSSetState(rasterizerState);
      context->RSSetViewports(1, &m_viewPort);
      context->OMSetRenderTargets(1, &renderTarget, nullptr);

      ID3D11Buffer *cameraBuffer =reinterpret_cast<ID3D11Buffer*>(scene.cameraBufferId);
      ID3D11Buffer *objectBuffer =reinterpret_cast<ID3D11Buffer*>(scene.objectBufferId);

      D3D11_MAPPED_SUBRESOURCE mapped ={};
      context->Map(
        cameraBuffer,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mapped);
      if(scene.cameraBufferUpdateFn)
        scene.cameraBufferUpdateFn(static_cast<CameraBuffer_t*>(mapped.pData));
      context->Unmap(cameraBuffer, 0);

      context->VSSetConstantBuffers(0, 1, &cameraBuffer);

      for(RenderObject const&object : scene.objects) {
        ID3D11Buffer       *vertexBuffer = reinterpret_cast<ID3D11Buffer*>(object.vertexBufferId);
        ID3D11Buffer       *indexBuffer  = reinterpret_cast<ID3D11Buffer*>(object.indexBufferId);
        ID3D11InputLayout  *inputLayout  = reinterpret_cast<ID3D11InputLayout*>(object.inputLayoutId);
        ID3D11VertexShader *vertexShader = reinterpret_cast<ID3D11VertexShader*>(object.vertexShaderId);
        ID3D11PixelShader  *pixelShader  = reinterpret_cast<ID3D11PixelShader*>(object.pixelShaderId);

        D3D11_BUFFER_DESC indexBufferDesc ={};
        indexBuffer->GetDesc(&indexBufferDesc);

        uint64_t indexBufferSize = indexBufferDesc.ByteWidth / sizeof(Mesh<XMVECTOR>::Index_t);

        context->Map(
          objectBuffer,
          0,
          D3D11_MAP_WRITE_DISCARD,
          0,
          &mapped);
        if(scene.objectBufferUpdateFn)
          scene.objectBufferUpdateFn(static_cast<ObjectBuffer_t*>(mapped.pData), object.objectId);
        context->Unmap(objectBuffer, 0);

        context->VSSetConstantBuffers(1, 1, &objectBuffer);

        std::size_t vertexSize = sizeof(Mesh<XMVECTOR>::Vertex_t);
        std::size_t offset     = 0;
        context->IASetInputLayout(inputLayout);
        context->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
        context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        context->VSSetShader(vertexShader, nullptr, 0);
        context->PSSetShader(pixelShader, nullptr, 0);

        context->DrawIndexed(indexBufferSize, 0, 0);
      }

      m_dx11Environment->getSwapChain()->Present(0, 0);
    }

  }
}