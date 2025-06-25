
#include "GameObject_Spline.h"
#include "GameInstance.h"

#pragma region Constructor/Destructor
CGameObject_Spline::CGameObject_Spline(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CGameObject_Spline::CGameObject_Spline(const CGameObject_Spline& Prototype)
	: CGameObject{ Prototype }
{

}
CGameObject_Spline* CGameObject_Spline::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameObject_Spline* pInstance = new CGameObject_Spline(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGameObject_Spline");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CGameObject_Spline::Clone(void* pArg)
{
	CGameObject_Spline* pInstance = new CGameObject_Spline(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGameObject_Spline");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGameObject_Spline::Clone(simdjson::ondemand::object& jsonData)
{
	CGameObject_Spline* pInstance = new CGameObject_Spline(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Created : CGameObject_Spline");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CGameObject_Spline::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject_Spline::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};
	if (pArg != nullptr)
		memcpy(&Desc, pArg, sizeof(GAMEOBJECT_DESC));
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	if (nullptr == pArg)
	{
		lstrcpy(Desc.szName, TEXT("GameObject_Spline"));
	}
	else
	{
		lstrcpy(Desc.szName, static_cast<GAMEOBJECT_DESC*>(pArg)->szName);
	}

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pSplineCom->AddControlPoint(_float3(-10.f, 0.f, 10.f));
	m_pSplineCom->AddControlPoint(_float3(-5.f, 5.f, -5.f));
	m_pSplineCom->AddControlPoint(_float3(5.f, -5.f, 5.f));
	m_pSplineCom->AddControlPoint(_float3(10.f, 0.f, -10.f));
	m_pSplineCom->AddControlPoint(_float3(15.f, 5.f, 0.f));
	m_pSplineCom->AddControlPoint(_float3(20.f, 0.f, 5.f));

	return S_OK;
}

HRESULT CGameObject_Spline::Initialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Deserialize(jsonData)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject_Spline::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_Shader_VtxSplineModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_VIBuffer_Spline"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Spline */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Spline"),
		TEXT("Com_Spline"), reinterpret_cast<CComponent**>(&m_pSplineCom))))
		return E_FAIL;

	/* For.Com_RigidBody */
	CRigidBody::RIGIDBODY_DESC rigidBodyDesc;
	rigidBodyDesc.eBodyType = CRigidBody::EBodyType::STATIC; // 정적 콜라이더로 설정

	/* For.Com_RigidBody */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_RigidBody"),
		TEXT("Com_RigidBody"), reinterpret_cast<CComponent**>(&m_pRigidBodyCom), &rigidBodyDesc)))
		return E_FAIL;

	return S_OK;
}

void CGameObject_Spline::Free()
{
	__super::Free();
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pSplineCom);
	Safe_Release(m_pShaderCom);
}

#pragma endregion

#pragma region Save&Load

HRESULT CGameObject_Spline::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CGameObject_Spline"] = data;
	outJson["CGameObject_Spline"]["PrototypeTag"] = "Prototype_GameObject_GameObject_Spline";
	return S_OK;
}

HRESULT CGameObject_Spline::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

HRESULT CGameObject_Spline::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CGameObject_Spline";
	return S_OK;
}

HRESULT CGameObject_Spline::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma endregion

#pragma region Loops
void CGameObject_Spline::Priority_Update(_float fTimeDelta)
{

}

void CGameObject_Spline::Update(_float fTimeDelta)
{
	if (m_pSplineCom && m_pVIBufferCom && m_pSplineCom->IsDirty())
	{
		vector<_float3> vPoints = m_pSplineCom->GetSplinePoints();
		m_pVIBufferCom->Update_Spline(vPoints);

		if (m_pRigidBodyCom)
		{
			m_pRigidBodyCom->Create_Colliders_Along_Spline(m_pSplineCom, 0.25f); // 기본 반지름 0.25f
		}
		m_pSplineCom->SetDirty(false);
	}
}

void CGameObject_Spline::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CGameObject_Spline::Bind_ShaderResources()
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

	// 라인 색상 설정 (예: 빨간색)
	_float4 vColor = _float4(1.f, 0.f, 0.f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}


HRESULT CGameObject_Spline::Render()
{
	if (!m_bVisible)
		return S_OK;
	//Bind_ShaderResources();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}
void CGameObject_Spline::OnCollisionEnter(PxActor* pOther, const PxContactPair& cp)
{

}
void CGameObject_Spline::OnCollisionOverlapping(PxActor* pOther)
{

}
void CGameObject_Spline::OnCollisionExit(PxActor* pOther)
{

}
#pragma endregion