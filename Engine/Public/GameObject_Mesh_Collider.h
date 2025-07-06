#pragma once
#include "Engine_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CRigidBody;
class ENGINE_DLL CGameObject_Mesh_Collider : public CGameObject
{
#pragma region Constructor/Destructor
protected:
	CGameObject_Mesh_Collider(ID3D11Device* m_pDevice, ID3D11DeviceContext* m_pContext);
	CGameObject_Mesh_Collider(const CGameObject_Mesh_Collider& Prototype);
	virtual ~CGameObject_Mesh_Collider() = default;
public:
	static CGameObject_Mesh_Collider* Create(ID3D11Device* m_pDevice, ID3D11DeviceContext* m_pContext);
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
private:
#pragma region MyRegion
	HRESULT Ready_Components();
#ifdef _DEBUG
	HRESULT Bind_ShaderResource();
#endif
#pragma endregion

private:
	CModel* m_pModelCom = { nullptr };
	CRigidBody* m_pRigidBodyCom = { nullptr };

};
NS_END;
