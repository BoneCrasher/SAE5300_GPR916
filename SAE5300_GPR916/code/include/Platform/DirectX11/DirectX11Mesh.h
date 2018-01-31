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
        loadFromFile(
          std::shared_ptr<DirectX11ResourceManager>      &resourceManager,
          std::string                               const&filename);
    
      uint64_t const& vertexBufferHandle() const { return m_vertexBufferHandle; }
      uint64_t const& indexBufferHandle()  const { return m_indexBufferHandle;  }
      uint64_t const& inputLayoutHandle()  const { return m_inputLayoutHandle;  }
      uint64_t const& vertexShaderHandle() const { return m_vertexShaderHandle; }
      uint64_t const& pixelShaderHandle()  const { return m_pixelShaderHandle;  }

    private:
      DirectX11Mesh() = default;

      inline void setVertexBuffer(uint64_t const&handle) { m_vertexBufferHandle = handle; }
      inline void setIndexBuffer(uint64_t const&handle)  { m_indexBufferHandle  = handle; }

      uint64_t
        m_vertexBufferHandle,
        m_indexBufferHandle,
        m_inputLayoutHandle,
        m_vertexShaderHandle,
        m_pixelShaderHandle;
    };
    using DirectX11MeshPtr = std::shared_ptr<DirectX11Mesh>;
  }
}

#endif