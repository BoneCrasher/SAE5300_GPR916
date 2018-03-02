#ifndef __SAE5300_GPR916_ENGINE_H__
#define __SAE5300_GPR916_ENGINE_H__

#include <memory>

#include "Platform/Input.h"
#include "Platform/Timer.h"

#include "Platform/DirectX11/DirectX11Camera.h"
#include "Platform/DirectX11/DirectX11ResourceManager.h"
#include "Platform/DirectX11/DirectX11Transform.h"
#include "Platform/DirectX11/DirectX11Mesh.h"
#include "Platform/DirectX11/DirectX11Light.h"

#include "Renderer/RendererDTO.h"

struct Node {
  uint64_t objectId;

  std::vector<struct Node> children;
};

namespace SAE {
  namespace Engine {
    using namespace SAE::Input;
    using namespace SAE::Timing;
    using namespace SAE::Resources;
    using namespace SAE::DirectX11;
    using namespace SAE::DTO;

    using Camera = SAE::DirectX11::Camera;

    class Engine {
    public:
      bool initialize(std::shared_ptr<DirectX11ResourceManager> &resourceManager);
      bool update(
        const Timer::State &time,
        const InputState   &inputState);
      bool render(
        PassType    const&passType,
        RenderScene      &sceneHolder,
        uint64_t    const&cubeIndex      = 0,
        uint64_t    const&shadowMapIndex = 0);
      bool deinitialize();

    private:
      uint64_t
        m_cameraBuffer,
        m_objectBuffer,
        m_lightBuffer,
        m_otherBuffer,
        m_shadowMapLightBufferId;

      Camera m_defaultCamera;

      Node   m_hierarchyRoot;
      std::map<uint64_t, SAE::DirectX11::DirectX11MeshPtr> m_meshes;
      std::map<uint64_t, SAE::DirectX11::DX11TransformPtr> m_transforms;
      std::map<uint64_t, Light> m_lights;

      uint64_t
        m_diffuseTextureId,
        m_diffuseTextureSRVId,
        m_specularTextureId,
        m_specularTextureSRVId,
        m_glossTextureId,
        m_glossTextureSRVId,
        m_normalTextureId,
        m_normalTextureSRVId,
        m_shadowMapTextureId,
        m_shadowMapSRVId,
        m_shadowMapDSVId[24];

      uint32_t m_displayMode;
    };

  }
}

#endif