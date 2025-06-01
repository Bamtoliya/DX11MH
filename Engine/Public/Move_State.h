#pragma once
#include "State.h"
NS_BEGIN(Engine)
class CGameObject;
class CModel;
class ENGINE_DLL CMove_State : public CState
{
public:
	CMove_State(CGameObject* pGameObject, CModel* pModel);
	virtual ~CMove_State() = default;
	virtual void Free();
public:
	virtual void Enter() override;
	virtual void Execute() override;
	virtual void HandleInput() override;
	virtual void Exit() override;
};

class ENGINE_DLL CRun_State : public CState
{
public:
	CRun_State(CGameObject* pGameObject, CModel* pModel);
	virtual ~CRun_State() = default;
	virtual void Free();
public:
	virtual void Enter() override;
	virtual void Execute() override;
	virtual void HandleInput() override;
	virtual void Exit() override;
};


class ENGINE_DLL CWalk_State : public CState
{
public:
	CWalk_State(CGameObject* pGameObject, CModel* pModel);
	virtual ~CWalk_State() = default;
	virtual void Free();
public:
	virtual void Enter() override;
	virtual void Execute() override;
	virtual void HandleInput() override;
	virtual void Exit() override;
};


class ENGINE_DLL CJump_State : public CState
{
public:
	CJump_State(CGameObject* pGameObject, CModel* pModel);
	virtual ~CJump_State() = default;
	virtual void Free();
public:
	virtual void Enter() override;
	virtual void Execute() override;
	virtual void HandleInput() override;
	virtual void Exit() override;
};


NS_END