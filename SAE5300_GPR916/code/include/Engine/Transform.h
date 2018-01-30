#ifndef __SAE5300_GPR916_TRANSFORM_H__
#define __SAE5300_GPR916_TRANSFORM_H__

#include <iostream>
#include <array>
#include <memory>

namespace SAE {
  namespace Engine
  {
    template <typename TFloatN, typename TFloat4x4>
    class ITransform
    {
    public:
      virtual const TFloatN& getDirection() const = 0;
      virtual const TFloatN& getUp()        const = 0;
      virtual const TFloatN& getRight()     const = 0;

      virtual void setScaleX(const float& factor) = 0;
      virtual void setScaleY(const float& factor) = 0;
      virtual void setScaleZ(const float& factor) = 0;
      virtual void setScale(
        const float& x,
        const float& y,
        const float& z) = 0;
      virtual void setScale(const TFloatN& vec) = 0;

      virtual const TFloatN& getScale() const = 0;

      virtual void rotateXBy(const float& angle)     = 0;
      virtual void rotateYBy(const float& angle)     = 0;
      virtual void rotateZBy(const float& angle)     = 0;
      virtual void setRotationX(const float& angle)  = 0;
      virtual void setRotationY(const float& angle)  = 0;
      virtual void setRotationZ(const float& angle)  = 0;
      virtual void setRotation(
        const float& x,
        const float& y,
        const float& z)
      {
        setRotationX(x);
        setRotationY(y);
        setRotationZ(z);
        invalidate();
      }
      virtual void setRotation(const TFloatN& vec) = 0;
      virtual const TFloatN getRotation() const = 0;


      virtual void translateXBy(const float& offset) = 0;
      virtual void translateYBy(const float& offset) = 0;
      virtual void translateZBy(const float& offset) = 0;
      virtual void setTranslationX(const float& offset) = 0;
      virtual void setTranslationY(const float& offset) = 0;
      virtual void setTranslationZ(const float& offset) = 0;
      virtual void setTranslation(
        const float& x,
        const float& y,
        const float& z) = 0;
      virtual void setTranslation(const TFloatN& vec)  = 0;

      virtual const TFloatN& getTranslation() const = 0;

      virtual void worldMatrix(const TFloat4x4& parent, TFloat4x4 *pCombined) = 0;

      virtual TFloat4x4 composedWorldMatrix() const = 0;

    private:
      virtual void invalidate() = 0;
    };

    template <typename TFloatN, typename TFloat4x4>
    using ITransformPtr = std::shared_ptr<ITransform<TFloatN, TFloat4x4>>;
  }
}

#endif