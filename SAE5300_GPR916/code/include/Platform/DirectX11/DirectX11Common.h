#ifndef __SAE5300_GPR916_DIRECTX11COMMON_H__
#define __SAE5300_GPR916_DIRECTX11COMMON_H__

#include <memory>

#define _USE_MATH_DEFINES_ // M_PI, etc...
#include <cmath>
#include <math.h>

#define _XM_NO_INTRINSICS_
#include <DirectXMath.h>

#include <dxgi.h>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dcommon.h>

#ifndef __DX11LINK__
#define __DX11LINK__
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#endif

namespace SAE {
  namespace DirectX11 {

    template <typename T>
    struct DirectX11ResourceDeleter {
      void operator()(T* p) {
        if(p)
          p->Release();
      }
    };

#define DeclareDirectX11ResourceSharedPointer(Type) \
    using Type##Ptr = std::shared_ptr<Type>;

    template <typename T> 
    static std::shared_ptr<T> 
      MakeDirectX11ResourceSharedPointer(T *pIn)
    {
      return std::shared_ptr<T>(pIn, DirectX11ResourceDeleter<T>());
    }

    DeclareDirectX11ResourceSharedPointer(IDXGIFactory);
    DeclareDirectX11ResourceSharedPointer(IDXGIAdapter);
    DeclareDirectX11ResourceSharedPointer(IDXGIOutput);

    DeclareDirectX11ResourceSharedPointer(IDXGISwapChain);
    DeclareDirectX11ResourceSharedPointer(ID3D11Device);
    DeclareDirectX11ResourceSharedPointer(ID3D11DeviceContext);

    DeclareDirectX11ResourceSharedPointer(ID3D11RenderTargetView);
    
  }
}

#endif