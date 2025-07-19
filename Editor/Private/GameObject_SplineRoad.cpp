#include "GameObject_SplineRoad.h"
#include "GameInstance.h"
#include "Spline_Component.h"
#include "Sonic.h"
#include "Sonic_Controller.h"

#pragma region Constructor/Destructor
CGameObject_SplineRoad::CGameObject_SplineRoad(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CGameObject_SplineRoad::CGameObject_SplineRoad(const CGameObject_SplineRoad& Prototype)
	: CGameObject{ Prototype }
{

}
CGameObject_SplineRoad* CGameObject_SplineRoad::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameObject_SplineRoad* pInstance = new CGameObject_SplineRoad(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGameObject_SplineRoad");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CGameObject_SplineRoad::Clone(void* pArg)
{
	CGameObject_SplineRoad* pInstance = new CGameObject_SplineRoad(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGameObject_SplineRoad");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGameObject_SplineRoad::Clone(simdjson::ondemand::object& jsonData)
{
	CGameObject_SplineRoad* pInstance = new CGameObject_SplineRoad(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Created : CGameObject_SplineRoad");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CGameObject_SplineRoad::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject_SplineRoad::Initialize(void* pArg)
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

	CTransform* pTransform = static_cast<CTransform*>(Get_Component(g_strTransformTag));
	if (pTransform)
	{
		_vector vCenter = pTransform->Get_State(STATE::POSITION);
		_float fRadius = 10.f; // 원하는 반지름
		m_pSplineCom->Create_CirclePath(vCenter, fRadius, 16); // 8개의 제어점으로 원 생성
	}

	m_pSplineCom->Set_IsRail(false);

	if (FAILED(Late_Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject_SplineRoad::Initialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Deserialize(jsonData)))
		return E_FAIL;

	if (FAILED(Late_Initialize()))
		return E_FAIL;

	m_pSplineCom->Set_IsRail(false);

	return S_OK;
}

HRESULT CGameObject_SplineRoad::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_Shader_Spline"),
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

void CGameObject_SplineRoad::Free()
{
	__super::Free();
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pSplineCom);
	Safe_Release(m_pShaderCom);
}

#pragma endregion

#pragma region Save&Load

HRESULT CGameObject_SplineRoad::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CGameObject_SplineRoad"] = data;
	outJson["CGameObject_SplineRoad"]["PrototypeTag"] = "Prototype_GameObject_SplineRoad";
	return S_OK;
}

HRESULT CGameObject_SplineRoad::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

HRESULT CGameObject_SplineRoad::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CGameObject_SplineRoad";
	return S_OK;
}

HRESULT CGameObject_SplineRoad::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma endregion

#pragma region Loops
void CGameObject_SplineRoad::Priority_Update(_float fTimeDelta)
{

}

void CGameObject_SplineRoad::Update(_float fTimeDelta)
{
	if (m_pSplineCom && m_pVIBufferCom && m_pSplineCom->IsDirty())
	{
		vector<_float3>& vPoints = m_pSplineCom->GetSplinePoints();
		vector<CSpline_Component::SPLINE_FRAME>& vSplineFrames = m_pSplineCom->Get_SplineFrames();
		//m_pVIBufferCom->Update_Spline(vPoints);
		m_pVIBufferCom->Update_Spline(vSplineFrames);

		if (m_pRigidBodyCom)
		{
			m_pRigidBodyCom->Create_Colliders_Along_Spline(m_pSplineCom, 0.25f); // 기본 반지름 0.25f
		}
		m_pSplineCom->SetDirty(false);
	}
}

void CGameObject_SplineRoad::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CGameObject_SplineRoad::Bind_ShaderResources()
{

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("objectId", &m_oID, sizeof(_uint))))
		return E_FAIL;

	// 라인 색상 설정 (예: 빨간색)
	_float4 vColor = _float4(1.f, 0.f, 0.f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}


HRESULT CGameObject_SplineRoad::Render()
{
	if (!m_bVisible)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}
void CGameObject_SplineRoad::OnCollisionEnter(PxActor* pOther, const PxContactPair& cp)
{
	CGameObject* pOtherObject = static_cast<CGameObject*>(pOther->userData);
	if (pOtherObject)
	{
		CSonic* pSonic = dynamic_cast<CSonic*>(pOtherObject);
		if (pSonic)
		{
			CSonic_Controller* pController = static_cast<CSonic_Controller*>(pSonic->Get_Component(L"Com_Controller"));
			if (pController)
			{
				pController->Register_Spline(m_pSplineCom);
			}
		}
	}
}
void CGameObject_SplineRoad::OnCollisionOverlapping(PxActor* pOther)
{

}
void CGameObject_SplineRoad::OnCollisionExit(PxActor* pOther)
{

}
#pragma endregion