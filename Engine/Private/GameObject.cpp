#include "GameObject.h"

CGameObject::CGameObject(ID3D11Device* m_pDevice, ID3D11DeviceContext* m_pContext)
	: m_pDevice 	{ pDevice }
	, m_pContext	 { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject& Prototype)
	: m_pDevice 	{ Prototype.m_pDevice }
	, m_pContext 	{ Prototype.m_pContext }
{
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{
	return S_OK;
}

void CGameObject::Priority_Update(_float fTimeDelta)
{
}

void CGameObject::Update(_float fTimeDelta)
{
}

void CGameObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CGameObject::Pre_Render()
{
	return S_OK;
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

HRESULT CGameObject::Post_Render()
{
	return S_OK;
}

HRESULT CGameObject::Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg)
{
	return S_OK;
}

CGameObject* CGameObject::Clone(void* pArg)
{
	return nullptr;
}

void CGameObject::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
