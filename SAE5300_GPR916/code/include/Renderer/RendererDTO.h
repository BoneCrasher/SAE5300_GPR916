#ifndef __SAE5300_GRP916__RENDERER_DTO_H__
#define __SAE5300_GRP916__RENDERER_DTO_H__

#include <vector>
#include <functional>
#include <stdint.h>

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

    struct LightBuffer_t {
      XMMATRIX lights[4];
      XMVECTOR cameraPosition;
      XMVECTOR cameraDirection;
      uint32_t displayMode;
      uint32_t lightIndex;
      uint32_t unused1;
      uint32_t unused2;
    };

    struct RenderObject {
      uint64_t
        objectId,
        vertexBufferId,
        indexBufferId,
        inputLayoutId,
        vertexShaderId,
        pixelShaderId,
        diffuseTextureSRVId,
        specularTextureSRVId,
        glossTextureSRVId;
    };

    struct RenderScene {
      uint64_t
        cameraBufferId,
        objectBufferId,
        lightBufferId;
      std::function<bool(CameraBuffer_t*)>
        cameraBufferUpdateFn;
      std::function<bool(ObjectBuffer_t*, uint64_t)>
        objectBufferUpdateFn;
      std::function<bool(LightBuffer_t*, uint64_t)>
        lightingBufferUpdateFn;
      std::vector<RenderObject> 
        objects;
      std::vector<uint64_t>
        lights;
    };


  }
}

#endif