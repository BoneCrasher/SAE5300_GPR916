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
      ID3D11Texture2D *pTextureUnmanaged = nullptr;
      HRESULT hres = m_dx11Environment->getSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), ((void**)&pTextureUnmanaged));
      HandleWINAPIError(hres, "Failed to get underlying swapchain backbuffer texture.");

      D3D11_TEXTURE2D_DESC dsvTexDesc ={};
      pTextureUnmanaged->GetDesc(&dsvTexDesc);
      dsvTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
      dsvTexDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;

      std::vector<D3D11_SUBRESOURCE_DATA> emptyRenderTargetData{};

      m_dsvTexHandle = m_resourceManager->create<ID3D11Texture2D>(dsvTexDesc, emptyRenderTargetData);
      std::shared_ptr<ID3D11Texture2D> pDsvTex = nullptr;
      pDsvTex = m_resourceManager->get<ID3D11Texture2D>(m_dsvTexHandle);

      D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc ={};
      dsvDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
      dsvDesc.Texture2D.MipSlice = 0;
      dsvDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;

      D3D11_SUBRESOURCE_DATA data{};
      m_dsvViewHandle = m_resourceManager->create<ID3D11DepthStencilView>(dsvDesc, pDsvTex.get(), data);

      D3D11_DEPTH_STENCIL_DESC dssDesc ={};
      dssDesc.DepthEnable                  = true;
      dssDesc.DepthFunc                    = D3D11_COMPARISON_LESS;
      dssDesc.DepthWriteMask               = D3D11_DEPTH_WRITE_MASK_ALL;
      dssDesc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
      dssDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
      dssDesc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
      dssDesc.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;
      dssDesc.BackFace.StencilFailOp       = D3D11_STENCIL_OP_KEEP;
      dssDesc.BackFace.StencilDepthFailOp  = D3D11_STENCIL_OP_DECR;
      dssDesc.BackFace.StencilPassOp       = D3D11_STENCIL_OP_KEEP;
      dssDesc.BackFace.StencilFunc         = D3D11_COMPARISON_ALWAYS;
      dssDesc.StencilEnable  = false;

      m_dssHandle = m_resourceManager->create<ID3D11DepthStencilState>(dssDesc);

      D3D11_RASTERIZER_DESC desc={};
      desc.FillMode              = D3D11_FILL_SOLID;
      desc.CullMode              = D3D11_CULL_BACK;
      desc.FrontCounterClockwise = true;
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

      D3D11_SAMPLER_DESC ssDesc={};
      ssDesc.Filter   = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
      ssDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
      ssDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
      ssDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
      m_defaultSamplerStateId = m_resourceManager->create<ID3D11SamplerState>(ssDesc);

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

      ID3D11RenderTargetView  *renderTarget      = m_dx11Environment->getMainRenderTarget().get();
      ID3D11DepthStencilView  *depthStencilView  = reinterpret_cast<ID3D11DepthStencilView*>(m_dsvViewHandle);
      ID3D11DepthStencilState *depthStencilState = reinterpret_cast<ID3D11DepthStencilState*>(m_dssHandle);
      ID3D11RasterizerState   *rasterizerState   = reinterpret_cast<ID3D11RasterizerState*>(m_rasterizerStateId);
      ID3D11SamplerState      *defaultSampler    = reinterpret_cast<ID3D11SamplerState*>(m_defaultSamplerStateId);

      FLOAT color[4] ={ 0.5f, 0.5f, 0.5f, 1.0f };
      context->ClearRenderTargetView(renderTarget, color);
      context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0xFF);
      context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      context->RSSetState(rasterizerState);
      context->RSSetViewports(1, &m_viewPort);
      context->OMSetRenderTargets(1, &renderTarget, depthStencilView);
      context->OMSetDepthStencilState(depthStencilState, 0);

      ID3D11Buffer *cameraBuffer =reinterpret_cast<ID3D11Buffer*>(scene.cameraBufferId);
      ID3D11Buffer *objectBuffer =reinterpret_cast<ID3D11Buffer*>(scene.objectBufferId);
      ID3D11Buffer *lightBuffer  =reinterpret_cast<ID3D11Buffer*>(scene.lightBufferId);
      ID3D11Buffer *otherBuffer  =reinterpret_cast<ID3D11Buffer*>(scene.otherBufferId);

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
      context->PSSetConstantBuffers(0, 1, &cameraBuffer);

      // Lights
      for(uint64_t const&lightId : scene.lights) {
        context->Map(
          lightBuffer,
          0,
          D3D11_MAP_WRITE_DISCARD,
          0,
          &mapped);
        if(scene.lightingBufferUpdateFn)
          scene.lightingBufferUpdateFn(static_cast<LightBuffer_t*>(mapped.pData), lightId);
        context->Unmap(lightBuffer, 0);
      }
      
      context->PSSetConstantBuffers(1, 1, &lightBuffer);

      // Other
      mapped ={};
      context->Map(
        otherBuffer,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mapped);
      if(scene.otherBufferUpdateFn)
        scene.otherBufferUpdateFn(static_cast<OtherBuffer_t*>(mapped.pData));
      context->Unmap(otherBuffer, 0);

      // context->PSSetConstantBuffers(2, 1, &otherBuffer);

      // Objects
      for(RenderObject const&object : scene.objects) {
        ID3D11Buffer       *vertexBuffer = reinterpret_cast<ID3D11Buffer*>(object.vertexBufferId);
        ID3D11Buffer       *indexBuffer  = reinterpret_cast<ID3D11Buffer*>(object.indexBufferId);
        ID3D11InputLayout  *inputLayout  = reinterpret_cast<ID3D11InputLayout*>(object.inputLayoutId);
        ID3D11VertexShader *vertexShader = reinterpret_cast<ID3D11VertexShader*>(object.vertexShaderId);
        ID3D11PixelShader  *pixelShader  = reinterpret_cast<ID3D11PixelShader*>(object.pixelShaderId);

        ID3D11ShaderResourceView *diffuseTexture  = reinterpret_cast<ID3D11ShaderResourceView*>(object.diffuseTextureSRVId);
        ID3D11ShaderResourceView *specularTexture = reinterpret_cast<ID3D11ShaderResourceView*>(object.specularTextureSRVId);
        ID3D11ShaderResourceView *glossTexture    = reinterpret_cast<ID3D11ShaderResourceView*>(object.glossTextureSRVId);
        ID3D11ShaderResourceView *normalTexture   = reinterpret_cast<ID3D11ShaderResourceView*>(object.normalTextureSRVId);


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

        std::vector<ID3D11ShaderResourceView*> psSRV
          ={ diffuseTexture, specularTexture, glossTexture, normalTexture };

        std::size_t vertexSize = sizeof(Mesh<XMVECTOR>::Vertex_t);
        std::size_t offset     = 0;
        context->IASetInputLayout(inputLayout);
        context->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
        context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        context->VSSetShader(vertexShader, nullptr, 0);
        context->PSSetShader(pixelShader, nullptr, 0);
        context->PSSetShaderResources(0, psSRV.size(), psSRV.data());
        context->PSSetSamplers(0, 1, &defaultSampler);

        context->DrawIndexed(indexBufferSize, 0, 0);
      }

      m_dx11Environment->getSwapChain()->Present(0, 0);
    }

  }
}