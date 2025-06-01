#pragma once
#include "State.h"

NS_BEGIN(Editor)
class CSonic_Ground_State : public CState
{
protected:
	CSonic_Ground_State(CGameObject* pOwner) : CState{ pOwner } {}
	// 상태별 파라미터 추가
	struct StateParams
	{
		float fMoveSpeed = 5.0f;
		float fRotationSpeed = 2.0f;
		bool bCanJump = true;
		bool bCanDash = false;
	};

	StateParams m_tStateParams;
public:
	static CSonic_Ground_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput() override;
	void Exit() override;
	void SetMoveSpeed(float fSpeed) { m_tStateParams.fMoveSpeed = fSpeed; }
	float GetMoveSpeed() const { return m_tStateParams.fMoveSpeed; }

	void SetCanDash(bool bCanDash) { m_tStateParams.bCanDash = bCanDash; }
	bool GetCanDash() const { return m_tStateParams.bCanDash; }
};

class CSonic_Idle_State : public CSonic_Ground_State
{
private:
	CSonic_Idle_State(CGameObject* pOwner) : CSonic_Ground_State{ pOwner } {}
public:
	static CSonic_Idle_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput() override;
	void Exit() override;
};

class CSonic_Walk_State : public CSonic_Ground_State
{
private:
	CSonic_Walk_State(CGameObject* pOwner) : CSonic_Ground_State{ pOwner } {}
public:
	static CSonic_Walk_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput() override;
	void Exit() override;
};

class CSonic_Run_State : public CSonic_Ground_State
{
private:
	CSonic_Run_State(CGameObject* pOwner) : CSonic_Ground_State{ pOwner } {}
public:
	static CSonic_Run_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput() override;
	void Exit() override;
};

class CSonic_Boost_State : public CSonic_Ground_State
{
private:
	CSonic_Boost_State(CGameObject* pOwner) : CSonic_Ground_State{ pOwner } {}
public:
	static CSonic_Boost_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput() override;
	void Exit() override;
};

class CSonic_Brake_State : public CSonic_Ground_State
{
private:
	CSonic_Brake_State(CGameObject* pOwner) : CSonic_Ground_State{ pOwner } {}
public:
	static CSonic_Brake_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput() override;
	void Exit() override;
};

NS_END