#pragma once
#include "State.h"

NS_BEGIN(Editor)
class CSonic_Ground_State : public CState
{
protected:
	CSonic_Ground_State(CGameObject* pOwner) : CState{ pOwner } {}
	virtual ~CSonic_Ground_State() = default;
	// 상태별 파라미터 추가
	struct StateParams
	{
		float fMoveSpeed = 5.0f;
		float fRotationSpeed = 2.0f;
		bool bCanJump = true;
		bool bCanDash = false;
		_float3 vMoveDirection = {};
	};

	StateParams m_tStateParams = {};
public:
	static CSonic_Ground_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput() override;
	void Exit() override;
	virtual void Free() override { __super::Free(); }
	void SetMoveSpeed(float fSpeed) { m_tStateParams.fMoveSpeed = fSpeed; }
	float GetMoveSpeed() const { return m_tStateParams.fMoveSpeed; }

	void SetCanDash(bool bCanDash) { m_tStateParams.bCanDash = bCanDash; }
	bool GetCanDash() const { return m_tStateParams.bCanDash; }
	void Set_Parameter(const StateParams& parameter) { memcpy(&m_tStateParams, &parameter, sizeof(StateParams)); }
	StateParams Get_Parameter() const { return m_tStateParams; }
};

class CSonic_Idle_State : public CSonic_Ground_State
{
private:
	CSonic_Idle_State(CGameObject* pOwner) : CSonic_Ground_State{ pOwner } {}
	virtual ~CSonic_Idle_State() = default;
public:
	static CSonic_Idle_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput() override;
	void Exit() override;
	virtual void Free() override { __super::Free(); }
};

class CSonic_Walk_State : public CSonic_Ground_State
{
private:
	CSonic_Walk_State(CGameObject* pOwner) : CSonic_Ground_State{ pOwner } {}
	virtual ~CSonic_Walk_State() = default;
public:
	static CSonic_Walk_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput() override;
	void Exit() override;
	virtual void Free() override { __super::Free(); }
};

class CSonic_Run_State : public CSonic_Ground_State
{
private:
	CSonic_Run_State(CGameObject* pOwner) : CSonic_Ground_State{ pOwner } {}
	virtual ~CSonic_Run_State() = default;
public:
	static CSonic_Run_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput() override;
	void Exit() override;
	virtual void Free() override { __super::Free(); }
};

class CSonic_Boost_State : public CSonic_Ground_State
{
private:
	CSonic_Boost_State(CGameObject* pOwner) : CSonic_Ground_State{ pOwner } {}
	virtual ~CSonic_Boost_State() = default;
public:
	static CSonic_Boost_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput() override;
	void Exit() override;
	virtual void Free() override { __super::Free(); }
};

class CSonic_Brake_State : public CSonic_Ground_State
{
private:
	CSonic_Brake_State(CGameObject* pOwner) : CSonic_Ground_State{ pOwner } {}
	virtual ~CSonic_Brake_State() = default;
public:
	static CSonic_Brake_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput() override;
	void Exit() override;
	virtual void Free() override { __super::Free(); }
};

NS_END