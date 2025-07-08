#pragma once
#include "State.h"
#include "Sonic_State.h"

NS_BEGIN(Editor)
class CSonic_Movement_State : public CSonic_State
{
private:
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

private:
	void ResolveCollisions(_vector vMovement, _float fTimeDelta);
private:
	EMovementState m_eMovementState = EMovementState::IDLE;
};
#pragma region Interact

#pragma endregion


NS_END