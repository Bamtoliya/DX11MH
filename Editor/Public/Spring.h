#pragma once
#include "Editor_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CRigidBody;
NS_END

NS_BEGIN(Editor)

class CSpring final : public CGameObject
{
#pragma region Constructor/Destructor
private:
	CSpring(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpring(const CSpring& Prototype);
	virtual ~CSpring() = default;
public:
	static CSpring* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
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
	CShader*	m_pShaderCom	= { nullptr };
	CModel*		m_pModelCom		= { nullptr };
	CRigidBody* m_pRigidBodyCom = { nullptr };
	CAnimator*	m_pAnimatorCom	= { nullptr };
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

#pragma region Collision
public:
	virtual void OnCollisionEnter(PxActor* pOther, const PxContactPair& cp) override;
	virtual void OnCollisionOverlapping(PxActor* pOther) override;
	virtual void OnCollisionExit(PxActor* pOther) override;
#pragma endregion

};

NS_END