#pragma once
#include "State.h"
NS_BEGIN(Engine)
class ENGINE_DLL CGround_State : public CState
{
private:
	map<const _wstring, CState*> m_pSubStates;
};
NS_END