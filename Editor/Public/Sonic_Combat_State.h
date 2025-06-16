#pragma once
#include "Sonic_State.h"

NS_BEGIN(Editor)
class CSonic_Combat_State : public CSonic_State
{
protected:
	CSonic_Combat_State(CGameObject* pOwner) : CSonic_State{ pOwner } { m_bLoop = false; }
	virtual ~CSonic_Combat_State() = default;
public:
	static CSonic_Combat_State* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
};

class CSonic_Combo_Accel_01_State : public CSonic_Combat_State
{
private:
	CSonic_Combo_Accel_01_State(CGameObject* pOwner) : CSonic_Combat_State{ pOwner } {}
	virtual ~CSonic_Combo_Accel_01_State() = default;
public:
	static CSonic_Combo_Accel_01_State* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Accel_02_State : public CSonic_Combat_State
{
private:
	CSonic_Combo_Accel_02_State(CGameObject* pOwner) : CSonic_Combat_State{ pOwner } {}
	virtual ~CSonic_Combo_Accel_02_State() = default;
public:
	static CSonic_Combo_Accel_02_State* Create(CGameObject* pOwner);
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Accel_Kick01_State : public CSonic_Combat_State
{
private:
	CSonic_Combo_Accel_Kick01_State(CGameObject* pOwner) : CSonic_Combat_State{ pOwner } {}
	virtual ~CSonic_Combo_Accel_Kick01_State() = default;
public:
	static CSonic_Combo_Accel_Kick01_State* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Accel_Kick02_State : public CSonic_Combat_State
{
private:
	CSonic_Combo_Accel_Kick02_State(CGameObject* pOwner) : CSonic_Combat_State{ pOwner } {}
	virtual ~CSonic_Combo_Accel_Kick02_State() = default;
public:
	static CSonic_Combo_Accel_Kick02_State* Create(CGameObject* pOwner);
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};
class CSonic_Combo_Accel_Kick03_State : public CSonic_Combat_State
{
private:
	CSonic_Combo_Accel_Kick03_State(CGameObject* pOwner) : CSonic_Combat_State{ pOwner } {}
	virtual ~CSonic_Combo_Accel_Kick03_State() = default;
public:
	static CSonic_Combo_Accel_Kick03_State* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Accel_Kick04_State : public CSonic_Combat_State
{
private:
	CSonic_Combo_Accel_Kick04_State(CGameObject* pOwner) : CSonic_Combat_State{ pOwner } {}
	virtual ~CSonic_Combo_Accel_Kick04_State() = default;
public:
	static CSonic_Combo_Accel_Kick04_State* Create(CGameObject* pOwner);
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Accel_Kick05_State : public CSonic_Combat_State
{
private:
	CSonic_Combo_Accel_Kick05_State(CGameObject* pOwner) : CSonic_Combat_State{ pOwner } {}
	virtual ~CSonic_Combo_Accel_Kick05_State() = default;
public:
	static CSonic_Combo_Accel_Kick05_State* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Accel_Off_D_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Accel_Off_L_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Accel_Off_R_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Accel_Off_U_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Accel_On_D_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Accel_On_L_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Accel_On_R_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Accel_On_U_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Accel_Punch01_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Accel_Punch02_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Accel_Punch03_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Accel_Punch04_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Accel_Punch05_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Aerial_Finish_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Avoid01_Finish_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Behind_Finish_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_ChargeAtk01_End_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_ChargeAtk01_Finish_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_ChargeAtk01_Loop_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};
class CSonic_Combo_ChargeAtk01_Start_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Cons_Strike_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_CrossSlash01_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_CrossSlash01_End_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_CrossSlash01_Loop_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_CyLoop01_Attack_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_CyLoop01_Start_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_DropDash01_Finish_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_DropKick_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_HomingShot01_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_JumpKick_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Kick01_State : public CSonic_Combat_State
{
private:
	CSonic_Combo_Kick01_State(CGameObject* pOwner) : CSonic_Combat_State{ pOwner } {}
	virtual ~CSonic_Combo_Kick01_State() = default;
public:
	static CSonic_Combo_Kick01_State* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Kick02_State : public CSonic_Combat_State
{
private:
	CSonic_Combo_Kick02_State(CGameObject* pOwner) : CSonic_Combat_State{ pOwner } {}
	virtual ~CSonic_Combo_Kick02_State() = default;
public:
	static CSonic_Combo_Kick02_State* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_OneTwo_State : public CSonic_Combat_State
{
private:
	CSonic_Combo_OneTwo_State(CGameObject* pOwner) : CSonic_Combat_State{ pOwner } {}
	virtual ~CSonic_Combo_OneTwo_State() = default;
public:
	static CSonic_Combo_OneTwo_State* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Punch01_State : public CSonic_Combat_State
{
private:
	CSonic_Combo_Punch01_State(CGameObject* pOwner) : CSonic_Combat_State{ pOwner } {}
	virtual ~CSonic_Combo_Punch01_State() = default;
public:
	static CSonic_Combo_Punch01_State* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Punch02_State : public CSonic_Combat_State
{
private:
	CSonic_Combo_Punch02_State(CGameObject* pOwner) : CSonic_Combat_State{ pOwner } {}
	virtual ~CSonic_Combo_Punch02_State() = default;
public:
	static CSonic_Combo_Punch02_State* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Pursuit_Finish_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Pursuit_Loop_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Pursuit_Start_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Quickstep01_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Sliding01_Finish_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};


class CSonic_Combo_Stomp_Finish_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Summersault_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Tornado_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Combo_Uppercut_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};


class CSonic_Damage_Backside_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Damage_Blow_Loop_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;

};

class CSonic_Damage_Down_F_Loop_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Damage_Down_F_Return_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Damage_Down_Loop_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Damage_Down_Return_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Damage_Down_Return_BTL_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Damage_Down_Return_VIR01_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Damage_Down_Return_VIR02_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Damage_Down_Start_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Damage_F_Start_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Damage_Guard_Walk_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Damage_Magma01_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Damage_Magma01_End_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Damage_Magma01_Loop_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};


class CSonic_Damage_Magma01_Start_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};

class CSonic_Damage_Return_State : public CSonic_Combat_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};
NS_END