#include "Platform/DirectX11/DirectX11Camera.h"

namespace SAE {
  namespace DirectX11 {
    
    Camera::Properties::Properties()
      : cameraType(Camera::Type::Free)
      , direction({ 0.0f, 0.0f, 1.0f, 0.0f })
      , fieldOfViewY(45.0)
      , nearPlane(1.0)
      , farPlane(1000.0)
    {}

    Camera::Camera(Properties const& props)
      : m_properties(props)
      , m_viewMatrix(XMMatrixIdentity())
      , m_projectionMatrix(XMMatrixIdentity())
    {
    }

    XMMATRIX const& 
      Camera::viewMatrix() const
    {
      return m_viewMatrix;
    }

    XMMATRIX const& 
      Camera::projectionMatrix() const
    {
      return m_projectionMatrix;
    }

    void 
      Camera::initialize()
    {
      createViewMatrix();
      createProjectionMatrix();
    }

    void 
      Camera::update()
    {
      createViewMatrix();
    }

    void 
      Camera::deinitialize()
    {
    }
    
    void 
      Camera::createViewMatrix()
    {
      m_viewMatrix = XMMatrixLookToLH(
        m_transform.getTranslation(),
        m_transform.getDirection(),
        m_transform.getUp());
    }

    void 
      Camera::createProjectionMatrix()
    {
      m_projectionMatrix = XMMatrixPerspectiveFovLH(
        m_properties.fieldOfViewY,
        m_properties.aspectRatio,
        m_properties.nearPlane,
        m_properties.farPlane);
    }

  }
}