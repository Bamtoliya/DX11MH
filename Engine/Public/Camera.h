#pragma once
#include "Component.h"
NS_BEGIN(Engine)
class ENGINE_DLL CCamera : public CComponent
{
#pragma region Structures
public:
	typedef struct tagCameraComponentDesc {
		_float4				vEye;
		_float4				vAt;
		_float				fAspect;
		_float				fFovy;
		_float				fNear;
		_float				fFar;
		_bool				bOrthographic = false;
	} CAMERA_COM_DESC;
#pragma endregion

#pragma region Constructor/Destructor
protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& Prototype);
	virtual ~CCamera() = default;
public:
	static CCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCamera* Clone(void* pArg) override;
	virtual void Free();
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
#pragma endregion
#pragma region Variables
protected:
	CAMERA_COM_DESC m_tDesc = {};
#pragma endregion
#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
public:
	virtual HRESULT Render() override;
public:
	virtual void Bind_Matrices(class CTransform* pTransformCom);
#pragma endregion
public:
	virtual HRESULT RenderImGui() override;

};
NS_END