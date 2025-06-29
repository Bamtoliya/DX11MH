#include "Level_Stage01.h"

CLevel_Stage01::CLevel_Stage01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Stage01::Initialize()
{
	return S_OK;
}

void CLevel_Stage01::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Stage01::Render()
{
	return S_OK;
}

void CLevel_Stage01::Free()
{
}
