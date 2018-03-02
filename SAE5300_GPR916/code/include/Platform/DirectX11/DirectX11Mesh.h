#ifndef __SAE5300_GPR916_DIRECTX_MESH_H__
#define __SAE5300_GPR916_DIRECTX_MESH_H__

#include "Engine/Mesh.h"

#include "Platform/DirectX11/DirectX11Common.h"
#include "Platform/DirectX11/DirectX11Environment.h"

#include "Platform/DirectX11/DirectX11ResourceManager.h"

namespace SAE {
  namespace DirectX11 {
    using namespace DirectX;

    class DirectX11Mesh
      : public SAE::Engine::Mesh<XMVECTOR>
    {
    public:      
      static  
        std::shared_ptr<DirectX11Mesh> 
        loadTriangle(
          std::shared_ptr<DirectX11ResourceManager>      &resourceManager,
          std::string                               const&filename);
      static
        std::shared_ptr<DirectX11Mesh> 
        loadFromFile(
          std::shared_ptr<DirectX11ResourceManager>      &resourceManager,
          std::string                               const&filename);
    
      uint64_t const& vertexBufferHandle() const { return m_vertexBufferHandle; }
      uint64_t const& indexBufferHandle()  const { return m_indexBufferHandle;  }
      uint64_t const& inputLayoutHandle()  const { return m_inputLayoutHandle;  }
      uint64_t const& vertexShaderHandle() const { return m_vertexShaderHandle; }
      uint64_t const& pixelShaderHandle()  const { return m_pixelShaderHandle;  }
      uint64_t const& shadowMapInputLayoutHandle()  const { return m_shadowMapInputLayoutHandle; }
      uint64_t const& shadowMapVertexShaderHandle() const { return m_shadowMapVertexShaderHandle; }
      uint64_t const& shadowMapPixelShaderHandle()  const { return m_shadowMapPixelShaderHandle; }

    private:
      DirectX11Mesh() = default;

      inline void setVertexBuffer(uint64_t const&handle) { m_vertexBufferHandle = handle; }
      inline void setIndexBuffer(uint64_t const&handle)  { m_indexBufferHandle  = handle; }
      inline void setVertexShader(uint64_t const&handle) { m_vertexShaderHandle = handle; }
      inline void setPixelShader(uint64_t const&handle)  { m_pixelShaderHandle = handle;  }
      inline void setInputLayout(uint64_t const&handle)  { m_inputLayoutHandle = handle;  }
      inline void setShadowMapVertexShader(uint64_t const&handle) { m_shadowMapVertexShaderHandle = handle; }
      inline void setShadowMapPixelShader(uint64_t const&handle)  { m_shadowMapPixelShaderHandle  = handle;  }
      inline void setShadowMapInputLayout(uint64_t const&handle)  { m_shadowMapInputLayoutHandle  = handle;  }

      uint64_t
        m_vertexBufferHandle,
        m_indexBufferHandle,
        m_inputLayoutHandle,
        m_vertexShaderHandle,
        m_pixelShaderHandle,
        m_shadowMapVertexShaderHandle,
        m_shadowMapPixelShaderHandle,
        m_shadowMapInputLayoutHandle;

    };
    using DirectX11MeshPtr = std::shared_ptr<DirectX11Mesh>;
  }
}

#endif