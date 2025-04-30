#include "Picking_System.h"
#include "GameInstance.h"

#pragma region Constructor/Destructor
CPicking_System::CPicking_System(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

CPicking_System* CPicking_System::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPicking_System* pInstance = new CPicking_System(pDevice, pContext);
	if (FAILED(pInstance->Initialize()))
		return nullptr;
	return pInstance;
}

void CPicking_System::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
#pragma endregion

HRESULT CPicking_System::Initialize()
{
	return S_OK;
}

void CPicking_System::Priority_Update(_float fTimeDelta)
{

}

void CPicking_System::Update(_float fTimeDelta)
{

}

void CPicking_System::Late_Update(_float fTimeDelta)
{

}
