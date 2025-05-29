#pragma once
#include "State.h"

NS_BEGIN(Editor)
class CSonic_Ground_State : public CState
{
public:
	static CSonic_Ground_State* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Enter() override;
	void Execute() override;
	void HandleInput() override;
	void Exit() override;
};

class CSonic_Idle_State : public CState
{
public:
	static CSonic_Idle_State* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Initialize();
	void Enter() override;
	void Execute() override;
	void HandleInput() override;
	void Exit() override;
};

class CSonic_Walk_State : public CState
{
public:
	static CSonic_Walk_State* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Initialize();
	void Enter() override;
	void Execute() override;
	void HandleInput() override;
	void Exit() override;
};

class CSonic_Run_State : public CState
{
public:
	static CSonic_Run_State* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Initialize();
	void Enter() override;
	void Execute() override;
	void HandleInput() override;
	void Exit() override;
};

class CSonic_Boost_State : public CState
{
public:
	static CSonic_Boost_State* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	HRESULT Initialize();
	void Enter() override;
	void Execute() override;
	void HandleInput() override;
	void Exit() override;
};

NS_END