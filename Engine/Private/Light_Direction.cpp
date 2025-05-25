#include "Light_Direction.h"

#include "Light.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"

#pragma region Constructor/Destructor
CLight_Direction::CLight_Direction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CLight_Direction::CLight_Direction(const CLight_Direction& Prototype)
	: CGameObject{ Prototype }
{
}

CLight_Direction* CLight_Direction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLight_Direction* pInstance = new CLight_Direction(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLight_Direction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CLight_Direction* CLight_Direction::Clone(void* pArg)
{
	CLight_Direction* pInstance = new CLight_Direction(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLight_Direction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CLight_Direction* CLight_Direction::Clone(simdjson::ondemand::object& jsonData)
{
	CLight_Direction* pInstance = new CLight_Direction(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Cloned : CLight_Direction");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CLight_Direction::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLight_Direction::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};
	lstrcpy(Desc.szName, TEXT("Point_Direction"));
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	return S_OK;
}

HRESULT CLight_Direction::Initialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Deserialize(jsonData);
	return S_OK;
}

HRESULT CLight_Direction::Ready_Components()
{
	m_pLightCom = CLight::Create(m_pDevice, m_pContext);
	m_Components.emplace(L"Com_Light", m_pLightCom);
	m_pLightCom->Set_OwnerGameObject(this);
	LIGHT_DESC tDesc = {};
	tDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	tDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	tDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	tDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	tDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	m_pLightCom->Initialize(&tDesc);
	Safe_AddRef(m_pLightCom);
	/* For.Com_Texture */
	m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, L"../../Assets/Textures/Light.png", 1);
	m_Components.emplace(L"Com_Texture", m_pTextureCom);
	m_pTextureCom->Set_OwnerGameObject(this);
	Safe_AddRef(m_pTextureCom);
	/* For.Com_VIBuffer */
	m_pVIBufferCom = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	m_Components.emplace(L"Com_VIBuffer", m_pVIBufferCom);
	m_pVIBufferCom->Set_OwnerGameObject(this);
	Safe_AddRef(m_pVIBufferCom);

	//여기서 만드는게아니라 나중엔 프로토타입에서 찾아서 대체해주기
	/* For.Com_Shader */
	m_pShaderCom = CShader::Create(m_pDevice, m_pContext, L"../../Assets/ShaderFiles/Shader_VtxPosTex.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements, "VTXPOSTEX");
	m_Components.emplace(L"Com_Shader", m_pShaderCom);
	m_pShaderCom->Set_OwnerGameObject(this);
	Safe_AddRef(m_pShaderCom);
	/*if (FAILED(__super::Add_Component(ENUM_CLASS(0), TEXT("Prototype_Component_Shader_Cube"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;*/
	return S_OK;
}


void CLight_Direction::Free()
{
	__super::Free();
	Safe_Release(m_pLightCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}

#pragma endregion

#pragma region Save&Load

HRESULT CLight_Direction::Serialize(json& outJson)
{
	return S_OK;
}

HRESULT CLight_Direction::Deserialize(simdjson::ondemand::object & jsonData)
{
	return S_OK;
}

HRESULT CLight_Direction::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CLight_Direction";
	return S_OK;
}

HRESULT CLight_Direction::Prototype_Deserialize(simdjson::ondemand::object & jsonData)
{

	return S_OK;
}

#pragma endregion

#pragma region Loops
void CLight_Direction::Priority_Update(_float fTimeDelta)
{
}

void CLight_Direction::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CLight_Direction::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_Light(m_pLightCom);
}

HRESULT CLight_Direction::Pre_Render()
{
	return S_OK;
}

HRESULT CLight_Direction::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	/* dx9 : 장치에 뷰, 투영행렬을 저장해두면 렌더링시 알아서 정점에 Transform해주었다. */
	/* dx11 : 셰이더에 뷰, 투영행렬을 저장해두고 우리가 직접 변환해주어야한다. */
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("objectId", &m_oID, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLight_Direction::Post_Render()
{
	return S_OK;
}

#pragma endregion