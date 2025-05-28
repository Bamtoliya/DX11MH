#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class ENGINE_DLL CState abstract :  public CBase
{
protected:

public:
	virtual void Enter() = 0;
	virtual void Execute() = 0;
	virtual void HandleInput() = 0;
	virtual void Exit() = 0;
};
NS_END