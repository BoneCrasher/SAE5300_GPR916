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
      XMVECTOR cameraPosition;
      XMVECTOR cameraDirection;
    };

    struct ObjectBuffer_t {
      XMMATRIX world;
      XMMATRIX invTransposeWorld;
    };

    struct LightInfo_t {
      XMMATRIX view[6];
      XMMATRIX projection;
      XMVECTOR position;
      XMVECTOR direction;
      XMVECTOR color;
      uint32_t type;
      uint32_t lightViewIndex;
      uint32_t unused1;
      uint32_t unused2;
      float    intensity;
      float    distance;
      float    hotSpotAngle;
      float    falloffAngle;
    };

    struct LightBuffer_t {
      LightInfo_t lights[4];
      uint32_t    lightIndex;
      uint32_t    unused0;
      uint32_t    unused1;
      uint32_t    unused2;
    };

    struct OtherBuffer_t {
      uint32_t displayMode;
      uint32_t unused0;
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
        glossTextureSRVId,
        normalTextureSRVId;
    };

    struct RenderScene {
      uint64_t
        cameraBufferId,
        objectBufferId,
        lightBufferId,
        otherBufferId,
        shadowMapTextureSRVId;
      uint64_t
        renderTargetId;
      std::function<bool(CameraBuffer_t*)>
        cameraBufferUpdateFn;
      std::function<bool(ObjectBuffer_t*, uint64_t)>
        objectBufferUpdateFn;
      std::function<bool(LightBuffer_t*, uint64_t, uint64_t)>
        lightingBufferUpdateFn;
      std::function<bool(OtherBuffer_t*)>
        otherBufferUpdateFn;
      std::vector<RenderObject> 
        objects;
      std::vector<uint64_t>
        lights;
    };
    
    enum class PassType {
      Main      = 1,
      ShadowMap = 2
    };
  }
}

#endif