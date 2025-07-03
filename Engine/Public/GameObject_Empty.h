#pragma once
#include "Engine_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class ENGINE_DLL CGameObject_Empty : public CGameObject
{
#pragma region Constructor/Destructor
protected:
	CGameObject_Empty(ID3D11Device* m_pDevice, ID3D11DeviceContext* m_pContext);
	CGameObject_Empty(const CGameObject_Empty& Prototype);
	virtual ~CGameObject_Empty() = default;
public:
	static CGameObject_Empty* Create(ID3D11Device* m_pDevice, ID3D11DeviceContext* m_pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual CGameObject* Clone(simdjson::ondemand::object& jsonData) override;
	virtual void Free() override;
#pragma endregion
#pragma region Initialize
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Initialize(simdjson::ondemand::object& jsonData);
#pragma endregion
#pragma region Save&Load
public:
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object& jsonData) override;
	virtual HRESULT Prototype_Serialize(json& data) override;
	virtual HRESULT Prototype_Deserialize(simdjson::ondemand::object& jsonData) override;
#pragma endregion
#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
public:
	virtual HRESULT Pre_Render() override;
	virtual HRESULT Render() override;
	virtual HRESULT Post_Render() override;
#pragma endregion
};
NS_END;
