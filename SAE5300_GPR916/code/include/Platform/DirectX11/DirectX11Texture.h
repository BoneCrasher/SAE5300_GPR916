#ifndef __SAE5300_GPR916_DX11TEXTURE_H__
#define __SAE5300_GPR916_DX11TEXTURE_H__

#include <vector>
#include <string>

#include "Engine/Texture.h"
#include "Platform/DirectX11/DirectX11ResourceManager.h"

namespace SAE {
  namespace DirectX11 {
    using namespace SAE::DirectX11;
    using namespace SAE::Texture;
    using namespace SAE::Resources;

    bool LoadTextureArrayFromFiles(
      std::shared_ptr<DirectX11ResourceManager> &resourceManager,
      std::vector<std::string>             const&filenames,
      uint64_t                                  &outTextureHandle,
      uint64_t                                  &outTexSRVHandle)
    {
      try {
        Texture2DDescriptor outImage={};
        SAELoadTextureArrayFromFiles(filenames, outImage);
        outImage.mipLevels = 1;

        D3D11_TEXTURE2D_DESC desc ={};
        desc.Width              = outImage.width;
        desc.Height             = outImage.height;
        desc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.MipLevels          = outImage.mipLevels;
        desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
        desc.Usage              = D3D11_USAGE_DEFAULT;
        desc.CPUAccessFlags     = 0;
        desc.MiscFlags          = 0;
        desc.SampleDesc.Quality = 0;
        desc.SampleDesc.Count   = 1;
        desc.ArraySize          = outImage.depth;

        if(desc.ArraySize > 1)
        {
          desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
        }

        std::vector<D3D11_SUBRESOURCE_DATA> pData={};
        pData.resize(desc.ArraySize);
        for(unsigned int k=0; k < desc.ArraySize; ++k)
        {
          pData[k].pSysMem          = &outImage.inData[k][0];
          pData[k].SysMemPitch      = 4 * outImage.width * sizeof(Byte);
          pData[k].SysMemSlicePitch = 0;
        }

        outTextureHandle = resourceManager->create<ID3D11Texture2D>(desc, pData);
        ID3D11Texture2D *pTexture =  reinterpret_cast<ID3D11Texture2D*>(outTextureHandle);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc={};
        if(desc.ArraySize > 1) {
          srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
          srvDesc.Format        = desc.Format;
          srvDesc.Texture2DArray.ArraySize       = desc.ArraySize;
          srvDesc.Texture2DArray.FirstArraySlice = 0;
          srvDesc.Texture2DArray.MipLevels       = desc.MipLevels;
          srvDesc.Texture2DArray.MostDetailedMip = 0;
        }
        else {
          srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
          srvDesc.Format        = desc.Format;
          srvDesc.Texture2D.MipLevels       = desc.MipLevels;
          srvDesc.Texture2D.MostDetailedMip = 0;
        }
        outTexSRVHandle  = resourceManager->create<ID3D11ShaderResourceView>(srvDesc, pTexture);

        outImage.freeData();
        return true;
      } catch(...) {
        return false;
      }
    }

    bool LoadTextureFromFile(
      std::shared_ptr<DirectX11ResourceManager> &resourceManager,
      std::string                          const&filename,
      uint64_t                                  &outTextureHandle,
      uint64_t                                  &outTexSRVHandle)
    {
      return LoadTextureArrayFromFiles(resourceManager, { filename }, outTextureHandle, outTexSRVHandle);
    }

  }
}

  #endif