#pragma once

#include "Editor_Defines.h"
#include "GameObject_Camera.h"

NS_BEGIN(Editor)
class CCamera_Free final : public CGameObject_Camera
{
public:
	typedef struct tagCameraFreeDesc : public CGameObject_Camera::CAMERA_DESC
	{
		_float			fSensor;
	}CAMERA_FREE_DESC;

#pragma region Constructor/Destructor
private:
	CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Free(const CCamera_Free& Prototype);
	virtual ~CCamera_Free() = default;
public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
public:
	virtual void Free() override;
#pragma endregion

#pragma region Variables
private:
	_float				m_fSensor{};
	_bool				m_bChanged = { false };
#pragma endregion

#pragma region Loops
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
#pragma endregion

public:
	void Focus_OnPickedObject();

#pragma region Save&Load
public:
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(const json& data) override;
	virtual HRESULT Prototype_Serialize(json& data) override;
	virtual HRESULT Prototype_Deserialize(const simdjson::ondemand::object& jsonData) override;
#pragma endregion

};

NS_END