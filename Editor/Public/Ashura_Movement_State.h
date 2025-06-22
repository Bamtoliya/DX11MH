#pragma once
#include "State.h"

NS_BEGIN(Editor)
class CAshura_Movement_State : public CState
{
protected:
	CAshura_Movement_State(CGameObject* pOwner) : CState{ pOwner } {}
	virtual ~CAshura_Movement_State() = default;
public:
	
	static CAshura_Movement_State* Create(CGameObject* pOwner);
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
private:
	void Idle(_float fTimeDelta);

	
};

NS_END