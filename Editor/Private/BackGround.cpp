#include "BackGround.h"
#include "GameInstance.h"
#include "Shader.h"
#pragma region Constructor/Destructor
CBackGround::CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{

}

CBackGround::CBackGround(const CBackGround& Prototype)
	: CUIObject{ Prototype }
{

}

CBackGround* CBackGround::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBackGround* pInstance = new CBackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBackGround::Clone(void* pArg)
{
	CBackGround* pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBackGround::Clone(simdjson::ondemand::object& jsonData)
{
	CBackGround* pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Cloned : CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CBackGround::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBackGround::Initialize(void* pArg)
{
	BACKGROUND_DESC* pDesc = static_cast<BACKGROUND_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pVIBufferCom->Set_LocalSize(m_fSizeX, m_fSizeY);

	return S_OK;
}

HRESULT CBackGround::Initialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if(FAILED(Deserialize(jsonData)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackGround::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Texture_BackGround"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

void CBackGround::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
#pragma endregion

#pragma region Save&Load

HRESULT CBackGround::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CBackGround"] = data;
	outJson["CBackGround"]["PrototypeTag"] = "Prototype_GameObject_BackGround";
	return S_OK;
}

HRESULT CBackGround::Deserialize(simdjson::ondemand::object& jsonData)
{
	if(FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBackGround::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CBackGround";
	return S_OK;
}

HRESULT CBackGround::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma endregion

#pragma region Loops
void CBackGround::Priority_Update(_float fTimeDelta)
{

}

void CBackGround::Update(_float fTimeDelta)
{
	if(m_pShaderCom)
		m_pShaderCom->Update(fTimeDelta);
}

void CBackGround::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_UI, this);
}

HRESULT CBackGround::Render()
{
	if (!m_bVisible)
		return S_OK;
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix", m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("objectId", &m_oID, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResourceWithRegion(m_pShaderCom, "g_Texture", "g_TextureRegion", 0)))
		return E_FAIL;
	/*if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;*/

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}
#pragma endregion