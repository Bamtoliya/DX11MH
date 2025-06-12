#pragma once
#include "State.h"
#include "Sonic_State.h"

NS_BEGIN(Editor)
class CSonic_Movement_State : public CSonic_State
{
private:
	enum class EMovementState
	{
		IDLE,
		WALK,
		RUN,
		DASH,
		BOOST,
		BRAKE,
		AVOID,
		SPRINGJUMP,
		QUICKSTEP,
		POLESPIN,
		PULLEY,
		RIDER,
		WALL,
		END
	};
protected:
	CSonic_Movement_State(CGameObject* pOwner) : CSonic_State{ pOwner } {}
	virtual ~CSonic_Movement_State() = default;
public:
	static CSonic_Movement_State* Create(CGameObject* pOwner);
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
private:
	void Idle(_float fTimeDelta);
	void Walk(_float fTimeDelta);
	void Run(_float fTimeDelta);
	void Dash(_float fTimeDelta);
	void Boost(_float fTimeDelta);
	void Brake(_float fTimeDelta);
private:
	EMovementState m_eMovementState = EMovementState::IDLE;
};
#pragma region Air

class CSonic_Jump_S_State : public CSonic_Movement_State
{
private:
	CSonic_Jump_S_State(CGameObject* pOwner) : CSonic_Movement_State{ pOwner } {}
	virtual ~CSonic_Jump_S_State() = default;
public:
	static CSonic_Jump_S_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Jump_U_State : public CSonic_Movement_State
{
private:
	CSonic_Jump_U_State(CGameObject* pOwner) : CSonic_Movement_State{ pOwner } {}
	virtual ~CSonic_Jump_U_State() = default;
public:
	static CSonic_Jump_U_State* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Jump_D_Loop_State : public CSonic_Movement_State
{
private:
	CSonic_Jump_D_Loop_State(CGameObject* pOwner) : CSonic_Movement_State{ pOwner } {}
	virtual ~CSonic_Jump_D_Loop_State() = default;
public:
	static CSonic_Jump_D_Loop_State* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Jump_D_S_Loop_State : public CSonic_Movement_State
{
private:
	CSonic_Jump_D_S_Loop_State(CGameObject* pOwner) : CSonic_Movement_State{ pOwner } {}
	virtual ~CSonic_Jump_D_S_Loop_State() = default;
public:
	static CSonic_Jump_D_S_Loop_State* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};


class CSonic_Jump_E_State : public CSonic_Movement_State
{
private:
	CSonic_Jump_E_State(CGameObject* pOwner) : CSonic_Movement_State{ pOwner } {}
	virtual ~CSonic_Jump_E_State() = default;
public:
	static CSonic_Jump_E_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Jump_E_BTL_State : public CSonic_Movement_State
{
private:
	CSonic_Jump_E_BTL_State(CGameObject* pOwner) : CSonic_Movement_State{ pOwner } {}
	virtual ~CSonic_Jump_E_BTL_State() = default;
public:
	static CSonic_Jump_E_BTL_State* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};


class CSonic_Jump_E_Injury_State : public CSonic_Movement_State
{
private:
	CSonic_Jump_E_Injury_State(CGameObject* pOwner) : CSonic_Movement_State{ pOwner } {}
	virtual ~CSonic_Jump_E_Injury_State() = default;
public:
	static CSonic_Jump_E_Injury_State* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};



class CSonic_Jump_E_VIR01_State : public CSonic_Movement_State
{
private:
	CSonic_Jump_E_VIR01_State(CGameObject* pOwner) : CSonic_Movement_State{ pOwner } {}
	virtual ~CSonic_Jump_E_VIR01_State() = default;
public:
	static CSonic_Jump_E_VIR01_State* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};



class CSonic_Jump_E_VIR02_State : public CSonic_Movement_State
{
private:
	CSonic_Jump_E_VIR02_State(CGameObject* pOwner) : CSonic_Movement_State{ pOwner } {}
	virtual ~CSonic_Jump_E_VIR02_State() = default;
public:
	static CSonic_Jump_E_VIR02_State* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};




#pragma endregion

#pragma region Interact

#pragma endregion


NS_END