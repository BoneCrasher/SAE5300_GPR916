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
      cameraProperties.aspectRatio  = 1920.0f/1080.0f;
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

      DX11TransformPtr lightTransform = DX11TransformPtr(new DX11Transform());
      lightTransform->setTranslation(0.0f, 20.0f, 40.0f);

      m_lights[1] = lightTransform;

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


      // LOAD MESHES HERE!!!
      uint64_t triangleId = 1;
      std::shared_ptr<SAE::DirectX11::DirectX11Mesh>
        triangleMesh = nullptr;

      triangleMesh = SAE::DirectX11::DirectX11Mesh::loadTriangle(resourceManager, "");

      DX11TransformPtr triangleTransform = DX11TransformPtr(new DX11Transform());
      triangleTransform->setTranslationZ(5);

      m_transforms[triangleId] = triangleTransform;
      m_meshes[triangleId]     = triangleMesh;

      uint64_t cubeId = 2;
      std::shared_ptr<SAE::DirectX11::DirectX11Mesh>
        cubeMesh = nullptr;

      // cubeMesh = SAE::DirectX11::DirectX11Mesh::loadFromFile(resourceManager, "resources/meshes/Sci-Fi-Floor-1-OBJ.obj");
      cubeMesh = SAE::DirectX11::DirectX11Mesh::loadFromFile(resourceManager, "resources/meshes/fourQuadPlane.obj");

      DX11TransformPtr cubeTransform = DX11TransformPtr(new DX11Transform());
      cubeTransform->setTranslationZ(20);
      cubeTransform->setScale(3.0f, 3.0f, 3.0f);

      m_transforms[cubeId] = cubeTransform;
      m_meshes[cubeId]     = cubeMesh;
      
      lightTransform->setTranslation(cubeTransform->getTranslation()); // Place light in the middle of the plane and shift up
      lightTransform->translateVerticalBy(10);

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

      // HIERARCHY GOES HERE!!!
      Node root = 
      { 
        0,
        {
          // First children: Triangle
          {
            triangleId,
            {}
          },
          {
            cubeId,
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
        m_lights[1]->translateDirectionalBy(0.1f);
      }
      if(inputState.getPressed(KeyCode::A)) {
        m_lights[1]->translateLateralBy(-0.1f);
      }
      if(inputState.getPressed(KeyCode::S)) {
        m_lights[1]->translateDirectionalBy(-0.1);
      }
      if(inputState.getPressed(KeyCode::D)) {
        m_lights[1]->translateLateralBy(0.1f);
      }
      if(inputState.getPressed(KeyCode::Q)) {
        m_lights[1]->translateVerticalBy(-0.1f);
      }
      if(inputState.getPressed(KeyCode::E)) {
        m_lights[1]->translateVerticalBy(0.1f);
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
        RenderScene &sceneHolder)
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
            object.inputLayoutId       = mesh->inputLayoutHandle();
            object.vertexShaderId      = mesh->vertexShaderHandle();
            object.pixelShaderId       = mesh->pixelShaderHandle();
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
      sceneHolder.lights         ={ 1 };
      sceneHolder.cameraBufferId = m_cameraBuffer;
      sceneHolder.objectBufferId = m_objectBuffer;
      sceneHolder.lightBufferId  = m_lightBuffer;
      sceneHolder.otherBufferId  = m_otherBuffer;
      sceneHolder.cameraBufferUpdateFn = 
        [this] (CameraBuffer_t *ptr) -> bool
      {
        ptr->view            = m_defaultCamera.viewMatrix();
        ptr->projection      = m_defaultCamera.projectionMatrix();
        ptr->cameraPosition  = m_defaultCamera.transform().getTranslation();
        ptr->cameraDirection = m_defaultCamera.transform().getDirection();

        return true;
      };

      sceneHolder.objectBufferUpdateFn = 
        [this] (ObjectBuffer_t *ptr, uint64_t const&objectId) -> bool
      {
        if(!objectId)
          return false;

        if(objectId == 1) { // TRiangle for light
          DX11TransformPtr transform = m_lights[1];
          ptr->world             = transform->composedWorldMatrix();
          ptr->invTransposeWorld = XMMatrixTranspose(XMMatrixInverse(nullptr, ptr->world));
        }
        else {
          DX11TransformPtr transform = m_transforms[objectId];
          ptr->world             = transform->composedWorldMatrix();
          ptr->invTransposeWorld = XMMatrixTranspose(XMMatrixInverse(nullptr, ptr->world));
        }
        return true;
      };

      sceneHolder.lightingBufferUpdateFn = 
        [this] (LightBuffer_t *ptr, uint64_t const&lightId) -> bool
      {
        if(!lightId)
          return false;

        DX11TransformPtr transform = m_lights[lightId];
        transform->worldMatrix(XMMatrixIdentity(), nullptr);
        ptr->lights[0] = transform->composedWorldMatrix();
        // First column: Color
        ptr->lights[0].m[0][0] = 1.0f;
        ptr->lights[0].m[0][1] = 1.0f;
        ptr->lights[0].m[0][2] = 1.0f;
        // Second column: Falloff Properties
        ptr->lights[0].m[1][0] = 5.0f;
        ptr->lights[0].m[1][1] = 1.0f;
        ptr->lights[0].m[1][2] = 1.0f;
        // Third column: Direction
        // Fourth column: Falloff Properties
        ptr->lights[0].m[0][3] = 1.0f;       // 0-Dir, 1-Point, 2-Spot
        ptr->lights[0].m[1][3] = 1.0f;       // Intensity
        ptr->lights[0].m[2][3] = RAD(30.0f); // Hot Spot Angle
        ptr->lights[0].m[3][3] = RAD(5.0f);  // Falloff Beam Angle

        // Clean 
        ptr->lights[1] = XMMatrixIdentity();
        ptr->lights[2] = XMMatrixIdentity();
        ptr->lights[3] = XMMatrixIdentity();

        ptr->lightIndex  = 0;
        
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