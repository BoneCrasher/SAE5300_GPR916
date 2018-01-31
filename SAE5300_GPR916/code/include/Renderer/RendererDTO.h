#ifndef __SAE5300_GRP916__RENDERER_DTO_H__
#define __SAE5300_GRP916__RENDERER_DTO_H__

#include <vector>
#include <functional>

#include "Platform/DirectX11/DirectX11Common.h"

namespace SAE {
  namespace DTO {

    using namespace DirectX;

    struct CameraBuffer_t {
      XMMATRIX view;
      XMMATRIX projection;
    };

    struct ObjectBuffer_t {
      XMMATRIX world;
    };

    struct RenderObject {
      uint64_t
        objectId,
        vertexBufferId,
        indexBufferId,
        inputLayoutId,
        vertexShaderId,
        pixelShaderId;
    };

    struct RenderScene {
      uint64_t 
        cameraBufferId,
        objectBufferId;
      std::function<bool(CameraBuffer_t*)>
        cameraBufferUpdateFn;
      std::function<bool(ObjectBuffer_t*, uint64_t)>
        objectBufferUpdateFn;
      std::vector<RenderObject> 
        objects;
    };


  }
}

#endif