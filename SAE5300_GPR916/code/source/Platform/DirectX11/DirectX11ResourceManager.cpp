#include "Platform/DirectX11/DirectX11ResourceManager.h"

namespace SAE {
  namespace DirectX11 {


    template <>
    uint64_t
      DirectX11ResourceManager
      ::create<ID3D11RasterizerState, D3D11_RASTERIZER_DESC>
      (D3D11_RASTERIZER_DESC const&desc)
    {
      // ---------------------------------------------------------------
      // RCA -> Resource Creation Algorithm 
      // ---------------------------------------------------------------
      // 1. Declare unmanaged null initialized pointer of specific type
      ID3D11RasterizerState
        *pRasterizerStateUnmanaged = nullptr;

      // 2. Call ID3D11Device creation function and handle error
      HRESULT hres = m_device->CreateRasterizerState(&desc, &pRasterizerStateUnmanaged);
      HandleWINAPIError(hres, "Failed to create rasterizer state.");

      // 3. Create unique id by abusing reinterpret_cast to uint64_t
      uint64_t id = reinterpret_cast<uint64_t>(pRasterizerStateUnmanaged);

      // 4. Create managed directX shared pointer
      std::shared_ptr<ID3D11RasterizerState> 
        pRasterizerStateManaged 
        = MakeDirectX11ResourceSharedPointer(pRasterizerStateUnmanaged);

      // 5. Push to resource holder
      this->ResourceHolder<ID3D11RasterizerState>
        ::set(id, pRasterizerStateManaged);

      // 6. Return id
      return id;
    }

    template <>
    uint64_t
      DirectX11ResourceManager
      ::create<ID3D11Buffer,
               D3D11_BUFFER_DESC,
               D3D11_SUBRESOURCE_DATA>
      (D3D11_BUFFER_DESC      const&desc, 
       D3D11_SUBRESOURCE_DATA const&initialSubresource)
    {
      // ---------------------------------------------------------------
      // RCA -> Resource Creation Algorithm 
      // ---------------------------------------------------------------
      // 1. Declare unmanaged null initialized pointer of specific type
      ID3D11Buffer
        *pBufferUnmanaged = nullptr;
        
      // 1.a.) Check, whether valid subresource data was passed and if so,
      //       initialize the pointer withit for immediate initialization.
      D3D11_SUBRESOURCE_DATA
        const *subresource = nullptr;

      if((initialSubresource.pSysMem && initialSubresource.SysMemPitch))
        subresource = &initialSubresource;
      
      // 2. Call ID3D11Device creation function and handle error
      HRESULT hres = m_device->CreateBuffer(&desc, subresource, &pBufferUnmanaged);
      HandleWINAPIError(hres, "Failed to create buffer.");

      // 3. Create unique id by abusing reinterpret_cast to uint64_t
      uint64_t id = reinterpret_cast<uint64_t>(pBufferUnmanaged);

      // 4. Create managed directX shared pointer
      std::shared_ptr<ID3D11Buffer> 
        pBufferManaged 
        = MakeDirectX11ResourceSharedPointer(pBufferUnmanaged);

      // 5. Push to resource holder
      this->ResourceHolder<ID3D11Buffer>
        ::set(id, pBufferManaged);

      // 6. Return id
      return id;
    }

    template <>
    uint64_t
      DirectX11ResourceManager
      ::create<ID3D11InputLayout,
               std::vector<D3D11_INPUT_ELEMENT_DESC>,
               DirectX11ShaderBuffer>
      (std::vector<D3D11_INPUT_ELEMENT_DESC> const&layoutElements,
       DirectX11ShaderBuffer                 const&vertexShader)
    {
      // ---------------------------------------------------------------
      // RCA -> Resource Creation Algorithm 
      // ---------------------------------------------------------------
      // 1. Declare unmanaged null initialized pointer of specific type
      ID3D11InputLayout
        *pInputLayoutUnmanaged = nullptr;

      // 2. Call ID3D11Device creation function and handle error
      HRESULT hres = m_device->CreateInputLayout(layoutElements.data(), layoutElements.size(), vertexShader.pData, vertexShader.size, &pInputLayoutUnmanaged);
      HandleWINAPIError(hres, "Failed to create input layout.");

      // 3. Create unique id by abusing reinterpret_cast to uint64_t
      uint64_t id = reinterpret_cast<uint64_t>(pInputLayoutUnmanaged);

      // 4. Create managed directX shared pointer
      std::shared_ptr<ID3D11InputLayout> 
        pInputLayoutManaged 
        = MakeDirectX11ResourceSharedPointer(pInputLayoutUnmanaged);

      // 5. Push to resource holder
      this->ResourceHolder<ID3D11InputLayout>
        ::set(id, pInputLayoutManaged);

      // 6. Return id
      return id;
    }
    
    #define ShaderCreationFunction(Type, Function)                              \
              Type                                                              \
                *pShaderUnmanaged = nullptr;                                    \
                                                                                \
              HRESULT hres                                                      \
                = m_device->Function(                                           \
                  byteCodeBuffer.pData,                                         \
                  byteCodeBuffer.size,                                          \
                  nullptr,                                                      \
                  &pShaderUnmanaged);                                           \
              HandleWINAPIError(hres, "Failed to #Function.");                  \
                                                                                \
              uint64_t id = reinterpret_cast<uint64_t>(pShaderUnmanaged);       \
                                                                                \
              std::shared_ptr<Type>                                             \
                phaderManaged                                                   \
                = MakeDirectX11ResourceSharedPointer(pShaderUnmanaged);         \
                                                                                \
              this->ResourceHolder<Type>                                        \
                ::set(id, phaderManaged);                                       \
                                                                                \
              return id;

    template <>
    uint64_t
      DirectX11ResourceManager
      ::create<ID3D11VertexShader,
      DirectX11ShaderBuffer>
      (DirectX11ShaderBuffer const& byteCodeBuffer)
    {
      ShaderCreationFunction(ID3D11VertexShader, CreateVertexShader);
    }


    template <>
    uint64_t
      DirectX11ResourceManager
      ::create<ID3D11PixelShader,
      DirectX11ShaderBuffer>
      (DirectX11ShaderBuffer const& byteCodeBuffer)
    {
       ShaderCreationFunction(ID3D11PixelShader, CreatePixelShader);
    }

    template <>
    uint64_t DirectX11ResourceManager
      ::create<ID3D11Texture2D, std::string>
      (std::string const&filename)
    {
      return 0;
    }

    template <>
    uint64_t
      DirectX11ResourceManager
      ::create<ID3D11Texture3D>()
    {
      return 0;
    }

  }
}