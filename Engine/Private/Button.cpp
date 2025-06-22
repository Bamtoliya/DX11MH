#include "Button.h"
#pragma region Constructor/Destructor
CButton::CButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Element{ pDevice, pContext }
{
}

CButton::CButton(const CButton& Prototype)
	: CUI_Element{ Prototype }
{
}

HRESULT CButton::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CButton::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
	}
	return S_OK;
}

HRESULT CButton::Ready_Components()
{
	return S_OK;
}

void CButton::Free()
{
	__super::Free();
}

#pragma endregion
#pragma region Update
void CButton::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CButton::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CButton::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}
#pragma endregion
#pragma region Render
HRESULT CButton::Pre_Render()
{
	return S_OK;
}

HRESULT CButton::Render()
{
	if (FAILED(Pre_Render())) return E_FAIL;
	__super::Render();
	if (FAILED(Post_Render())) return E_FAIL;
	return S_OK;
}

HRESULT CButton::Post_Render()
{
	return S_OK;
}

#pragma endregion