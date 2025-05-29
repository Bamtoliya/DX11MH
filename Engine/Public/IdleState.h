#pragma once
#include "State.h"
NS_BEGIN(Engine)
class ENGINE_DLL CIdleState : public CState
{
public:
	virtual void Enter() override;
	virtual void Execute() override;
	virtual void HandleInput() override;
	virtual void Exit() override;
};
NS_END