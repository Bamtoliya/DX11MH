
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

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Model_GrindRail"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;


	return S_OK;
}

void CGameObject_Spline::Free()
{
	__super::Free();
	Safe_Release(m_pRigidBodyCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pSplineCom);
	Safe_Release(m_pModelCom);
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
		//vector<_float3> vPoints = m_pSplineCom->GetSplinePoints();
		//m_pVIBufferCom->Update_Spline(vPoints);

		m_pSplineCom->UpdateSplineFrames();
		UpdateSplineBuffer();
		// 스플라인이 변경되면 콜라이더도 업데이트
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

	// 여기에 우리가 추가한 데이터들을 바인딩합니다.
	if (m_pSplineDataSRV)
	{
		m_pShaderCom->Bind_SRV("g_SplineData", m_pSplineDataSRV);
	}

	_float fModelLength = 0.196f; //m_pSplineCom->GetTotalLength();
	if (fModelLength <= 0.0f) fModelLength = 1.0f; // 0 이하인 경우 기본값 사용

	m_pShaderCom->Bind_RawValue("g_fModelLength", &fModelLength, sizeof(_float));
	_uint iNumFrames = (_uint)m_pSplineCom->GetSplineFrames().size();
	m_pShaderCom->Bind_RawValue("g_iNumSplineFrames", &iNumFrames, sizeof(_uint));

	// 라인 색상 설정 (예: 빨간색)
	/*_float4 vColor = _float4(1.f, 0.f, 0.f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;*/

	return S_OK;
	/*if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
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

	return S_OK;*/
}


HRESULT CGameObject_Spline::Render()
{
	if (!m_bVisible)
		return S_OK;
	//Bind_ShaderResources();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMesh = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMesh; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;
		//m_pModelCom->Bind_Bone_Matrices(m_pShaderCom, "g_BoneMatrices", i);
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	/*if (m_pVIBufferCom == nullptr || m_pVIBufferCom->Get_NumVertices() == 0)
		return S_OK;*/

	//if (FAILED(Bind_ShaderResources()))
	//	return E_FAIL;
	//
	//if (FAILED(m_pShaderCom->Begin(0)))
	//	return E_FAIL;
	//	
	//if (FAILED(m_pVIBufferCom->Render()))
	//	return E_FAIL;

	return S_OK;
}
HRESULT CGameObject_Spline::UpdateSplineBuffer()
{
	// 기존 버퍼 해제
	Safe_Release(m_pSplineDataSRV);
	Safe_Release(m_pSplineDataBuffer);

	const auto& frames = m_pSplineCom->GetSplineFrames();
	if (frames.empty())
		return S_OK;

	// 버퍼 생성
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.ByteWidth = sizeof(CSpline_Component::SPLINE_FRAME) * (_uint)frames.size();
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(CSpline_Component::SPLINE_FRAME);

	D3D11_SUBRESOURCE_DATA subData;
	ZeroMemory(&subData, sizeof(D3D11_SUBRESOURCE_DATA));
	subData.pSysMem = frames.data();

	if (FAILED(m_pDevice->CreateBuffer(&bufferDesc, &subData, &m_pSplineDataBuffer)))
		return E_FAIL;

	// SRV 생성
	if (FAILED(m_pDevice->CreateShaderResourceView(m_pSplineDataBuffer, nullptr, &m_pSplineDataSRV)))
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