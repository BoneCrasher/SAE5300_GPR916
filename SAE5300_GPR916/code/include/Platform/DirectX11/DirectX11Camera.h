#ifndef __SAE5300_GPR916_H__
#define __SAE5300_GPR916_H__

#include "DirectX11Transform.h"

namespace SAE {
  namespace DirectX11 {

    class Camera {
    public:
      enum class Type {
        Free   = 1,
        Locked = 2
      };

      enum class PerspectiveMode {
        ByFieldOfViewY     = 1,
        ByFixedScreenSize 
      };

      struct Properties {
        Type cameraType;
        union {
          // Locked camera
          XMVECTOR lockTarget;
          // Free camera
          XMVECTOR direction;
        };

        PerspectiveMode perspectiveMode;
        union {
          double
            fieldOfViewY,
            aspectRatio;
          uint32_t
            width,
            height;
        };

        double
          nearPlane,
          farPlane;

        Properties();
      };

      Camera() = default;
      Camera(Properties const&);

      XMMATRIX const& viewMatrix() const;
      XMMATRIX const& projectionMatrix() const;

      void initialize();
      void update();
      void deinitialize();

      inline DX11Transform & transform() { return m_transform; }

    private:
      void createViewMatrix();
      void createProjectionMatrix();

      Properties
        m_properties;
      DX11Transform
        m_transform;
      XMMATRIX
        m_viewMatrix,
        m_projectionMatrix;
    };

  }
}

#endif