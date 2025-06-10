#pragma once
#include "Sonic_State.h"

NS_BEGIN(Editor)
class CSonic_Wall_State : public CSonic_State
{
protected:
	CSonic_Wall_State(CGameObject* pOwner) : CSonic_State{ pOwner } {}
	virtual ~CSonic_Wall_State() = default;
public:
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
};


class CSonic_WallRun_D_Loop : public CSonic_Wall_State
{
private:
	CSonic_WallRun_D_Loop(CGameObject* pOwner) : CSonic_Wall_State{ pOwner } {}
	virtual ~CSonic_WallRun_D_Loop() = default;
public:
	static CSonic_WallRun_D_Loop* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
};


NS_END