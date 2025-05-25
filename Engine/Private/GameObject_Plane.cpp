#pragma once
#include "GameInstance.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "GameObject_Plane.h"

CGameObject_Plane::CGameObject_Plane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{
}

CGameObject_Plane::CGameObject_Plane(const CGameObject_Plane& Prototype)
	: CGameObject { Prototype }
{
}

CGameObject_Plane* CGameObject_Plane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameObject_Plane* pInstance = new CGameObject_Plane(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGameObject_Plane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGameObject_Plane::Clone(void* pArg)
{
	CGameObject_Plane* pInstance = new CGameObject_Plane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGameObject_Plane");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CGameObject* CGameObject_Plane::Clone(simdjson::ondemand::object& jsonData)
{
	CGameObject_Plane* pInstance = new CGameObject_Plane(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Cloned : CGameObject_Plane");
		Safe_Release(pInstance);
	}

	return pInstance;
}



HRESULT CGameObject_Plane::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject_Plane::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject_Plane::Initialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Deserialize(jsonData);

	return S_OK;
}

HRESULT CGameObject_Plane::Ready_Components()
{
	return S_OK;
}

void CGameObject_Plane::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}

#pragma region  Loops
void CGameObject_Plane::Priority_Update(_float fTimeDelta)
{
}

void CGameObject_Plane::Update(_float fTimeDelta)
{
}

void CGameObject_Plane::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_PRIORITY, this);
}

HRESULT CGameObject_Plane::Pre_Render()
{
	return S_OK;
}

HRESULT CGameObject_Plane::Render()
{
	Pre_Render();
	//if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ViewMatrix)))
	//	return E_FAIL;
	///*if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
	//	return E_FAIL;*/

	//if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Begin(0)))
	//	return E_FAIL;

	//if (FAILED(m_pVIBufferCom->Bind_Buffers()))
	//	return E_FAIL;

	//if (FAILED(m_pVIBufferCom->Render()))
	//	return E_FAIL;
	Post_Render();
	return S_OK;
}

HRESULT CGameObject_Plane::Post_Render()
{
	return S_OK;
}
#pragma endregion




