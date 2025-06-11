#pragma once
#include "State.h"
#include "Sonic_State.h"

NS_BEGIN(Editor)
class CSonic_Movement_State : public CSonic_State
{
protected:
	CSonic_Movement_State(CGameObject* pOwner) : CSonic_State{ pOwner } {}
	virtual ~CSonic_Movement_State() = default;
public:
	void Idle();
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
};


#pragma region Grounds
class CSonic_Idle_State : public CSonic_Movement_State
{
private:
	CSonic_Idle_State(CGameObject* pOwner) : CSonic_Movement_State{ pOwner } {}
	virtual ~CSonic_Idle_State() = default;
public:
	static CSonic_Idle_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Walk_State : public CSonic_Movement_State
{
private:
	CSonic_Walk_State(CGameObject* pOwner) : CSonic_Movement_State{ pOwner } {}
	virtual ~CSonic_Walk_State() = default;
public:
	static CSonic_Walk_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;

};

class CSonic_Run_State : public CSonic_Movement_State
{
private:
	CSonic_Run_State(CGameObject* pOwner) : CSonic_Movement_State{ pOwner } {}
	virtual ~CSonic_Run_State() = default;
public:
	static CSonic_Run_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;

};

class CSonic_Boost_State : public CSonic_Movement_State
{
private:
	CSonic_Boost_State(CGameObject* pOwner) : CSonic_Movement_State{ pOwner } {}
	virtual ~CSonic_Boost_State() = default;
public:
	static CSonic_Boost_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;

};

class CSonic_Brake_State : public CSonic_Movement_State
{
protected:
	CSonic_Brake_State(CGameObject* pOwner) : CSonic_Movement_State{ pOwner } {}
	virtual ~CSonic_Brake_State() = default;
public:
	static CSonic_Brake_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};


class CSonic_Brake_Cliff_State : public CSonic_Brake_State
{
private:
	CSonic_Brake_Cliff_State(CGameObject* pOwner) : CSonic_Brake_State{ pOwner } {}
	virtual ~CSonic_Brake_Cliff_State() = default;
public:
	static CSonic_Brake_Cliff_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};


class CSonic_Brake_Neutral_State : public CSonic_Brake_State
{
private:
	CSonic_Brake_Neutral_State(CGameObject* pOwner) : CSonic_Brake_State{ pOwner } {}
	virtual ~CSonic_Brake_Neutral_State() = default;
public:
	static CSonic_Brake_Neutral_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Brake_QuickTurn_State : public CSonic_Brake_State
{
private:
	CSonic_Brake_QuickTurn_State(CGameObject* pOwner) : CSonic_Brake_State{ pOwner } {}
	virtual ~CSonic_Brake_QuickTurn_State() = default;
public:
	static CSonic_Brake_QuickTurn_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Brake_Slope_State : public CSonic_Brake_State
{
private:
	CSonic_Brake_Slope_State(CGameObject* pOwner) : CSonic_Brake_State{ pOwner } {}
	virtual ~CSonic_Brake_Slope_State() = default;
public:
	static CSonic_Brake_Slope_State* Create(CGameObject* pOwner);
	virtual HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};
#pragma endregion

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