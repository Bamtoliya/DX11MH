#include "Light_Point.h"

#include "Light.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"

#pragma region Constructor/Destructor
CLight_Point::CLight_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CLight_Point::CLight_Point(const CLight_Point& Prototype)
	: CGameObject{ Prototype }
{

}

CLight_Point* CLight_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLight_Point* pInstance = new CLight_Point(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLight_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CLight_Point* CLight_Point::Clone(void* pArg)
{
	CLight_Point* pInstance = new CLight_Point(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLight_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CLight_Point* CLight_Point::Clone(simdjson::ondemand::object& jsonData)
{
	CLight_Point* pInstance = new CLight_Point(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Cloned : CLight_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CLight_Point::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLight_Point::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};
	lstrcpy(Desc.szName, TEXT("Point_Light"));
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	return S_OK;
}

HRESULT CLight_Point::Initialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Deserialize(jsonData);

	return S_OK;
}

HRESULT CLight_Point::Ready_Components()
{
	m_pLightCom = CLight::Create(m_pDevice, m_pContext);
	m_Components.emplace(L"Com_Light", m_pLightCom);
	m_pLightCom->Set_OwnerGameObject(this);
	LIGHT_DESC tDesc = {};
	tDesc.eType = LIGHT_DESC::TYPE_POINT;
	tDesc.vPosition = _float4(0.0f, 10.0f, 0.0f, 1.0f);
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

void CLight_Point::Free()
{
	__super::Free();
	Safe_Release(m_pLightCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}

#pragma endregion

#pragma region Save&Load

HRESULT CLight_Point::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CLight_Point"] = data;
	outJson["CLight_Point"]["PrototypeTag"] = "Prototype_GameObject_Light_Point";
	return S_OK;
}

HRESULT CLight_Point::Deserialize(simdjson::ondemand::object & jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

HRESULT CLight_Point::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CLight_Point";
	return S_OK;
}

HRESULT CLight_Point::Prototype_Deserialize(simdjson::ondemand::object & jsonData)
{

	return S_OK;
}

#pragma endregion

#pragma region Loops
void CLight_Point::Priority_Update(_float fTimeDelta)
{
}

void CLight_Point::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CLight_Point::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_Light(m_pLightCom);
}

HRESULT CLight_Point::Pre_Render()
{
	return S_OK;
}

HRESULT CLight_Point::Render()
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

HRESULT CLight_Point::Post_Render()
{
	return S_OK;
}

#pragma endregion



