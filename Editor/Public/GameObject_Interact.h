
#pragma once
#include "Editor_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
NS_END

NS_BEGIN(Editor)

class CGameObject_Interact : public CGameObject
{
#pragma region Constructor/Destructor
private:
	CGameObject_Interact(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject_Interact(const CGameObject_Interact& Prototype);
	virtual ~CGameObject_Interact() = default;
public:
	static CGameObject_Interact* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Clone(simdjson::ondemand::object& jsonData) override;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Initialize(simdjson::ondemand::object& jsonData);
public:
	virtual void Free() override;
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
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
public:
	virtual HRESULT Render();
#pragma endregion

#pragma region Collision
public:
	virtual void OnCollisionEnter(PxActor* pOther, const PxContactPair& cp) override;
	virtual void OnCollisionOverlapping(PxActor* pOther) override;
	virtual void OnCollisionExit(PxActor* pOther) override;
public:
	void LockOn();
#pragma endregion
};

NS_END