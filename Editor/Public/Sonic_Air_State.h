#pragma once
#include "Sonic_State.h"
class CSonic_Air_State : public CSonic_State
{
protected:
	CSonic_Air_State(CGameObject* pOwner) : CSonic_State{ pOwner } {}
	virtual ~CSonic_Air_State() = default;
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};


class CSonic_Jump_D_Loop : public CSonic_Air_State
{
private:
	CSonic_Jump_D_Loop(CGameObject* pOwner) : CSonic_Air_State{ pOwner } {}
	virtual ~CSonic_Jump_D_Loop() = default;
public:
	static CSonic_Jump_D_Loop* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};


class CSonic_Jump_D_S_Loop : public CSonic_Air_State
{
private:
	CSonic_Jump_D_S_Loop(CGameObject* pOwner) : CSonic_Air_State{ pOwner } {}
	virtual ~CSonic_Jump_D_S_Loop() = default;
public:
	static CSonic_Jump_D_S_Loop* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};


class CSonic_Jump_E : public CSonic_Air_State
{
private:
	CSonic_Jump_E(CGameObject* pOwner) : CSonic_Air_State{ pOwner } {}
	virtual ~CSonic_Jump_E() = default;
public:
	static CSonic_Jump_E* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};



class CSonic_Jump_E_BTL : public CSonic_Air_State
{
private:
	CSonic_Jump_E_BTL(CGameObject* pOwner) : CSonic_Air_State{ pOwner } {}
	virtual ~CSonic_Jump_E_BTL() = default;
public:
	static CSonic_Jump_E_BTL* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};


class CSonic_Jump_E_Injury : public CSonic_Air_State
{
private:
	CSonic_Jump_E_Injury(CGameObject* pOwner) : CSonic_Air_State{ pOwner } {}
	virtual ~CSonic_Jump_E_Injury() = default;
public:
	static CSonic_Jump_E_Injury* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};


class CSonic_Jump_E_Vir01 : public CSonic_Air_State
{
private:
	CSonic_Jump_E_Vir01(CGameObject* pOwner) : CSonic_Air_State{ pOwner } {}
	virtual ~CSonic_Jump_E_Vir01() = default;
public:
	static CSonic_Jump_E_Vir01* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};


class CSonic_Jump_S : public CSonic_Air_State
{
private:
	CSonic_Jump_S(CGameObject* pOwner) : CSonic_Air_State{ pOwner } {}
	virtual ~CSonic_Jump_S() = default;
public:
	static CSonic_Jump_S* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};


class CSonic_Jump_U : public CSonic_Air_State
{
private:
	CSonic_Jump_U(CGameObject* pOwner) : CSonic_Air_State{ pOwner } {}
	virtual ~CSonic_Jump_U() = default;
public:
	static CSonic_Jump_U* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
};





class CSonic_Jump_Boost_State : public CSonic_Air_State
{
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
	virtual void Free();
};
