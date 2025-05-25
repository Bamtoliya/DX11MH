#include "Sonic.h"

#include "GameInstance.h"

#pragma region Constructor / Destructor
CSonic::CSonic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CSonic::CSonic(const CSonic& Prototype)
	: CGameObject{ Prototype }
{

}

CSonic* CSonic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSonic* pInstance = new CSonic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSonic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSonic::Clone(void* pArg)
{
	CSonic* pInstance = new CSonic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSonic");
		Safe_Release(pInstance);
	}

	return pInstance;
}



HRESULT CSonic::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSonic::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};
	if (pArg != nullptr)
		memcpy(&Desc, pArg, sizeof(GAMEOBJECT_DESC));
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	if (nullptr == pArg)
	{
		lstrcpy(Desc.szName, TEXT("Sonic"));
	}
	else
	{
		lstrcpy(Desc.szName, static_cast<GAMEOBJECT_DESC*>(pArg)->szName);
	}

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_Animation(0, true);

	return S_OK;
}

HRESULT CSonic::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::MAP), TEXT("Prototype_Component_Model_Sonic"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	return S_OK;
}

void CSonic::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
#pragma endregion

HRESULT CSonic::Serialize(json& outJson)
{
	return S_OK;
}

HRESULT CSonic::Deserialize(const json& inData)
{
	return S_OK;
}

HRESULT CSonic::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CSonic";
	return S_OK;
}

HRESULT CSonic::Prototype_Deserialize(simdjson::ondemand::object & jsonData)
{
	return S_OK;
}

#pragma region Loops
void CSonic::Priority_Update(_float fTimeDelta)
{

}

void CSonic::Update(_float fTimeDelta)
{
	if (m_pModelCom->Play_Animation(fTimeDelta))
		int a = 10;
}

void CSonic::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CSonic::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("objectId", &m_oID, sizeof(_uint))))
		return E_FAIL;


	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_Light(0);
	if (pLightDesc == nullptr)
		return S_OK;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSonic::Render()
{
	if (!m_bVisible)
		return S_OK;
	Bind_ShaderResources();

	_uint		iNumMesh = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMesh; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		m_pModelCom->Bind_Bone_Matrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}
#pragma endregion