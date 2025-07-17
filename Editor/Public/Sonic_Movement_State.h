#pragma once
#include "State.h"
#include "Sonic_State.h"

NS_BEGIN(Editor)
class CSonic_Movement_State : public CSonic_State
{
public:
	enum class EMovementState
	{
		IDLE,
		WALK,
		RUN,
		DASH,
		BOOST,
		BRAKE,
		AVOID,
		SPRINGJUMP,
		QUICKSTEP,
		POLESPIN,
		PULLEY,
		RIDER,
		WALL,
		JUMP,
		DASHRING,
		END
	};
protected:
	CSonic_Movement_State(CGameObject* pOwner) : CSonic_State{ pOwner } {}
	virtual ~CSonic_Movement_State() = default;
public:
	static CSonic_Movement_State* Create(CGameObject* pOwner);
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;


private:
	void Idle(_float fTimeDelta);
	void Walk(_float fTimeDelta);
	void Run(_float fTimeDelta);
	void Dash(_float fTimeDelta);
	void Boost(_float fTimeDelta);
	void Brake(_float fTimeDelta);
	void Jump(_float fTimeDelta);
	void Rider(_float fTimeDelta);
	void Avoid(_float fTimeDelta);
	void QuickStep(_float fTimeDelta);
	void SpringJump(_float fTimeDelta);
	void DashRing(_float fTimeDelta);

#pragma region Interact
private:
	void Begin_Spline();
	void Update_SplineMovement(_float fTimeDelta);
	void Update_PositionOnSpline();
	void Determine_SplineDirection();
#pragma endregion

public:
	void Set_MovementState(EMovementState eMovementState) { m_eMovementState = eMovementState; }

private:
	void ResolveCollisions(_vector vMovement, _float fTimeDelta);
private:
	EMovementState m_eMovementState = EMovementState::IDLE;
};



NS_END