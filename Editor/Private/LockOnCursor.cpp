#include "LockOnCursor.h"
#include "GameInstance.h"
#include "Shader.h"
#pragma region Constructor/Destructor
CLockOnCursor::CLockOnCursor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CLockOnCursor::CLockOnCursor(const CLockOnCursor& Prototype)
	: CGameObject{ Prototype }
{

}

CLockOnCursor* CLockOnCursor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLockOnCursor* pInstance = new CLockOnCursor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLockOnCursor::Clone(void* pArg)
{
	CLockOnCursor* pInstance = new CLockOnCursor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLockOnCursor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLockOnCursor::Clone(simdjson::ondemand::object& jsonData)
{
	CLockOnCursor* pInstance = new CLockOnCursor(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Cloned : CLockOnCursor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CLockOnCursor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLockOnCursor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Late_Initialize()))
		return E_FAIL;

	m_pTextureCom->Set_TextureRegion(true);
	m_pTextureCom->Set_TextureRegionPixels(881, 881, 1012, 1012);
	m_pVIBufferCom->Set_LocalSize(100.f, 100.f);

	

	return S_OK;
}

HRESULT CLockOnCursor::Initialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Deserialize(jsonData)))
		return E_FAIL;

	if (FAILED(Late_Initialize()))
		return E_FAIL;

	m_pTextureCom->Set_TextureRegion(true);
	m_pTextureCom->Set_TextureRegionPixels(881, 881, 1012, 1012);
	m_pVIBufferCom->Set_LocalSize(100.f, 100.f);

	return S_OK;
}

HRESULT CLockOnCursor::Ready_Components()
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
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Texture_LockOnCursor2"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;
	

	return S_OK;
}

void CLockOnCursor::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
#pragma endregion

#pragma region Save&Load

HRESULT CLockOnCursor::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CLockOnCursor"] = data;
	outJson["CLockOnCursor"]["PrototypeTag"] = "Prototype_GameObject_LockOnCursor";
	return S_OK;
}

HRESULT CLockOnCursor::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLockOnCursor::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CLockOnCursor";
	return S_OK;
}

HRESULT CLockOnCursor::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma endregion

#pragma region Loops
void CLockOnCursor::Priority_Update(_float fTimeDelta)
{

}

void CLockOnCursor::Update(_float fTimeDelta)
{
	CFollow_Camera* pCamera = dynamic_cast<CFollow_Camera*>(m_pGameInstance->Get_Current_Camera());
	if (pCamera)
	{
		pCamera->Set_LockOnCursor(this);
	}
	if (m_pShaderCom)
		m_pShaderCom->Update(fTimeDelta);
}

void CLockOnCursor::Late_Update(_float fTimeDelta)
{
	// 빌보드 처리: 항상 카메라를 바라보도록 설정
	CTransform* pCameraTransform = static_cast<CTransform*>(m_pGameInstance->Get_Current_Camera()->Get_OwnerGameObject()->Get_Component(g_strTransformTag));
	if (pCameraTransform)
	{
		_vector vCamPos = pCameraTransform->Get_State(STATE::POSITION);
		m_pTransformCom->LookAt(vCamPos);
	}

	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_UI, this);
}

HRESULT CLockOnCursor::Render()
{
	if (!m_bVisible)
		return S_OK;
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix", m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("objectId", &m_oID, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResourceWithRegion(m_pShaderCom, "g_Texture", "g_TextureRegion", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}
#pragma endregion