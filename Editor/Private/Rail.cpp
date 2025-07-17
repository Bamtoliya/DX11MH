#include "Rail.h"
#include "GameInstance.h"
#include "Spline_Component.h"
#include "Sonic.h"
#include "Sonic_Controller.h"

#pragma region Constructor/Destructor
CRail::CRail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CRail::CRail(const CRail& Prototype)
	: CGameObject{ Prototype }
{

}
CRail* CRail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRail* pInstance = new CRail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRail");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CRail::Clone(void* pArg)
{
	CRail* pInstance = new CRail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRail::Clone(simdjson::ondemand::object& jsonData)
{
	CRail* pInstance = new CRail(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Created : CRail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CRail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRail::Initialize(void* pArg)
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

	/*m_pSplineCom->AddControlPoint(_float3(0.f, 0.f, 0.f));
	m_pSplineCom->AddControlPoint(_float3(0.f, 2.f, 0.f));
	m_pSplineCom->AddControlPoint(_float3(30.f, 2.f, 0.f));
	m_pSplineCom->AddControlPoint(_float3(30.f, 0.f, 0.f));*/

	m_pSplineCom->Set_IsRail(true);

	if (FAILED(Late_Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRail::Initialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Deserialize(jsonData)))
		return E_FAIL;

	if (FAILED(Late_Initialize()))
		return E_FAIL;

	m_pSplineCom->Set_IsRail(true);

	return S_OK;
}

HRESULT CRail::Ready_Components()
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

void CRail::Free()
{
	__super::Free();
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pSplineCom);
	Safe_Release(m_pShaderCom);
}

#pragma endregion

#pragma region Save&Load

HRESULT CRail::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CRail"] = data;
	outJson["CRail"]["PrototypeTag"] = "Prototype_GameObject_Rail";
	return S_OK;
}

HRESULT CRail::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

HRESULT CRail::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CRail";
	return S_OK;
}

HRESULT CRail::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma endregion

#pragma region Loops
void CRail::Priority_Update(_float fTimeDelta)
{

}

void CRail::Update(_float fTimeDelta)
{
	if (m_pRigidBodyCom) m_pRigidBodyCom->Update(fTimeDelta);

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

void CRail::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CRail::Bind_ShaderResources()
{

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("objectId", &m_oID, sizeof(_uint))))
		return E_FAIL;

	_float4 vColor = _float4(1.f, 0.f, 0.f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}


HRESULT CRail::Render()
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
void CRail::OnCollisionEnter(PxActor* pOther, const PxContactPair& cp)
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
				CTransform* pSonicTransform = static_cast<CTransform*>(pSonic->Get_Component(g_strTransformTag));
				_vector vSonicPosition = pSonicTransform->Get_State(STATE::POSITION);

				// 스플라인에서 가장 가까운 지점의 파라미터(t) 찾기
				_float fClosestT = m_pSplineCom->Find_Closest_Param(vSonicPosition);

				// 컨트롤러에 스플라인 타기 시작을 알림
				pController->Register_Spline(m_pSplineCom);
				//pController->Begin_Rail(m_pSplineCom, fClosestT);
			}
		}
	}
}
void CRail::OnCollisionOverlapping(PxActor* pOther)
{

}
void CRail::OnCollisionExit(PxActor* pOther)
{

}
#pragma endregion