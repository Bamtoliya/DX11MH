#include "GameObject_Skybox.h"
#include "GameInstance.h"
#include "VIBuffer_Cube.h"

#pragma region Constructor/Destructor
CGameObject_Skybox::CGameObject_Skybox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CGameObject_Skybox::CGameObject_Skybox(const CGameObject_Skybox& Prototype)
	: CGameObject{ Prototype }
{
}

CGameObject_Skybox* CGameObject_Skybox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameObject_Skybox* pInstance = new CGameObject_Skybox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGameObject_Skybox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGameObject_Skybox::Clone(void* pArg)
{
	CGameObject_Skybox* pInstance = new CGameObject_Skybox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGameObject_Skybox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CGameObject_Skybox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject_Skybox::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};
	lstrcpy(Desc.szName, TEXT("Skybox"));

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject_Skybox::Ready_Components()
{
	/* For.Com_Texture */
	m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, L"../../Assets/Textures/Skybox/XYZ.dds", 1);
	m_Components.emplace(L"Com_Texture", m_pTextureCom);
	Safe_AddRef(m_pTextureCom);
	/* For.Com_VIBuffer */
	m_pVIBufferCom = CVIBuffer_Cube::Create(m_pDevice, m_pContext);
	m_Components.emplace(L"Com_VIBuffer", m_pVIBufferCom);
	Safe_AddRef(m_pVIBufferCom);
	/* For.Com_Shader */
	m_pShaderCom = CShader::Create(m_pDevice, m_pContext, L"../../Assets/ShaderFiles/Shader_VtxCubeTex.hlsl", VTXCUBETEX::Elements, VTXCUBETEX::iNumElements, "VTXCUBETEX");
	m_Components.emplace(L"Com_Shader", m_pShaderCom);
	Safe_AddRef(m_pShaderCom);
	/*if (FAILED(__super::Add_Component(ENUM_CLASS(0), TEXT("Prototype_Component_Shader_Cube"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;*/

	return S_OK;
}

void CGameObject_Skybox::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}

#pragma endregion

#pragma region Save&Load

HRESULT CGameObject_Skybox::Serialize(json& outJson)
{
	return S_OK;
}

HRESULT CGameObject_Skybox::Deserialize(const json& inData)
{
	return S_OK;
}

HRESULT CGameObject_Skybox::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CGameObject_Skybox";
	return S_OK;
}

HRESULT CGameObject_Skybox::Prototype_Deserialize(const simdjson::ondemand::object& jsonData)
{

	return S_OK;
}

#pragma endregion

#pragma region Loops
void CGameObject_Skybox::Priority_Update(_float fTimeDelta)
{
}

void CGameObject_Skybox::Update(_float fTimeDelta)
{
	const _float4* pCameraWorldPos = m_pGameInstance->Get_CamPosition();
	if (pCameraWorldPos != nullptr)
	{
		// _float4를 XMVECTOR로 로드하여 설정
		_vector vCamPos = XMLoadFloat4(pCameraWorldPos);
		m_pTransformCom->Set_State(STATE::POSITION, vCamPos);
	}
}

void CGameObject_Skybox::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_PRIORITY, this);
}

HRESULT CGameObject_Skybox::Render()
{

	if (!m_bVisible)
		return S_OK;
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	/* dx9 : 장치에 뷰, 투영행렬을 저장해두면 렌더링시 알아서 정점에 Transform해주었다. */
	/* dx11 : 셰이더에 뷰, 투영행렬을 저장해두고 우리가 직접 변환해주어야한다. */
	if (FAILED(m_pShaderCom->Bind_RawValue("objectId", &m_iSkyboxID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TextureCube", 0)))
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