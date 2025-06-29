
#include "GameObject_SplineMesh.h"
#include "GameInstance.h"

#pragma region Constructor/Destructor
CGameObject_SplineMesh::CGameObject_SplineMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{

}

CGameObject_SplineMesh::CGameObject_SplineMesh(const CGameObject_SplineMesh& Prototype)
	: CGameObject{ Prototype }
{

}
CGameObject_SplineMesh* CGameObject_SplineMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameObject_SplineMesh* pInstance = new CGameObject_SplineMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGameObject_SplineMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CGameObject_SplineMesh::Clone(void* pArg)
{
	CGameObject_SplineMesh* pInstance = new CGameObject_SplineMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGameObject_SplineMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGameObject_SplineMesh::Clone(simdjson::ondemand::object& jsonData)
{
	CGameObject_SplineMesh* pInstance = new CGameObject_SplineMesh(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Created : CGameObject_SplineMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CGameObject_SplineMesh::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject_SplineMesh::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};
	if (pArg != nullptr)
		memcpy(&Desc, pArg, sizeof(GAMEOBJECT_DESC));
	Desc.fRotationPerSec = 0.f;
	Desc.fSpeedPerSec = 0.f;
	if (nullptr == pArg)
	{
		lstrcpy(Desc.szName, TEXT("GameObject_SplineMesh"));
	}
	else
	{
		lstrcpy(Desc.szName, static_cast<GAMEOBJECT_DESC*>(pArg)->szName);
	}

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject_SplineMesh::Initialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Deserialize(jsonData)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject_SplineMesh::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_Shader_VtxSplineModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Model_GrindRail"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Spline */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_Spline"),
		TEXT("Com_Spline"), reinterpret_cast<CComponent**>(&m_pSplineCom))))
		return E_FAIL;

	m_pSplineCom->AddControlPoint(_float3(-10.f, 0.f, 10.f));
	m_pSplineCom->AddControlPoint(_float3(-5.f, 5.f, -5.f));
	m_pSplineCom->AddControlPoint(_float3(5.f, -5.f, 5.f));
	m_pSplineCom->AddControlPoint(_float3(10.f, 0.f, -10.f));
	m_pSplineCom->AddControlPoint(_float3(15.f, 5.f, 0.f));
	m_pSplineCom->AddControlPoint(_float3(20.f, 0.f, 5.f));
	m_pSplineCom->UpdateSplineFrames();

	CVIBuffer_Mesh_Instance::MESH_INSTANCE_DESC meshInstanceDesc;
	meshInstanceDesc.pMesh = m_pModelCom->Get_Meshes()[0];
	meshInstanceDesc.iNumInstance = (_uint)m_pSplineCom->GetSplineFrames().size();
	m_fMeshLength = 0.2f;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Prototype_Component_VIBuffer_Mesh_Instance"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferInstanceCom), &meshInstanceDesc)))
		return E_FAIL;

	


	return S_OK;
}

void CGameObject_SplineMesh::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferInstanceCom);
	Safe_Release(m_pSplineCom);
}

#pragma endregion

#pragma region Save&Load

HRESULT CGameObject_SplineMesh::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CGameObject_SplineMesh"] = data;
	outJson["CGameObject_SplineMesh"]["PrototypeTag"] = "Prototype_GameObject_GameObject_SplineMesh";
	return S_OK;
}

HRESULT CGameObject_SplineMesh::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

HRESULT CGameObject_SplineMesh::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CGameObject_SplineMesh";
	return S_OK;
}

HRESULT CGameObject_SplineMesh::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma endregion

#pragma region Loops
void CGameObject_SplineMesh::Priority_Update(_float fTimeDelta)
{

}

void CGameObject_SplineMesh::Update(_float fTimeDelta)
{
	// 스플라인과 VIBuffer 컴포넌트가 없으면 return
	if (!m_pSplineCom || !m_pVIBufferInstanceCom)
		return;

	const auto& splineFrames = m_pSplineCom->GetSplineFrames();
	if (splineFrames.size() < 2)
		return;

	// 인스턴스 개수 = 스플라인 세그먼트 개수
	_uint iNumSegments = static_cast<_uint>(splineFrames.size() - 1);

	// 인스턴스 개수가 변경되었다면 VIBuffer를 리사이즈해야 합니다.
	if (m_pVIBufferInstanceCom->Get_NumInstance() != iNumSegments)
	{
		if (FAILED(m_pVIBufferInstanceCom->Resize(iNumSegments)))
			return;
	}

	// 월드 행렬을 담을 벡터를 준비합니다.
	vector<_float4x4> worldMatrices(iNumSegments);

	// 각 세그먼트(인스턴스)에 대한 정보를 월드 행렬에 압축하여 저장합니다.
	for (_uint i = 0; i < iNumSegments; ++i)
	{
		const auto& startFrame = splineFrames[i];
		const auto& endFrame = splineFrames[i + 1];

		// _float4x4 행렬에 데이터를 채워넣습니다.
		// r[0]: 시작점 위치 (Start Position) + 시작점 방향의 X (Start Tangent.x)
		// r[1]: 시작점 방향의 Y, Z (Start Tangent.y, z) + 끝점 위치의 X, Y (End Position.x, y)
		// r[2]: 끝점 위치의 Z (End Position.z) + 끝점 방향 (End Tangent)
		// r[3]: 스플라인의 Up 벡터 (Normal) - 여기서는 간단하게 World Up(0,1,0)을 사용한다고 가정
		//        시작과 끝 노멀을 보간하기 위해 두 노멀 정보를 모두 담을 수도 있습니다.

		_float4x4& matrix = worldMatrices[i];

		// 1행: 시작점 위치 (vPosition)
		memcpy(&matrix.m[0][0], &startFrame.vPosition, sizeof(_float3));
		matrix.m[0][3] = 0.f; // Padding

		// 2행: 시작점 방향 (vTangent)
		memcpy(&matrix.m[1][0], &startFrame.vTangent, sizeof(_float3));
		matrix.m[1][3] = 0.f; // Padding

		// 3행: 끝점 위치 (vPosition)
		memcpy(&matrix.m[2][0], &endFrame.vPosition, sizeof(_float3));
		matrix.m[2][3] = 0.f; // Padding

		// 4행: 끝점 방향 (vTangent)
		memcpy(&matrix.m[3][0], &endFrame.vTangent, sizeof(_float3));
		matrix.m[3][3] = 0.f; // Padding
	}

	// 가공된 데이터가 담긴 벡터로 인스턴스 버퍼를 업데이트합니다.
	m_pVIBufferInstanceCom->Update(worldMatrices);
}

void CGameObject_SplineMesh::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::RG_NONBLEND, this);
}

HRESULT CGameObject_SplineMesh::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("objectId", &m_oID, sizeof(_uint))))
		return E_FAIL;

	/*m_pShaderCom->Bind_SRV("")*/

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


HRESULT CGameObject_SplineMesh::Render()
{
	if (!m_bVisible)
		return S_OK;
	Bind_ShaderResources();

	_uint		iNumMesh = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMesh; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		//m_pModelCom->Bind_Bone_Matrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		if (FAILED(m_pVIBufferInstanceCom->Bind_Buffers()))
			return E_FAIL;


		m_pShaderCom->Bind_RawValue("g_fMeshLength", &m_fMeshLength, sizeof(_float)); // 모델 길이는 별도 전달

		if (FAILED(m_pVIBufferInstanceCom->Render()))
			return E_FAIL;

		/*if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;*/
	}

	return S_OK;
}
HRESULT CGameObject_SplineMesh::Update_InstanceTransforms()
{
	if (!m_pSplineCom || !m_pVIBufferInstanceCom)
		return E_FAIL;

	const auto& splineFrames = m_pSplineCom->GetSplineFrames();
	if (splineFrames.size() < 2)
		return S_OK;

	// 세그먼트 수 = 스플라인 프레임 수 - 1
	_uint iNumSegments = static_cast<_uint>(splineFrames.size() - 1);

	// 인스턴스 수가 세그먼트 수와 다르면 리사이즈
	if (m_pVIBufferInstanceCom->Get_NumInstance() != iNumSegments)
	{
		if (FAILED(m_pVIBufferInstanceCom->Resize(iNumSegments)))
			return E_FAIL;
	}

	vector<_float4x4> worldMatrices(iNumSegments);

	for (_uint i = 0; i < iNumSegments; ++i)
	{
		const auto& startFrame = splineFrames[i];
		const auto& endFrame = splineFrames[i + 1];

		// 세그먼트의 중점 계산
		_float3 centerPos;
		XMStoreFloat3(&centerPos, XMVectorScale(
			XMVectorAdd(XMLoadFloat3(&startFrame.vPosition), XMLoadFloat3(&endFrame.vPosition)), 0.5f));

		// 세그먼트의 방향 벡터 (정규화된 Tangent 사용)
		XMVECTOR forward = XMVector3Normalize(XMLoadFloat3(&startFrame.vTangent));
		XMVECTOR up = XMVector3Normalize(XMLoadFloat3(&startFrame.vNormal));
		XMVECTOR right = XMVector3Cross(up, forward);

		// 세그먼트 길이 계산
		_float segmentLength = XMVectorGetX(XMVector3Length(
			XMVectorSubtract(XMLoadFloat3(&endFrame.vPosition), XMLoadFloat3(&startFrame.vPosition))));

		// 스케일 행렬 (Z축으로 길이만큼 스트레치)
		XMMATRIX scaleMatrix = XMMatrixScaling(1.0f, 1.0f, segmentLength);

		// 회전 행렬 구성
		XMMATRIX rotationMatrix = XMMatrixIdentity();
		rotationMatrix.r[0] = right;
		rotationMatrix.r[1] = up;
		rotationMatrix.r[2] = forward;
		rotationMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

		// 이동 행렬
		XMMATRIX translationMatrix = XMMatrixTranslation(centerPos.x, centerPos.y, centerPos.z);

		// 최종 월드 행렬 = Scale * Rotation * Translation
		XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
		XMStoreFloat4x4(&worldMatrices[i], worldMatrix);
	}

	// 인스턴스 버퍼 업데이트
	m_pVIBufferInstanceCom->Update(worldMatrices);

	return S_OK;
}
HRESULT CGameObject_SplineMesh::Update_SplinBuffer()
{
	//const auto& frames = m_pSplineCom->GetSplineFrames();
	//if (frames.size() < 2) return S_OK;

	//// 세그먼트 데이터를 담을 벡터
	//vector<SplineSegment> segments;
	//segments.reserve(frames.size() - 1);

	//for (size_t i = 0; i < frames.size() - 1; ++i)
	//{
	//	SplineSegment segment;
	//	segment.vStartPos = _float4(frames[i].vPosition.x, frames[i].vPosition.y, frames[i].vPosition.z, 1.0f);
	//	segment.vStartTangent = frames[i].vTangent; // 스플라인 길이에 따라 접선 크기 조절 필요
	//	segment.vEndPos = _float4(frames[i + 1].vPosition.x, frames[i + 1].vPosition.y, frames[i + 1].vPosition.z, 1.0f);
	//	segment.vEndTangent = frames[i + 1].vTangent;

	//	segments.push_back(segment);
	//}
	return S_OK;
}
#pragma endregion