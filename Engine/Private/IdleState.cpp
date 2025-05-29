#include "IdleState.h"
#include "Model.h"
#include "GameObject.h"
#include "GameInstance.h"

CIdleState::CIdleState(CGameObject* pGameObject, CModel* pModel)
	: m_pOwner { pGameObject }
	, m_pModelCom { pModel }
{
	Safe_AddRef(m_pOwner);
	Safe_AddRef(m_pModelCom);
}

void CIdleState::Enter()
{
	if (m_pModelCom)
	{
		//m_pModelCom->Set_Animation(0, true);
	}
}

void CIdleState::Execute()
{

}

void CIdleState::HandleInput()
{
}

void CIdleState::Exit()
{
}


void CIdleState::Free()
{
	Safe_Release(m_pOwner);
	Safe_Release(m_pModelCom);
}