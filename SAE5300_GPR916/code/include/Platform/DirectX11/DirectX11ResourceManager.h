#ifndef __SAE5300_GPR916_DIRECTX11_RESOURCEMANAGER_H__
#define __SAE5300_GPR916_DIRECTX11_RESOURCEMANAGER_H__

#include <stdint.h>
#include <string>
#include <map>
#include <memory>

#include "Platform/ResourceManager.h"

#include "DirectX11Common.h"
#include "DirectX11Environment.h"

namespace SAE {
  namespace DirectX11 {
    using namespace SAE::Error;
    using namespace SAE::Resources;

#define SupportedTypes     \
    ID3D11RasterizerState, \
    ID3D11Buffer,          \
    ID3D11InputLayout,     \
    ID3D11VertexShader,    \
    ID3D11PixelShader,     \
    ID3D11Texture2D,       \
    ID3D11Texture3D

    class DirectX11ResourceManager 
      : public ResourceManager<SupportedTypes>
    {
    public:
      DirectX11ResourceManager(ID3D11DevicePtr device)
        : m_device(device)
      {}

    template <typename T, typename... Args>
    uint64_t
      create(Args const&... arguments)
    {
      throw std::exception("Type not supported!");
    }

    private:
      ID3D11DevicePtr m_device;
    };

    template <>
    uint64_t
      DirectX11ResourceManager
      ::create<ID3D11RasterizerState,
               D3D11_RASTERIZER_DESC>
      (D3D11_RASTERIZER_DESC const&desc);

    template <>
    uint64_t
      DirectX11ResourceManager
      ::create<ID3D11Buffer,
               D3D11_BUFFER_DESC,
               D3D11_SUBRESOURCE_DATA>
      (D3D11_BUFFER_DESC      const&desc, 
       D3D11_SUBRESOURCE_DATA const&initialSubresources);

    struct DirectX11ShaderBuffer {
      void        *pData;
      std::size_t  size;
    };

    template <>
    uint64_t
      DirectX11ResourceManager
      ::create<ID3D11InputLayout,
               std::vector<D3D11_INPUT_ELEMENT_DESC>, 
               DirectX11ShaderBuffer>
      (std::vector<D3D11_INPUT_ELEMENT_DESC> const&layoutElements,
       DirectX11ShaderBuffer                 const&vertexShader);

    template <>
    uint64_t
      DirectX11ResourceManager
      ::create<ID3D11VertexShader,
               DirectX11ShaderBuffer>
      (DirectX11ShaderBuffer const& byteCodeBuffer);

    template <>
    uint64_t
      DirectX11ResourceManager
      ::create<ID3D11PixelShader,
               DirectX11ShaderBuffer>
      (DirectX11ShaderBuffer const& byteCodeBuffer);

    template <>
    uint64_t
      DirectX11ResourceManager
      ::create<ID3D11Texture2D, 
               std::string>
      (std::string const&filename);

    template <>
    uint64_t
      DirectX11ResourceManager
      ::create<ID3D11Texture3D>();
  }
}

#endif 