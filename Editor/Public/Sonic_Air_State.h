#pragma once
#include "State.h"
class CSonic_Air_State : public CState
{
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
	virtual void Free();
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
