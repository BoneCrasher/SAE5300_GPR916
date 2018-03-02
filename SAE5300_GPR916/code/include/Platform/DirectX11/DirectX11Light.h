#ifndef __SAE5300_GPR916_LIGHT_H__
#define __SAE5300_GPR916_LIGHT_H__

#include "DirectX11Transform.h"

namespace SAE {
  namespace DirectX11 {

    class Light {
    public:
      enum class Type {
        Directional = 1,
        Point       = 2,
        Spot        = 4
      };

      struct Properties {
        Type  type;
        union SpecificProperties {
          struct Directional {
          } directional;
          struct Point {
            float    distance;
          } point;
          struct Spot {
            float    distance;
            float    hotSpotAngle;
            float    falloffAngle;
          } Spot;

          SpecificProperties()
            : point()
          {}
        } specificProperties;
        float intensity;
        XMVECTOR color;

        Properties();
      };

      Light() = default;
      Light(Properties const&);

      XMMATRIX const& viewMatrix(uint64_t faceIndex);
      XMMATRIX const& projectionMatrix(uint64_t faceIndex);

      XMMATRIX* viewMatrices();
      XMMATRIX* projectionMatrices();

      inline DX11Transform & transform() { return m_transform; }

      inline Properties const& properties() const { return m_properties; }

    private:
      void createViewMatrices();
      void createProjectionMatrices();

      DX11Transform
        m_transform;
      XMMATRIX
        m_viewMatrix[6],
        m_projectionMatrix[6];
      Properties
        m_properties;
    };

  }
}

#endif