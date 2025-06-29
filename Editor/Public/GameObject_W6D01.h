#pragma once
#include "Editor_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CVIBuffer_Instance;
class CShader;
class CRigidBody;
NS_END

NS_BEGIN(Editor)
class CGameObject_W6D01 : public CGameObject
{
#pragma region Constructor/Destructor
private:
	CGameObject_W6D01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject_W6D01(const CGameObject_W6D01& Prototype);
	virtual ~CGameObject_W6D01() = default;
public:
	static CGameObject_W6D01* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
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
	CShader*				m_pShaderCom = { nullptr };
	CModel*					m_pModelCom = { nullptr };
	CModel*					m_pModelColliderCom = { nullptr };
	CVIBuffer_Instance*		m_pVIBufferCom = { nullptr };
	CRigidBody*				m_pRigidBodyCom = { nullptr };
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
#pragma endregion
};
NS_END