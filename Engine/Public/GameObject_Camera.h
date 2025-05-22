#pragma once
#include "GameObject.h"
NS_BEGIN(Engine)
class CCamera;
class ENGINE_DLL CGameObject_Camera abstract :public CGameObject
{
public:
	typedef struct tagCameraDesc : CGameObject::GAMEOBJECT_DESC, CCamera::CAMERA_COM_DESC
	{

	} CAMERA_DESC;
protected:
#pragma region Constructor/Destructor
protected:
	CGameObject_Camera(ID3D11Device* m_pDevice, ID3D11DeviceContext* m_pContext);
	CGameObject_Camera(const CGameObject_Camera& Prototype);
	virtual ~CGameObject_Camera() = default;
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
#pragma endregion
protected:
	CCamera* m_pCameraCom = { nullptr };
};
NS_END