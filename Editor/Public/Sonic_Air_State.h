#pragma once
#include "State.h"
class CSonic_Air_State : public CState
{
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput() override;
	void Exit() override;
	virtual void Free();
};

