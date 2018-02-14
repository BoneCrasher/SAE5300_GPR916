#include <map>
#include <functional>

#include "Logging/Logging.h"

#include "Engine/Engine.h"

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
      lightTransform->setTranslation(-10.0f, 10.0f, 0.0f);

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
      
      // LOAD MESHES HERE!!!
      uint64_t triangleId = 1;
      std::shared_ptr<SAE::DirectX11::DirectX11Mesh>
        triangleMesh = nullptr;

      triangleMesh = SAE::DirectX11::DirectX11Mesh::loadFromFile(resourceManager, "");

      DX11TransformPtr triangleTransform = DX11TransformPtr(new DX11Transform());
      triangleTransform->setTranslationZ(5);

      m_transforms[triangleId] = triangleTransform;
      m_meshes[triangleId]     = triangleMesh;

      // LOAD TEXTURES HERE!!!

      // HIERARCHY GOES HERE!!!
      Node root = 
      { 
        0,
        {
          // First children: Triangle
          {
            triangleId,
            {}
          }
        } 
      };

      m_hierarchyRoot = root;

      return true;
    }

    bool Engine
      ::update(
        const Timer::State &time,
        const InputState   &inputState)
    {
      m_defaultCamera.update();

      std::function<void(DX11TransformPtr const&, Node &)>
        updateHierarchyFn = nullptr;

      // Transform all objects
      float rotation = (360.0f / 15.0f) * time.totalElapsed;
      m_transforms[1]->setRotation(0.0f, rotation, 0.0f);

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
            object.objectId       = root.objectId;
            object.vertexBufferId = mesh->vertexBufferHandle();
            object.indexBufferId  = mesh->indexBufferHandle();
            object.inputLayoutId  = mesh->inputLayoutHandle();
            object.vertexShaderId = mesh->vertexShaderHandle();
            object.pixelShaderId  = mesh->pixelShaderHandle();
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
      sceneHolder.cameraBufferUpdateFn = 
        [this] (CameraBuffer_t *ptr) -> bool
      {
        ptr->view       = m_defaultCamera.viewMatrix();
        ptr->projection = m_defaultCamera.projectionMatrix();
        return true;
      };

      sceneHolder.objectBufferUpdateFn = 
        [this] (ObjectBuffer_t *ptr, uint64_t const&objectId) -> bool
      {
        if(!objectId)
          return false;

        DX11TransformPtr transform = m_transforms[objectId];
        ptr->world = transform->composedWorldMatrix();
        return true;
      };

      sceneHolder.lightingBufferUpdateFn = 
        [this] (LightBuffer_t *ptr, uint64_t const&lightId) -> bool
      {
        if(!lightId)
          return false;

        DX11TransformPtr transform = m_lights[lightId];
        ptr->position = transform->getTranslation();
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