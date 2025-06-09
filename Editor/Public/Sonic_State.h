#pragma once
#include "State.h"

NS_BEGIN(Editor)
class CSonic_State : public CState
{
protected:
	CSonic_State(CGameObject* pOwner) : CState{ pOwner } {}
	virtual ~CSonic_State() = default;
public:
	// 상태별 파라미터 추가
	struct StateParams
	{
		float fMoveSpeed = 5.0f;
		float fRotationSpeed = 2.0f;
		bool bCanJump = true;
		bool bCanDash = false;
		_float3 vMoveDirection = {};
	};
	HRESULT Initialize() override;
	void Enter() override;
	void Execute(_float fTimeDelta) override;
	void HandleInput(_float fTimeDelta) override;
	void Exit() override;
protected:
	StateParams m_tStateParams = {};
};

NS_END