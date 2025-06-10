#pragma once
#include "State.h"

NS_BEGIN(Editor)
class CSonic_State : public CState
{
protected:
	CSonic_State(CGameObject* pOwner) : CState{ pOwner } {}
	virtual ~CSonic_State() = default;
public:
	HRESULT Initialize() override { return S_OK; }
	void Enter() override {}
	void Execute(_float fTimeDelta) override {}
	void HandleInput(_float fTimeDelta) override {}
	void Exit() override {}
};

NS_END