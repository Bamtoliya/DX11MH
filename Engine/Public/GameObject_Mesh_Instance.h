#pragma once
#include "Engine_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Mesh_Instance;
class ENGINE_DLL CGameObject_Mesh_Instance final : public CGameObject
{
#pragma region Constructor/Destructor
private:
	CGameObject_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject_Mesh_Instance(const CGameObject_Mesh_Instance& Prototype);
	virtual ~CGameObject_Mesh_Instance() = default;
public:
	static CGameObject_Mesh_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Clone(simdjson::ondemand::object& jsonData) override;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Initialize(simdjson::ondemand::object& jsonData);
private:
	HRESULT Ready_Components();
public:
	virtual void Free() override;
#pragma endregion

#pragma region Variables
private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CVIBuffer_Mesh_Instance* m_pVIBufferInstanceCom = { nullptr };
	vector<CTransform*> m_pInstanceTransform = {};
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
private:
	HRESULT Bind_ShaderResources();
public:
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow() override;
#pragma endregion

#pragma region Collision
public:
	virtual void OnCollisionEnter(PxActor* pOther, const PxContactPair& cp) {}
	virtual void OnCollisionOverlapping(PxActor* pOther) {}
	virtual void OnCollisionExit(PxActor* pOther) {}
#pragma endregion

};

NS_END