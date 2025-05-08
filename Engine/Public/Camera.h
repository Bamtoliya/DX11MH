#pragma once
#include "GameObject.h"
NS_BEGIN(Engine)
class ENGINE_DLL CCamera abstract :public CGameObject
{
public:
	typedef struct tagCameraDesc : public GAMEOBJECT_DESC
	{
		_float4				vEye;
		_float4				vAt;

		_float				fAspect;
		_float				fFovy;
		_float				fNear;
		_float				fFar;
	}CAMERA_DESC;
protected:
#pragma region Constructor/Destructor
protected:
	CCamera(ID3D11Device* m_pDevice, ID3D11DeviceContext* m_pContext);
	CCamera(const CCamera& Prototype);
	virtual ~CCamera() = default;
public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
#pragma endregion
public:
#pragma region Initialize
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
#pragma endregion

#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta) = 0;
	virtual void Update(_float fTimeDelta) = 0;
	virtual void Late_Update(_float fTimeDelta) = 0;
public:
	virtual HRESULT Render() = 0;
public:
	virtual void Bind_Matrices();
protected:
	CAMERA_DESC			m_tDesc;
#pragma endregion
};
NS_END