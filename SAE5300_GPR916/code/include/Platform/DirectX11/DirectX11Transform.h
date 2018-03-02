#ifndef __SAE5300_DX11TRANSFORM_H__
#define __SAE5300_DX11TRANSFORM_H__

#define _USE_MATH_DEFINES
#include <math.h>

#define RAD(a) (float) ((a / 180.0f) * M_PI)
#define DEG(r) (float) ((r / M_PI)  * 180.0f)

#include "Platform/DirectX11/DirectX11Common.h"
#include "Engine/Transform.h"

namespace SAE {
  namespace DirectX11 {
    using namespace DirectX;

    class DX11Transform
      : public Engine::ITransform<XMVECTOR, XMMATRIX>
    {
    public:
      inline DX11Transform()
        : m_scale({ 1.0f, 1.0f, 1.0f })
        , m_rotation(XMMatrixIdentity())
        , m_translation({ 0.0f, 0.0f, 0.0f })
        , m_direction({ 0.0f, 0.0f, 1.0f })
        , m_right({ 1.0f, 0.0f, 0.0f })
        , m_up({ 0.0f, 1.0f, 0.0f })
        , m_local(XMMatrixIdentity())
        , m_composed(XMMatrixIdentity())
      {

      }

      inline const XMVECTOR& getDirection() const { return m_direction; };
      inline const XMVECTOR& getUp()        const { return m_up; };
      inline const XMVECTOR& getRight()     const { return m_right; };

      inline void setScaleX(const float& factor) { VEC_X(m_scale) = factor; invalidate(); }
      inline void setScaleY(const float& factor) { VEC_Y(m_scale) = factor; invalidate(); }
      inline void setScaleZ(const float& factor) { VEC_Z(m_scale) = factor; invalidate(); }
      inline void setScale(
        const float& x,
        const float& y,
        const float& z)
      {
        setScaleX(x);
        setScaleY(y);
        setScaleZ(z);
        invalidate();
      }
      inline void setScale(const XMVECTOR& vec) { m_scale = vec; invalidate(); }

      inline const XMVECTOR& getScale() const { return m_scale; }

      inline void rotateAroundAxisBy(const XMVECTOR& axis, float angle)
      {
        XMMATRIX rot = XMMatrixRotationAxis(axis, angle);
        m_rotation *= rot;
        invalidate(); invalidate();
      }

      inline void rotateXBy(const float& angle) { rotateAroundAxisBy({ 1.0f, 0.0f, 0.0f, 0.0f }, RAD(angle)); }
      inline void rotateYBy(const float& angle) { rotateAroundAxisBy({ 0.0f, 1.0f, 0.0f, 0.0f }, RAD(angle)); }
      inline void rotateZBy(const float& angle) { rotateAroundAxisBy({ 0.0f, 0.0f, 1.0f, 0.0f }, RAD(angle)); }

      inline void rollBy(const float& angle) { rotateAroundAxisBy(m_direction, RAD(angle)); }
      inline void pitchBy(const float& angle) { rotateAroundAxisBy(m_right, RAD(angle)); }
      inline void yawBy(const float& angle) { rotateAroundAxisBy(m_up, RAD(angle)); }

      inline void setRotationX(const float& angle) { setRotation(angle, 0, 0); }
      inline void setRotationY(const float& angle) { setRotation(0, angle, 0); }
      inline void setRotationZ(const float& angle) { setRotation(0, 0, angle); }
      inline void setRotation(
        const float& x,
        const float& y,
        const float& z)
      {
        setRotation({ x, y, z, 0.0f });
      }
      inline void setRotation(const XMVECTOR& vec) {
        m_rotation  = XMMatrixIdentity();
        m_rotation *= XMMatrixRotationY(RAD(VEC_Y(vec)));
        m_rotation *= XMMatrixRotationX(RAD(VEC_X(vec)));
        m_rotation *= XMMatrixRotationZ(RAD(VEC_Z(vec)));

        invalidate();
      }

      inline const XMVECTOR getRotation() const {
        return {
          DEG(atan2(VEC_X(m_rotation.r[1]),  VEC_X(m_rotation.r[0]))),
          DEG(atan2(-VEC_X(m_rotation.r[2]), sqrt(pow(VEC_Y(m_rotation.r[2]), 2) + pow(VEC_Z(m_rotation.r[2]),2)))),
          DEG(atan2(VEC_Y(m_rotation.r[2]),  VEC_Z(m_rotation.r[2]))),
          0.0f
        };
      }

      inline void translateXBy(const float& offset) { VEC_X(m_translation) += offset; invalidate(); }
      inline void translateYBy(const float& offset) { VEC_Y(m_translation) += offset; invalidate(); }
      inline void translateZBy(const float& offset) { VEC_Z(m_translation) += offset; invalidate(); }

      inline void translateDirectionalBy(const float& offset) { m_translation += (offset * m_direction); invalidate(); }
      inline void translateVerticalBy(const float& offset) { m_translation += (offset * m_up);        invalidate(); }
      inline void translateLateralBy(const float& offset) { m_translation += (offset * m_right);     invalidate(); }

      inline void setTranslationX(const float& offset) { VEC_X(m_translation) = offset; invalidate(); }
      inline void setTranslationY(const float& offset) { VEC_Y(m_translation) = offset; invalidate(); }
      inline void setTranslationZ(const float& offset) { VEC_Z(m_translation) = offset; invalidate(); }
      inline void setTranslation(
        const float& x,
        const float& y,
        const float& z)
      {
        setTranslationX(x);
        setTranslationY(y);
        setTranslationZ(z);
        invalidate();
      }
      inline void setTranslation(const XMVECTOR& vec) { m_translation = vec; invalidate(); }

      inline const XMVECTOR& getTranslation() const { return m_translation; }

      inline void worldMatrix(const XMMATRIX& parent, XMMATRIX *pCombined)
      {
        m_composed =  XMMatrixMultiply(m_local, parent);

        if(pCombined)
          *pCombined = composedWorldMatrix();
      }

      inline XMMATRIX composedWorldMatrix() const
      {
        return m_composed;
      }

    private:
      inline void invalidate()
      {
        using namespace DirectX;

        static XMVECTOR rgt ={ 1.0f, 0.0f, 0.0f, 0.0f };
        static XMVECTOR up  ={ 0.0f, 1.0f, 0.0f, 0.0f };
        static XMVECTOR fwd ={ 0.0f, 0.0f, 1.0f, 0.0f };

        XMMATRIX T  = XMMatrixTranslation(VEC_X(m_translation), VEC_Y(m_translation), VEC_Z(m_translation));
        XMMATRIX R  = m_rotation;
        XMMATRIX S  = XMMatrixScaling(VEC_X(m_scale), VEC_Y(m_scale), VEC_Z(m_scale));

        m_local = XMMatrixMultiply(XMMatrixMultiply(S, R), T);

        // Recalculate the axis vectors
        m_right     = R.r[0];
        m_up        = R.r[1];
        m_direction = R.r[2];
      }

      XMVECTOR m_scale;
      XMMATRIX m_rotation;
      XMVECTOR m_translation;

      XMVECTOR m_direction;
      XMVECTOR m_right;
      XMVECTOR m_up;

      XMMATRIX m_local;
      XMMATRIX m_composed;
    };
    using DX11TransformPtr = std::shared_ptr<DX11Transform>;
    using TransformPtr     = DX11TransformPtr;

    //static void hierarchicalTransformExample()
    //{
    //	Transform o0;
    //	o0.translateXBy(20.0f);
    //
    //	Transform o1;
    //	o1.rotateXBy(30.0f);
    //
    //	XMMATRIX root = XMMatrixIdentity();
    //	XMMATRIX world0;
    //	XMMATRIX world1;
    //	o0.worldMatrix(root, &world0);
    //	o1.worldMatrix(world0, &world1);
    //}


    //struct ObjectMatrixBuffer
    //{
    //	XMMATRIX _WORLD;
    //	XMMATRIX _INVERSE_WORLD;
    //	XMVECTOR heightScale;
    //};

    //HRESULT SAECreateWorldMatrixBuffer(
    //	ResourceManager &resourceManager,
    //	unsigned int     deviceId,
    //	unsigned int    *pOutBuffer
    //)
    //{
    //	ResourcePtr<ID3D11Device> pDevice = resourceManager.getDevice(deviceId);
    //	if( !pDevice )
    //		return E_INVALIDARG;

    //	HRESULT res = S_OK;

    //	D3D11_BUFFER_DESC desc ={ 0 };
    //	desc.ByteWidth           = sizeof(ObjectMatrixBuffer);
    //	desc.Usage               = D3D11_USAGE_DYNAMIC;
    //	desc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
    //	desc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
    //	desc.MiscFlags           = 0;
    //	desc.StructureByteStride = 0;

    //	ID3D11Buffer *tmpBuffer = nullptr;
    //	res = pDevice->CreateBuffer(&desc, nullptr, &tmpBuffer);
    //	if( FAILED(res) )
    //	{
    //		std::cout << "Mimimi" << std::endl;
    //	}

    //	*pOutBuffer = resourceManager.addBuffer(wrapDxResource(tmpBuffer));

    //	return res;
    //}

    //HRESULT SAEUpdateWorldMatrices(
    //	Engine::ResourceManager& resourceManager,
    //	unsigned int             contextId,
    //	unsigned int             bufferId,
    //	const XMMATRIX          &worldMatrix)
    //{
    //	ResourcePtr<ID3D11DeviceContext> pContext = resourceManager.getDeviceContext(contextId);
    //	ResourcePtr<ID3D11Buffer>        pBuffer  = resourceManager.getBuffer(bufferId);

    //	if( !(pContext && pBuffer) )
    //	{
    //		return E_INVALIDARG;
    //	}

    //	HRESULT res = S_OK;

    //	D3D11_MAPPED_SUBRESOURCE subresource;

    //	// Establish connection to temporary memory location used 
    //	// to stream to GPU on Unmap
    //	res = pContext->Map(
    //		pBuffer.get(),   // Buffer-Handle
    //		0,                       // Subresource-Index in Buffer
    //		D3D11_MAP_WRITE_DISCARD, // What shall we do with the data once unmapped?
    //		0,                       // Any additional flags?
    //		&subresource);           // Mapped-Subresource description.
    //	if( FAILED(res) )
    //	{
    //		std::cout << "Mimimimi" << std::endl;
    //		return res;
    //	}

    //	// Set data
    //	ObjectMatrixBuffer *pData = nullptr;

    //	// Make void * usable and copy data.
    //	pData = (ObjectMatrixBuffer *)subresource.pData;
    //	pData->_WORLD = worldMatrix; // XMMatrixTranspose(worldMatrix);;
    //								 // Release connection
    //	pContext->Unmap(pBuffer.get(), 0);

    //	return res;
    //}

  }
}

#endif
