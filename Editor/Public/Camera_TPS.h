#pragma once

#include "Editor_Defines.h"
#include "GameObject_Camera.h"

NS_BEGIN(Editor)
class CCamera_TPS final : public CGameObject_Camera
{
public:
	typedef struct tagCameraThirdPersonDesc : public CGameObject_Camera::CAMERA_DESC
	{
		_float			fSensor;
	}CAMERA_TPS_DESC;

#pragma region Constructor/Destructor
private:
	CCamera_TPS(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_TPS(const CCamera_TPS& Prototype);
	virtual ~CCamera_TPS() = default;
public:
	static CCamera_TPS* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Clone(simdjson::ondemand::object& jsonData) override;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Initialize(simdjson::ondemand::object& jsonData);
public:
	virtual void Free() override;
#pragma endregion

#pragma region Variables
private:
	_float				m_fSensor{};
	_bool				m_bChanged = { false };
	_bool				m_bLockOn = { false };
	CGameObject*		m_pFollowTarget = { nullptr };
	CGameObject*		m_pLockOnTarget = { nullptr };
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
	virtual HRESULT Deserialize(simdjson::ondemand::object& jsonData) override;
	virtual HRESULT Prototype_Serialize(json& data) override;
	virtual HRESULT Prototype_Deserialize(simdjson::ondemand::object& jsonData) override;
#pragma endregion

};

NS_END