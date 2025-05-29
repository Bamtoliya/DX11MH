#include "Sonic_Ground_State.h"

using namespace Editor;

#pragma region Ground
CSonic_Ground_State* CSonic_Ground_State::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return new CSonic_Ground_State();
}

void CSonic_Ground_State::Enter()
{
}

void CSonic_Ground_State::Execute()
{
}

void CSonic_Ground_State::HandleInput()
{
}

void CSonic_Ground_State::Exit()
{
}

#pragma endregion

#pragma region Idle
CSonic_Idle_State* CSonic_Idle_State::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return new CSonic_Idle_State();
}

void CSonic_Idle_State::Enter()
{
}

void CSonic_Idle_State::Execute()
{
}

void CSonic_Idle_State::HandleInput()
{
}

void CSonic_Idle_State::Exit()
{
}

#pragma endregion


#pragma region Walk
CSonic_Walk_State* CSonic_Walk_State::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return new CSonic_Walk_State();
}

void CSonic_Walk_State::Enter()
{
}

void CSonic_Walk_State::Execute()
{

}

void CSonic_Walk_State::HandleInput()
{
}

void CSonic_Walk_State::Exit()
{
}
#pragma endregion

#pragma region Run
CSonic_Run_State* CSonic_Run_State::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return new CSonic_Run_State();
}

void CSonic_Run_State::Enter()
{
}

void CSonic_Run_State::Execute()
{
}

void CSonic_Run_State::HandleInput()
{
}

void CSonic_Run_State::Exit()
{
}

#pragma endregion
#pragma region Boost
CSonic_Boost_State* CSonic_Boost_State::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return new CSonic_Boost_State();
}

void CSonic_Boost_State::Enter()
{
}

void CSonic_Boost_State::Execute()
{
}

void CSonic_Boost_State::HandleInput()
{
}

void CSonic_Boost_State::Exit()
{
}


#pragma endregion
