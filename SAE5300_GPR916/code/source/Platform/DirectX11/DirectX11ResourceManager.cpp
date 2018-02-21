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

      if((initialSubresource.pSysMem))
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
    uint64_t
      DirectX11ResourceManager
      ::create<ID3D11Texture3D>()
    {
      return 0;
    }

    template <>
    uint64_t
      DirectX11ResourceManager
      ::create<ID3D11DepthStencilState,
      D3D11_DEPTH_STENCIL_DESC>
      (D3D11_DEPTH_STENCIL_DESC const&desc)
    {
      // ---------------------------------------------------------------
      // RCA -> Resource Creation Algorithm 
      // ---------------------------------------------------------------
      // 1. Declare unmanaged null initialized pointer of specific type
      ID3D11DepthStencilState
        *pDSSUnmanaged = nullptr;

      // 2. Call ID3D11Device creation function and handle error
      HRESULT hres = m_device->CreateDepthStencilState(&desc, &pDSSUnmanaged);
      HandleWINAPIError(hres, "Failed to create input layout.");

      // 3. Create unique id by abusing reinterpret_cast to uint64_t
      uint64_t id = reinterpret_cast<uint64_t>(pDSSUnmanaged);

      // 4. Create managed directX shared pointer
      std::shared_ptr<ID3D11DepthStencilState>
        pDSSManaged
        = MakeDirectX11ResourceSharedPointer(pDSSUnmanaged);

      // 5. Push to resource holder
      this->ResourceHolder<ID3D11DepthStencilState>
        ::set(id, pDSSManaged);

      // 6. Return id
      return id;
    }

    template <>
    uint64_t
      DirectX11ResourceManager
      ::create<ID3D11DepthStencilView,
               D3D11_DEPTH_STENCIL_VIEW_DESC,
               ID3D11Texture2D*,
               D3D11_SUBRESOURCE_DATA>
      (D3D11_DEPTH_STENCIL_VIEW_DESC  const&desc,
       ID3D11Texture2D               *const&texture,
       D3D11_SUBRESOURCE_DATA         const&initialSubresources)
    {

      // ---------------------------------------------------------------
      // RCA -> Resource Creation Algorithm 
      // ---------------------------------------------------------------
      // 1. Declare unmanaged null initialized pointer of specific type
      ID3D11DepthStencilView
        *pDSVUnmanaged = nullptr;

      // 2. Call ID3D11Device creation function and handle error
      HRESULT hres = m_device->CreateDepthStencilView(texture, &desc, &pDSVUnmanaged);
      HandleWINAPIError(hres, "Failed to create depth stencil view.");

      // 3. Create unique id by abusing reinterpret_cast to uint64_t
      uint64_t id = reinterpret_cast<uint64_t>(pDSVUnmanaged);

      // 4. Create managed directX shared pointer
      std::shared_ptr<ID3D11DepthStencilView>
        pDSVManaged
        = MakeDirectX11ResourceSharedPointer(pDSVUnmanaged);

      // 5. Push to resource holder
      this->ResourceHolder<ID3D11DepthStencilView>
        ::set(id, pDSVManaged);

      // 6. Return id
      return id;
    }

    template <>
    uint64_t
      DirectX11ResourceManager
      ::create < ID3D11Texture2D,
                 D3D11_TEXTURE2D_DESC,
      std::vector<D3D11_SUBRESOURCE_DATA>>
      (D3D11_TEXTURE2D_DESC                const&desc,
       std::vector<D3D11_SUBRESOURCE_DATA> const&initialData)
    {

      // ---------------------------------------------------------------
      // RCA -> Resource Creation Algorithm 
      // ---------------------------------------------------------------
      // 1. Declare unmanaged null initialized pointer of specific type
      ID3D11Texture2D
        *pTexUnmanaged = nullptr;

      // 2. Call ID3D11Device creation function and handle error
      HRESULT hres = m_device->CreateTexture2D(&desc, initialData.data(), &pTexUnmanaged);
      HandleWINAPIError(hres, "Failed to create texture 2d.");

      // 3. Create unique id by abusing reinterpret_cast to uint64_t
      uint64_t id = reinterpret_cast<uint64_t>(pTexUnmanaged);

      // 4. Create managed directX shared pointer
      std::shared_ptr<ID3D11Texture2D>
        pTexManaged
        = MakeDirectX11ResourceSharedPointer(pTexUnmanaged);

      // 5. Push to resource holder
      this->ResourceHolder<ID3D11Texture2D>
        ::set(id, pTexManaged);

      // 6. Return id
      return id;
    }

    template <>
    uint64_t
      DirectX11ResourceManager
      ::create<ID3D11ShaderResourceView,
      D3D11_SHADER_RESOURCE_VIEW_DESC,
      ID3D11Texture2D*>
      (D3D11_SHADER_RESOURCE_VIEW_DESC  const&desc, 
       ID3D11Texture2D               *const&texture)
    {

      // ---------------------------------------------------------------
      // RCA -> Resource Creation Algorithm 
      // ---------------------------------------------------------------
      // 1. Declare unmanaged null initialized pointer of specific type
      ID3D11ShaderResourceView
        *pSRVUnmanaged = nullptr;

      // 2. Call ID3D11Device creation function and handle error
      HRESULT hres = m_device->CreateShaderResourceView(texture, &desc, &pSRVUnmanaged);
      HandleWINAPIError(hres, "Failed to create shader resource view.");

      // 3. Create unique id by abusing reinterpret_cast to uint64_t
      uint64_t id = reinterpret_cast<uint64_t>(pSRVUnmanaged);

      // 4. Create managed directX shared pointer
      std::shared_ptr<ID3D11ShaderResourceView>
        pSRVManaged
        = MakeDirectX11ResourceSharedPointer(pSRVUnmanaged);

      // 5. Push to resource holder
      this->ResourceHolder<ID3D11ShaderResourceView>
        ::set(id, pSRVManaged);

      // 6. Return id
      return id;
    }


    template <>
    uint64_t 
      DirectX11ResourceManager
      ::create<ID3D11SamplerState,
               D3D11_SAMPLER_DESC>
      (D3D11_SAMPLER_DESC const&desc)
    {

      // ---------------------------------------------------------------
      // RCA -> Resource Creation Algorithm 
      // ---------------------------------------------------------------
      // 1. Declare unmanaged null initialized pointer of specific type
      ID3D11SamplerState
        *pSSUnmanaged = nullptr;

      // 2. Call ID3D11Device creation function and handle error
      HRESULT hres = m_device->CreateSamplerState(&desc, &pSSUnmanaged);
      HandleWINAPIError(hres, "Failed to create shader resource view.");

      // 3. Create unique id by abusing reinterpret_cast to uint64_t
      uint64_t id = reinterpret_cast<uint64_t>(pSSUnmanaged);

      // 4. Create managed directX shared pointer
      std::shared_ptr<ID3D11SamplerState>
        pSSManaged
        = MakeDirectX11ResourceSharedPointer(pSSUnmanaged);

      // 5. Push to resource holder
      this->ResourceHolder<ID3D11SamplerState>
        ::set(id, pSSManaged);

      // 6. Return id
      return id;
    }
  }
}