#pragma once
#include "State.h"
NS_BEGIN(Engine)
class CGameObject;
class CModel;
class IBlendNode;
class ENGINE_DLL CIdleState : public CState
{
public:
	CIdleState(CGameObject* pGameObject, CModel* pModel);
	virtual ~CIdleState() = default;
	virtual void Free();
public:
	virtual void Enter() override;
	virtual void Execute() override;
	virtual void HandleInput() override;
	virtual void Exit() override;

private:
	CGameObject*	m_pOwner = { nullptr };
	CModel*			m_pModelCom = { nullptr };
	IBlendNode*		m_pIdleAnimationNode = { nullptr };
};
NS_END