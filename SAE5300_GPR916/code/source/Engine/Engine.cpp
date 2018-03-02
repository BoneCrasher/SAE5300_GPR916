#include <map>
#include <functional>

#include "Logging/Logging.h"

#include "Engine/Engine.h"

#include "Platform/DirectX11/DirectX11Texture.h"

namespace SAE {
  namespace Engine {
    using namespace SAE::Log;

    bool Engine
      ::initialize(std::shared_ptr<DirectX11ResourceManager> &resourceManager)
    {
      Camera::Properties cameraProperties ={};
      cameraProperties.aspectRatio  = 1920.0f / 1080.0f;
      cameraProperties.fieldOfViewY = M_PI / 4.0f;
      cameraProperties.nearPlane    = 0.05;
      cameraProperties.farPlane     = 1000.0;

      m_defaultCamera = Camera(cameraProperties);
      m_defaultCamera.initialize();

      D3D11_BUFFER_DESC
        cameraBufferDesc ={};
      cameraBufferDesc.ByteWidth           = sizeof(CameraBuffer_t);
      cameraBufferDesc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
      cameraBufferDesc.Usage               = D3D11_USAGE_DYNAMIC;
      cameraBufferDesc.MiscFlags           = 0;
      cameraBufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
      cameraBufferDesc.StructureByteStride = 0;

      D3D11_SUBRESOURCE_DATA
        cameraInitialData={};

      m_cameraBuffer
        = resourceManager->create<ID3D11Buffer>(cameraBufferDesc, cameraInitialData);

      D3D11_BUFFER_DESC
        lightBufferDesc ={};
      lightBufferDesc.ByteWidth           = sizeof(LightBuffer_t);
      lightBufferDesc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
      lightBufferDesc.Usage               = D3D11_USAGE_DYNAMIC;
      lightBufferDesc.MiscFlags           = 0;
      lightBufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
      lightBufferDesc.StructureByteStride = 0;

      D3D11_SUBRESOURCE_DATA
        lightInitialData={};

      m_lightBuffer
        = resourceManager->create<ID3D11Buffer>(lightBufferDesc, lightInitialData);

      Light::Properties props={};
      props.intensity = 4.0f;
      props.type      = Light::Type::Point;
      props.color     ={ 1.0f, 1.0f, 1.0f, 1.0f };
      props.specificProperties.point.distance = 10.0f;
      Light light(props);
      light.transform().setTranslation(0.0f, 1.0f, 20.0f);
      light.transform().setScale(0.01f, 0.01f, 0.01f);
      m_lights[1] = light;

      props={};
      props.intensity = 10.0f;
      props.type      = Light::Type::Point;
      props.color     ={ 0.0f, 1.0f, 0.0f, 1.0f };
      props.specificProperties.point.distance = 10.0f;
      light = Light(props);
      light.transform().setTranslation(-1.0f, 3.0f, 17.0f);
      light.transform().setScale(0.01f, 0.01f, 0.01f);
      m_lights[2] = light;

      props={};
      props.intensity = 10.0f;
      props.type      = Light::Type::Point;
      props.color     ={ 1.0f, 0.0f, 0.0f, 1.0f };
      props.specificProperties.point.distance = 10.0f;
      light = Light(props);
      light.transform().setTranslation(1.0f, 3.0f, 17.0f);
      light.transform().setScale(0.01f, 0.01f, 0.01f);
      m_lights[3] = light;

      props={};
      props.intensity = 10.0f;
      props.type      = Light::Type::Point;
      props.color     ={ 0.0f, 0.0f, 1.0f, 1.0f };
      props.specificProperties.point.distance = 10.0f;
      light = Light(props);
      light.transform().setTranslation(0.0f, 3.0f, 23.0f);
      light.transform().setScale(0.01f, 0.01f, 0.01f);
      m_lights[4] = light;

      D3D11_BUFFER_DESC
        objectBufferDesc ={};
      objectBufferDesc.ByteWidth           = sizeof(ObjectBuffer_t);
      objectBufferDesc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
      objectBufferDesc.Usage               = D3D11_USAGE_DYNAMIC;
      objectBufferDesc.MiscFlags           = 0;
      objectBufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
      objectBufferDesc.StructureByteStride = 0;

      D3D11_SUBRESOURCE_DATA
        objectInitialData={};

      m_objectBuffer
        = resourceManager->create<ID3D11Buffer>(objectBufferDesc, objectInitialData);

      D3D11_BUFFER_DESC
        otherBufferDesc ={};
      otherBufferDesc.ByteWidth           = sizeof(OtherBuffer_t);
      otherBufferDesc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
      otherBufferDesc.Usage               = D3D11_USAGE_DYNAMIC;
      otherBufferDesc.MiscFlags           = 0;
      otherBufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
      otherBufferDesc.StructureByteStride = 0;

      D3D11_SUBRESOURCE_DATA
        otherInitialData={};

      m_otherBuffer
        = resourceManager->create<ID3D11Buffer>(otherBufferDesc, otherInitialData);

      std::shared_ptr<SAE::DirectX11::DirectX11Mesh>
        lightSphereMesh = nullptr;
      lightSphereMesh = SAE::DirectX11::DirectX11Mesh::loadFromFile(resourceManager, "resources/meshes/regular_sphere.obj");
      uint64_t lightSphereId[4];
      // LOAD MESHES HERE!!!
      for(uint32_t k=0; k < 4; ++k) {
        lightSphereId[k] = 1 + k;

        DX11TransformPtr lightSphereTransform = DX11TransformPtr(new DX11Transform());
        lightSphereTransform->setTranslation(-2.0f + (k * 2.0f), 1.0f, 20.0f);
        lightSphereTransform->setScale(0.01f, 0.01f, 0.01f);

        m_transforms[lightSphereId[k]] = lightSphereTransform;
        m_meshes[lightSphereId[k]]     = lightSphereMesh;
      }

      uint64_t planeId = 5;
      std::shared_ptr<SAE::DirectX11::DirectX11Mesh>
        planeMesh = nullptr;

      // cubeMesh = SAE::DirectX11::DirectX11Mesh::loadFromFile(resourceManager, "resources/meshes/Sci-Fi-Floor-1-OBJ.obj");
      planeMesh = SAE::DirectX11::DirectX11Mesh::loadFromFile(resourceManager, "resources/meshes/fourQuadPlane.obj");

      float planeScale = 10.0f;
      DX11TransformPtr planeTransform = DX11TransformPtr(new DX11Transform());
      planeTransform->setTranslationZ(20);
      planeTransform->setScale(planeScale, 1.0f, planeScale);

      m_transforms[planeId] = planeTransform;
      m_meshes[planeId]     = planeMesh;

      m_lights[1].transform().setTranslation(planeTransform->getTranslation()); // Place light in the middle of the plane and shift up
      m_lights[1].transform().translateVerticalBy(1); // Place light in the middle of the plane and shift up

      uint64_t shadowSphereId = 6;
      std::shared_ptr<SAE::DirectX11::DirectX11Mesh>
        shadowSphereMesh = nullptr;

      shadowSphereMesh = SAE::DirectX11::DirectX11Mesh::loadFromFile(resourceManager, "resources/meshes/regular_sphere.obj");

      DX11TransformPtr shadowSphereTransform = DX11TransformPtr(new DX11Transform());
      shadowSphereTransform->setTranslation(0.0f, 1.0f, 20.0f);
      shadowSphereTransform->setScale(0.01f, 0.01f, 0.01f);

      m_transforms[shadowSphereId] = shadowSphereTransform;
      m_meshes[shadowSphereId]     = shadowSphereMesh;

      // LOAD TEXTURES HERE!!!      
      if(!SAE::DirectX11::LoadTextureFromFile(resourceManager, "resources/textures/Sci-Fi-Floor-Diffuse.tga", m_diffuseTextureId, m_diffuseTextureSRVId)) {
        // Ohoh...
        m_diffuseTextureId = 0;
        m_diffuseTextureSRVId = 0;
      }
      if(!SAE::DirectX11::LoadTextureFromFile(resourceManager, "resources/textures/Sci-Fi-Floor-Specular.tga", m_specularTextureId, m_specularTextureSRVId)) {
        // Ohoh...
        m_specularTextureId = 0;
        m_specularTextureSRVId = 0;
      }
      if(!SAE::DirectX11::LoadTextureFromFile(resourceManager, "resources/textures/Sci-Fi-Floor-Gloss.tga", m_glossTextureId, m_glossTextureSRVId)) {
        // Ohoh...
        m_glossTextureId = 0;
        m_glossTextureSRVId = 0;
      }
      if(!SAE::DirectX11::LoadTextureFromFile(resourceManager, "resources/textures/Sci-Fi-Floor-Normal.tga", m_normalTextureId, m_normalTextureSRVId)) {
        // Ohoh...
        m_normalTextureId = 0;
        m_normalTextureSRVId = 0;
      }

      D3D11_TEXTURE2D_DESC shadowMapTextureDesc ={};
      shadowMapTextureDesc.Width              = 1024;
      shadowMapTextureDesc.Height             = 1024;
      shadowMapTextureDesc.ArraySize          = 24;
      shadowMapTextureDesc.MipLevels          = 1;
      shadowMapTextureDesc.SampleDesc.Count   = 1;
      shadowMapTextureDesc.SampleDesc.Quality = 0;
      shadowMapTextureDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
      shadowMapTextureDesc.Usage              = D3D11_USAGE_DEFAULT;
      shadowMapTextureDesc.CPUAccessFlags     = 0;
      shadowMapTextureDesc.Format             = DXGI_FORMAT_R24G8_TYPELESS;
      shadowMapTextureDesc.MiscFlags          = D3D11_RESOURCE_MISC_TEXTURECUBE;

      std::vector<D3D11_SUBRESOURCE_DATA> shadowMapInitialData;
      m_shadowMapTextureId = resourceManager->create<ID3D11Texture2D>(shadowMapTextureDesc, shadowMapInitialData);

      ID3D11Texture2D *pShadowMapTexture = reinterpret_cast<ID3D11Texture2D*>(m_shadowMapTextureId);

      D3D11_SHADER_RESOURCE_VIEW_DESC shadowMapSRVDesc ={};
      shadowMapSRVDesc.Format                            = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
      shadowMapSRVDesc.ViewDimension                     = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
      shadowMapSRVDesc.TextureCubeArray.MipLevels        = shadowMapTextureDesc.MipLevels;
      shadowMapSRVDesc.TextureCubeArray.MostDetailedMip  = 0;
      shadowMapSRVDesc.TextureCubeArray.First2DArrayFace = 0;
      shadowMapSRVDesc.TextureCubeArray.NumCubes         = 4;
      m_shadowMapSRVId = resourceManager->create<ID3D11ShaderResourceView>(shadowMapSRVDesc, pShadowMapTexture);

      D3D11_SUBRESOURCE_DATA emptyDSVData={};

      D3D11_DEPTH_STENCIL_VIEW_DESC shadowMapDSVDesc ={};
      shadowMapDSVDesc.Format                         = DXGI_FORMAT_D24_UNORM_S8_UINT;
      shadowMapDSVDesc.ViewDimension                  = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
      shadowMapDSVDesc.Texture2DArray.ArraySize       = 1;
      shadowMapDSVDesc.Texture2DArray.MipSlice        = 0;
      for(uint32_t j=0; j < 4; ++j) {
        for(uint32_t k=0; k < 6; ++k) {
          shadowMapDSVDesc.Texture2DArray.FirstArraySlice = (j * 6) + k;
          m_shadowMapDSVId[(j * 6) + k] = resourceManager->create<ID3D11DepthStencilView>(shadowMapDSVDesc, pShadowMapTexture, emptyDSVData);
        }
      }
      // HIERARCHY GOES HERE!!!
      Node root =
      {
        0,
        {
        // First children: Triangle
        {
          lightSphereId[0],
          {}
        },
        {
          lightSphereId[1],
          {}
        },
        {
          lightSphereId[2],
          {}
        },
        {
          lightSphereId[3],
          {}
        },
        {
          planeId,
          {}
        },
        {
          shadowSphereId,
          {}
         }
      }
      };

      m_hierarchyRoot = root;

      m_displayMode = 1; // Normal

      return true;
    }

    bool Engine
      ::update(
        const Timer::State &time,
        const InputState   &inputState)
    {
      if(inputState.getPressed(KeyCode::ARROW_UP)) {
        m_defaultCamera.transform().translateVerticalBy(0.1f);
      }
      if(inputState.getPressed(KeyCode::ARROW_DOWN)) {
        m_defaultCamera.transform().translateVerticalBy(-0.1f);
      }
      if(inputState.getPressed(KeyCode::ARROW_LEFT)) {
        m_defaultCamera.transform().translateLateralBy(-0.1f);
      }
      if(inputState.getPressed(KeyCode::ARROW_RIGHT)) {
        m_defaultCamera.transform().translateLateralBy(0.1f);
      }
      if(inputState.getPressed(KeyCode::DNUM7)) {
        m_defaultCamera.transform().translateDirectionalBy(-0.1f);
      }
      if(inputState.getPressed(KeyCode::DNUM9)) {
        m_defaultCamera.transform().translateDirectionalBy(0.1f);
      }
      if(inputState.getPressed(KeyCode::DNUM8)) {
        m_defaultCamera.transform().rotateXBy(1.0f);
      }
      if(inputState.getPressed(KeyCode::DNUM2)) {
        m_defaultCamera.transform().rotateXBy(-1.0f);
      }
      if(inputState.getPressed(KeyCode::DNUM4)) {
        m_defaultCamera.transform().rotateYBy(-1.0f);
      }
      if(inputState.getPressed(KeyCode::DNUM6)) {
        m_defaultCamera.transform().rotateYBy(1.0f);
      }


      if(inputState.getPressed(KeyCode::W)) {
        m_lights[1].transform().translateDirectionalBy(0.1f);
      }
      if(inputState.getPressed(KeyCode::A)) {
        m_lights[1].transform().translateLateralBy(-0.1f);
      }
      if(inputState.getPressed(KeyCode::S)) {
        m_lights[1].transform().translateDirectionalBy(-0.1);
      }
      if(inputState.getPressed(KeyCode::D)) {
        m_lights[1].transform().translateLateralBy(0.1f);
      }
      if(inputState.getPressed(KeyCode::Q)) {
        m_lights[1].transform().translateVerticalBy(-0.1f);
      }
      if(inputState.getPressed(KeyCode::E)) {
        m_lights[1].transform().translateVerticalBy(0.1f);
      }

      if(inputState.getPressed(KeyCode::D1)) {
        m_displayMode = 1;
      }
      if(inputState.getPressed(KeyCode::D2)) {
        m_displayMode = 2;
      }
      if(inputState.getPressed(KeyCode::D3)) {
        m_displayMode = 3;
      }
      if(inputState.getPressed(KeyCode::D4)) {
        m_displayMode = 4;
      }
      if(inputState.getPressed(KeyCode::D5)) {
        m_displayMode = 5;
      }

      m_defaultCamera.update();

      std::function<void(DX11TransformPtr const&, Node &)>
        updateHierarchyFn = nullptr;

      // Transform all objects
      float rotation = (360.0f / 30.0f) * time.totalElapsed;
      m_transforms[2]->setRotation(0.0f, rotation, 0.0f);

      // Update hierarchy to generate world matrices
      DX11TransformPtr parent = DX11TransformPtr(new DX11Transform());
      updateHierarchyFn
        = [&] (DX11TransformPtr const&parent, Node &root) ->void
      {
        DX11TransformPtr transform = DX11TransformPtr(new DX11Transform());
        if(root.objectId) {
          transform = m_transforms[root.objectId];
          transform->worldMatrix(parent->composedWorldMatrix(), nullptr);
        }
        for(Node &child : root.children)
          updateHierarchyFn(transform, child);
      };

      updateHierarchyFn(parent, m_hierarchyRoot);

      return true;
    }

    /**********************************************************************************************//**
     * \fn  bool render( RenderScene &sceneHolder);
     *
     * \brief Renders the given sceneHolder
     *
     * \param [in,out]  sceneHolder The scene holder.
     *
     * \return  True if it succeeds, false if it fails.
     **************************************************************************************************/
    bool Engine
      ::render(
        PassType    const&passType,
        RenderScene      &sceneHolder,
        uint64_t    const&cubeIndex,
        uint64_t    const&shadowMapIndex)
    {
      std::vector<RenderObject> renderObjects ={};

      std::function<void(std::vector<RenderObject> &, Node &)>
        renderHierarchyFn = nullptr;

      renderHierarchyFn
        = [&] (std::vector<RenderObject> &renderObjects, Node &root) ->void
      {
        RenderObject object ={};

        DirectX11MeshPtr mesh = nullptr;
        if(root.objectId) {

          mesh = m_meshes[root.objectId];

          if(mesh) {
            object.objectId            = root.objectId;
            object.vertexBufferId      = mesh->vertexBufferHandle();
            object.indexBufferId       = mesh->indexBufferHandle();

            if(passType == PassType::Main) {
              object.vertexShaderId = mesh->vertexShaderHandle();
              object.pixelShaderId  = mesh->pixelShaderHandle();
              object.inputLayoutId  = mesh->inputLayoutHandle();
            }
            else {
              object.vertexShaderId = mesh->shadowMapVertexShaderHandle();
              object.pixelShaderId  = mesh->shadowMapPixelShaderHandle();
              object.inputLayoutId  = mesh->shadowMapInputLayoutHandle();
            }

            // Register textures
            object.diffuseTextureSRVId  = m_diffuseTextureSRVId;
            object.specularTextureSRVId = m_specularTextureSRVId;
            object.glossTextureSRVId    = m_glossTextureSRVId;
            object.normalTextureSRVId   = m_normalTextureSRVId;
          }
        }

        for(Node &child : root.children)
          renderHierarchyFn(renderObjects, child);

        if(root.objectId)
          renderObjects.push_back(object);
      };

      renderHierarchyFn(renderObjects, m_hierarchyRoot);

      sceneHolder.objects        = renderObjects;
      sceneHolder.lights         ={ 1, 2, 3, 4 };

      if(passType == PassType::Main) {
        sceneHolder.renderTargetId = 0;
        sceneHolder.cameraBufferId = m_cameraBuffer;
        sceneHolder.cameraBufferUpdateFn =
          [this] (CameraBuffer_t *ptr) -> bool
        {
          ptr->view            = m_defaultCamera.viewMatrix();
          ptr->projection      = m_defaultCamera.projectionMatrix();
          ptr->cameraPosition  = m_defaultCamera.transform().getTranslation();
          ptr->cameraDirection = m_defaultCamera.transform().getDirection();

          return true;
        };

      }
      else {
        sceneHolder.renderTargetId = m_shadowMapDSVId[(cubeIndex * 6) + shadowMapIndex];
        sceneHolder.lightBufferId  = 0;
        sceneHolder.cameraBufferId = 0;
      }
      sceneHolder.shadowMapTextureSRVId = m_shadowMapSRVId;
      sceneHolder.lightBufferId  = m_lightBuffer;
      sceneHolder.lightingBufferUpdateFn =
        [&, this] (LightBuffer_t *ptr, uint64_t const&lightId, uint64_t const&targetIndex) -> bool
      {
        if(!lightId)
          return false;

        Light &light = m_lights[lightId];
        light.transform().worldMatrix(XMMatrixIdentity(), nullptr);
;

        memcpy(ptr->lights[targetIndex].view, light.viewMatrices(), sizeof(XMMATRIX) * 6);
        ptr->lights[targetIndex].projection   = light.projectionMatrix(shadowMapIndex);
        ptr->lights[targetIndex].lightViewIndex = shadowMapIndex;
        ptr->lights[targetIndex].position     = light.transform().getTranslation();
        ptr->lights[targetIndex].direction    = light.transform().getDirection();
        ptr->lights[targetIndex].color        = light.properties().color;
        ptr->lights[targetIndex].distance     = light.properties().specificProperties.point.distance;
        ptr->lights[targetIndex].type         = 1;          // 0-Dir, 1-Point, 2-Spot
        ptr->lights[targetIndex].intensity    = light.properties().intensity;       // Intensity
        // ptr->lights[targetIndex].hotSpotAngle = RAD(30.0f); // Hot Spot Angle
        ptr->lights[targetIndex].falloffAngle = RAD(5.0f);  // Falloff Beam Angle
        ptr->lights[targetIndex].hotSpotAngle = RAD(30.0f);

        ptr->lightIndex = cubeIndex;

        return true;
      };

      sceneHolder.objectBufferId = m_objectBuffer;
      sceneHolder.otherBufferId  = m_otherBuffer;

      sceneHolder.objectBufferUpdateFn =
        [this] (ObjectBuffer_t *ptr, uint64_t const&objectId) -> bool
      {
        if(!objectId)
          return false;

        for(uint32_t k=0; k < 4; ++k)
          if(objectId == (k + 1)) { // Object for light
            Light &light = m_lights[(k + 1)];
            ptr->world             = light.transform().composedWorldMatrix();
            ptr->invTransposeWorld = XMMatrixTranspose(XMMatrixInverse(nullptr, ptr->world));
            return true;
          }

        DX11TransformPtr transform = m_transforms[objectId];
        ptr->world             = transform->composedWorldMatrix();
        ptr->invTransposeWorld = XMMatrixTranspose(XMMatrixInverse(nullptr, ptr->world));

        return true;
      };

      sceneHolder.otherBufferUpdateFn =
        [this] (OtherBuffer_t *ptr) -> bool
      {
        ptr->displayMode = m_displayMode;

        return true;
      };
      return true;
    }

    /**********************************************************************************************//**
     * \fn  bool Engine ::deinitialize()
     *
     * \brief De-initialises this object and frees any resources it is using
     *
     * \return  An Engine.
     **************************************************************************************************/
    bool Engine
      ::deinitialize()
    {
      m_defaultCamera.deinitialize();

      return true;
    }

  }
}