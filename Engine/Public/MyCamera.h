#pragma once
#include "Component.h"
NS_BEGIN(Engine)
class ENGINE_DLL CMyCamera : public CComponent
{
public:
	typedef struct tagCameraComponentDesc {
		_float4				vAt;
		_float				fAspect;
		_float				fFovy;
		_float				fNear;
		_float				fFar;
		_bool				bOrthographic = false;
	} CAMERA_COM_DESC;
#pragma region Constructor/Destructor
protected:
	CMyCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMyCamera(const CMyCamera& Prototype);
	virtual ~CMyCamera() = default;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
public:
	static CMyCamera* Create();
	virtual CMyCamera* Clone(void* pArg) override;
	virtual void Free();
#pragma endregion
#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
public:
	virtual HRESULT Render();
public:
	virtual void Bind_Matrices(class CTransform* pTransformCom);
#pragma endregion
public:
	virtual HRESULT RenderImGui();
	virtual void* Dump() { return nullptr; }
#pragma region Variables
protected:
	CAMERA_COM_DESC m_tDesc = {};
#pragma endregion
};
NS_END