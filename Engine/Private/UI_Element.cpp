#include "UI_Element.h"

CUI_Element::CUI_Element(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CUI_Element::CUI_Element(const CUI_Element& Prototype)
	: CGameObject{ Prototype }
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

void CUI_Element::Priority_Update(_float fTimeDelta)
{
	for (auto& i : m_Children)
		i.second->Priority_Update(fTimeDelta);
}

void CUI_Element::Update(_float fTimeDelta)
{
	for (auto& i : m_Children)
		i.second->Update(fTimeDelta);
}

void CUI_Element::Late_Update(_float fTimeDelta)
{
	for (auto& i : m_Children)
		i.second->Late_Update(fTimeDelta);
}

HRESULT CUI_Element::Pre_Render()
{
	return S_OK;
}

HRESULT CUI_Element::Render()
{
	if (FAILED(Pre_Render())) return E_FAIL;
	for (auto& i : m_Children)
		if(FAILED(i.second->Render())) return E_FAIL;
	if (FAILED(Post_Render())) return E_FAIL;
	return S_OK;
}

HRESULT CUI_Element::Post_Render()
{
	return S_OK;
}


void CUI_Element::Free()
{
	__super::Free();
}

