#include "Platform/DirectX11/DirectX11Light.h"

namespace SAE {
  namespace DirectX11 {
    
    Light::Properties::Properties()
      : type(Type::Point)
      , specificProperties()
      , intensity(1.0f)
    {}

    Light::Light(Properties const& props)
      : m_properties(props)
      , m_viewMatrix()
      , m_projectionMatrix()
    {
      // F = I  / (sq*d^2 + lin*d + c)
      // F * (sq*d^2 + lin*d + c) = I
      // (sq*d^2 + lin*d + c) = I/F
      // (sq*d^2 + lin*d + c) - (I/F) = 0
      // (sq*d^2 + lin*d + (c - (I/F)) = 0
      // ax^2 + bx + c = 0 --> Mitternachtsformel
      
      float intensity   = props.intensity;
      float falloff_sq  = 0.5f;
      float falloff_lin = 0.3f;
      float falloff_c   = 0.01f - (intensity / 0.001);

      float determinant = sqrtf(pow(falloff_lin, 2) - (4 * falloff_sq * falloff_c));
      float x1          = -falloff_lin + determinant;
      float x2          = -falloff_lin - determinant;

      x1 /= (2.0f * falloff_sq);
      x2 /= (2.0f * falloff_sq);

      m_properties.specificProperties.point.distance = max(0.0f, ((x1 < 0) ? x2 : x1));
    }

    XMMATRIX const& 
      Light::viewMatrix(uint64_t faceIndex)
    {
      createViewMatrices();
      return m_viewMatrix[faceIndex];
    }

    XMMATRIX const& 
      Light::projectionMatrix(uint64_t faceIndex)
    {
      createProjectionMatrices();
      return m_projectionMatrix[faceIndex];
    }

    XMMATRIX*
      Light::viewMatrices()
    {
      createViewMatrices();
      return m_viewMatrix;
    }

    XMMATRIX* 
      Light::projectionMatrices()
    {
      createProjectionMatrices();
      return m_projectionMatrix;
    }

    void 
      Light::createViewMatrices()
    {
      transform().worldMatrix(XMMatrixIdentity(), nullptr);
      m_viewMatrix[0] = XMMatrixLookToLH(transform().getTranslation(), XMVECTOR({  1.0f,  0.0f,  0.0f, 0.0f }), { 0.0f, 1.0f,  0.0f, 0.0f });
      m_viewMatrix[1] = XMMatrixLookToLH(transform().getTranslation(), XMVECTOR({ -1.0f,  0.0f,  0.0f, 0.0f }), { 0.0f, 1.0f,  0.0f, 0.0f });
      m_viewMatrix[2] = XMMatrixLookToLH(transform().getTranslation(), XMVECTOR({  0.0f,  1.0f,  0.0f, 0.0f }), { 0.0f, 0.0f, -1.0f, 0.0f });
      m_viewMatrix[3] = XMMatrixLookToLH(transform().getTranslation(), XMVECTOR({  0.0f, -1.0f,  0.0f, 0.0f }), { 0.0f, 0.0f,  1.0f, 0.0f });
      m_viewMatrix[4] = XMMatrixLookToLH(transform().getTranslation(), XMVECTOR({  0.0f,  0.0f,  1.0f, 0.0f }), { 0.0f, 1.0f,  0.0f, 0.0f });
      m_viewMatrix[5] = XMMatrixLookToLH(transform().getTranslation(), XMVECTOR({  0.0f,  0.0f, -1.0f, 0.0f }), { 0.0f, 1.0f,  0.0f, 0.0f });
    }

    void 
      Light::createProjectionMatrices()
    {
      transform().worldMatrix(XMMatrixIdentity(), nullptr);
      m_projectionMatrix[0] = XMMatrixPerspectiveFovLH((float)(M_PI / 2.0f), 1.0f, 0.001f, m_properties.specificProperties.point.distance);
      m_projectionMatrix[1] = XMMatrixPerspectiveFovLH((float)(M_PI / 2.0f), 1.0f, 0.001f, m_properties.specificProperties.point.distance);
      m_projectionMatrix[2] = XMMatrixPerspectiveFovLH((float)(M_PI / 2.0f), 1.0f, 0.001f, m_properties.specificProperties.point.distance);
      m_projectionMatrix[3] = XMMatrixPerspectiveFovLH((float)(M_PI / 2.0f), 1.0f, 0.001f, m_properties.specificProperties.point.distance);
      m_projectionMatrix[4] = XMMatrixPerspectiveFovLH((float)(M_PI / 2.0f), 1.0f, 0.001f, m_properties.specificProperties.point.distance);
      m_projectionMatrix[5] = XMMatrixPerspectiveFovLH((float)(M_PI / 2.0f), 1.0f, 0.001f, m_properties.specificProperties.point.distance);
    }

  }
}