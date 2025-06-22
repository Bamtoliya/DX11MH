#include "UI_Element.h"
#pragma region Constructor/Destructor
CUI_Element::CUI_Element(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CUI_Element::CUI_Element(const CUI_Element& Prototype)
	: CGameObject{ Prototype }
	, m_tDesc { Prototype.m_tDesc }
{
}

HRESULT CUI_Element::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Element::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
	}
	return S_OK;
}

HRESULT CUI_Element::Ready_Components()
{
	return S_OK;
}

void CUI_Element::Free()
{
	__super::Free();
}

#pragma endregion
#pragma region Update
void CUI_Element::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CUI_Element::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CUI_Element::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}
#pragma endregion
#pragma region Render
HRESULT CUI_Element::Pre_Render()
{
	return S_OK;
}

HRESULT CUI_Element::Render()
{
	if (FAILED(Pre_Render())) return E_FAIL;
	__super::Render();
	if (FAILED(Post_Render())) return E_FAIL;
	return S_OK;
}

HRESULT CUI_Element::Post_Render()
{
	return S_OK;
}

#pragma endregion