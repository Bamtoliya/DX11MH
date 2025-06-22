#pragma once
#include "Editor_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CModel;
class CAnimator;
class CController;
class CRigidBody;
NS_END

NS_BEGIN(Editor)

class CAshura final : public CGameObject
{
public:
#pragma region AnimIndex
	enum STATE {
		CYLOOP_REACT_L_END,
		CYLOOP_REACT_L_LOOP,
		CYLOOP_REACT_L_START,
		CYLOOP_REACT_R_END,
		CYLOOP_REACT_R_LOOP,
		CYLOOP_REACT_R_START,
		DEAD,
		DESTROY_ARM_C,
		DESTROY_ARM_L,
		DESTROY_ARM_R,
		DESTROY_FOOT_L,
		DESTROY_FOOT_R,
		IDLE_LOOP,
		ROLL,
		SLAM_ATK_C_END,
		SLAM_ATK_C_LOOP,
		SLAM_ATK_C_START,
		SLAM_ATK_L_END,
		SLAM_ATK_L_LOOP,
		SLAM_ATK_L_START,
		SLAM_ATK_R_END,
		SLAM_ATK_R_LOOP,
		SLAM_ATK_R_START,
		SLAM_BASE_LOOP,
		SLAM_C_READY_LOOP,
		SLAM_C_READY_START,
		SLAM_L_READY_LOOP,
		SLAM_L_READY_START,
		SLAM_R_READY_LOOP,
		SLAM_R_READY_START,
		SPECIAL_ATK_DAMAGE_HEAD_C,
		SPECIAL_ATK_DAMAGE_HEAD_L,
		SPECIAL_ATK_DAMAGE_HEAD_R,
		STUN_END,
		STUN_LOOP,
		STUN_START,
		TURN90_L,
		TURN90_R,
		WALK_END,
		WALK_LOOP,
		WALK_START,
		END
	};
#pragma endregion

#pragma region Constructor/Destructor
private:
	CAshura(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAshura(const CAshura& Prototype);
	virtual ~CAshura() = default;
public:
	static CAshura* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual CGameObject* Clone(simdjson::ondemand::object& jsonData) override;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Initialize(simdjson::ondemand::object& jsonData);
private:
	HRESULT Ready_Components();
	HRESULT Ready_States();
public:
	virtual void Free() override;
#pragma endregion

#pragma region Variables
private:
	CShader*	m_pShaderCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };
	CModel*		m_pModelColliderCom = { nullptr };
	CAnimator* m_pAnimatorCom = { nullptr };
	CController* m_pControllerCom = { nullptr };
	CRigidBody* m_pRigidBodyCom = { nullptr };
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