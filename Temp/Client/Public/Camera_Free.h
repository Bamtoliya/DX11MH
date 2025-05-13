#pragma once

#include "Client_Defines.h"
#include "GameObject_Camera.h"

NS_BEGIN(Client)

class CCamera_Free final : public CGameObject_Camera
{
public:
	typedef struct tagCameraFreeDesc : public CGameObject_Camera::CAMERA_DESC
	{
		_float			fSensor;
	}CAMERA_FREE_DESC;
private:
	CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Free(const CCamera_Free& Prototype);
	virtual ~CCamera_Free() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_float				m_fSensor{};

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END