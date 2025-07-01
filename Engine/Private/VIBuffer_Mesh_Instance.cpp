#include "VIBuffer_Mesh_Instance.h"
#include "Mesh.h"
#include "GameInstance.h"

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instance{ pDevice, pContext }
{
}

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(const CVIBuffer_Mesh_Instance& Prototype)
	: CVIBuffer_Instance{ Prototype }
{
	
}

HRESULT CVIBuffer_Mesh_Instance::Initialize_Prototype()
{
	m_iNumVertexBuffers = 2;
	return S_OK;
}

HRESULT CVIBuffer_Mesh_Instance::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	// 1. 전달받은 DESC로 캐스팅
	MESH_INSTANCE_DESC* pDesc = static_cast<MESH_INSTANCE_DESC*>(pArg);
	if (nullptr == pDesc->pMesh || 0 == pDesc->iNumInstance)
		return E_FAIL;

	// 2. 원본 메쉬로부터 정점/인덱스 정보 설정 (기존 Prototype 로직)
	m_iNumVertices = pDesc->pMesh->Get_NumVertices();
	m_iVertexStride = sizeof(VTXMESH);
	m_pVB = pDesc->pMesh->Get_VertexBuffer();
	Safe_AddRef(m_pVB);

	m_iNumIndices = pDesc->pMesh->Get_NumIndices();
	m_iIndexStride = sizeof(_uint);
	m_iNumIndexPerInstance = m_iNumIndices;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_pIB = pDesc->pMesh->Get_IndexBuffer();
	Safe_AddRef(m_pIB);

	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// 3. 인스턴스 정보 설정 (기존 Prototype 로직)
	m_iNumInstance = pDesc->iNumInstance;
	m_iVertexInstanceStride = sizeof(VTXMESH_INSTANCE);

	// 4. 인스턴스 버퍼 생성 (기존 Initialize와 Prototype 로직의 결합)
	m_VBInstanceDesc.ByteWidth = m_iNumInstance * m_iVertexInstanceStride;
	m_VBInstanceDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBInstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBInstanceDesc.MiscFlags = 0;
	m_VBInstanceDesc.StructureByteStride = m_iVertexInstanceStride;

	// 5. 시스템 메모리 할당 (기존 Prototype 로직)
	m_pVertexInstances = new VTXMESH_INSTANCE[m_iNumInstance];
	ZeroMemory(m_pVertexInstances, sizeof(VTXMESH_INSTANCE) * m_iNumInstance);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		m_pVertexInstances[i].vRight = {1.0f, 0.f, 0.f, 0.f};
		m_pVertexInstances[i].vUp = { 0.f, 1.f, 0.f, 0.f };
		m_pVertexInstances[i].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pVertexInstances[i].vTranslation = { (i % 20) * 2.f, 0.f, (i / 20) * 2.f, 1.0f };
		//_float3 vPos = _float3((i % 20) * 2.f, 0.f, (i / 20) * 2.f);
		//pTransform->Set_State(STATE::POSITION, XMLoadFloat3(&vPos));	
	}

	m_VBInstanceSubresourceData.pSysMem = m_pVertexInstances;

	if (FAILED(m_pDevice->CreateBuffer(&m_VBInstanceDesc, &m_VBInstanceSubresourceData, &m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}

#pragma region Save&Load
HRESULT CVIBuffer_Mesh_Instance::Serialize(json& outJosn)
{
	// 부모 클래스 직렬화 실행
	if (FAILED(__super::Serialize(outJosn)))
		return E_FAIL;

	// 인스턴스 개수 저장
	outJosn["NumInstance"] = m_iNumInstance;

	// 인스턴스 데이터 배열 생성
	json instancesArray = json::array();

	// 각 인스턴스의 트랜스폼 정보 저장
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		json instanceData;

		// 인스턴스 인덱스 저장
		instanceData["Index"] = i;

		// 위치 정보 저장
		instanceData["Position"] = {
			m_EditData[i].vPosition.x,
			m_EditData[i].vPosition.y,
			m_EditData[i].vPosition.z
		};

		// 회전 정보 저장
		instanceData["Rotation"] = {
			m_EditData[i].vRotation.x,
			m_EditData[i].vRotation.y,
			m_EditData[i].vRotation.z
		};

		// 크기 정보 저장
		instanceData["Scale"] = {
			m_EditData[i].vScale.x,
			m_EditData[i].vScale.y,
			m_EditData[i].vScale.z
		};

		// 배열에 인스턴스 데이터 추가
		instancesArray.push_back(instanceData);
	}

	// 인스턴스 배열을 json 객체에 추가
	outJosn["Instances"] = instancesArray;
}

HRESULT CVIBuffer_Mesh_Instance::Deserialize(simdjson::ondemand::object& jsonData)
{
	// 부모 클래스 역직렬화 실행
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;

	try {
		// 인스턴스 수 가져오기
		_uint iNumInstances = static_cast<_uint>(jsonData["NumInstance"].get_uint64());

		// 인스턴스 수가 변경되었다면 리사이즈
		if (iNumInstances != m_iNumInstance) {
			if (FAILED(Resize(iNumInstances)))
				return E_FAIL;
		}

		// 인스턴스 배열이 없으면 종료
		if (!jsonData["Instances"].error()) {
			auto instances = jsonData["Instances"].get_array();

			// 각 인스턴스 데이터 로드
			_uint i = 0;
			for (auto instance : instances) {
				if (i >= m_iNumInstance)
					break;

				auto instanceObj = instance.get_object();

				// 인덱스 확인 (선택사항)
				_uint idx = static_cast<_uint>(instanceObj["Index"].get_uint64());
				if (idx >= m_iNumInstance)
					continue;

				// 위치 데이터 로드
				auto positionArray = instanceObj["Position"].get_array();
				auto posIter = positionArray.begin();
				m_EditData[idx].vPosition.x = static_cast<_float>((*posIter).get_double());
				m_EditData[idx].vPosition.y = static_cast<_float>((*(++posIter)).get_double());
				m_EditData[idx].vPosition.z = static_cast<_float>((*(++posIter)).get_double());

				// 회전 데이터 로드
				auto rotationArray = instanceObj["Rotation"].get_array();
				auto rotIter = rotationArray.begin();
				m_EditData[idx].vRotation.x = static_cast<_float>((*rotIter).get_double());
				m_EditData[idx].vRotation.y = static_cast<_float>((*(++rotIter)).get_double());
				m_EditData[idx].vRotation.z = static_cast<_float>((*(++rotIter)).get_double());

				// 크기 데이터 로드
				auto scaleArray = instanceObj["Scale"].get_array();
				auto scaleIter = scaleArray.begin();
				m_EditData[idx].vScale.x = static_cast<_float>((*scaleIter).get_double());
				m_EditData[idx].vScale.y = static_cast<_float>((*(++scaleIter)).get_double());
				m_EditData[idx].vScale.z = static_cast<_float>((*(++scaleIter)).get_double());

				++i;
			}

			// 변경된 데이터를 버퍼에 적용
			ApplyChanges();
		}
	}
	catch (const simdjson::simdjson_error& e) {
		MSG_BOX("JSON 파싱 에러: 인스턴스 데이터 로드 실패");
		return S_OK;
	}

	return S_OK;
}

HRESULT CVIBuffer_Mesh_Instance::Prototype_Serialize(json& data)
{
	return S_OK;
}

HRESULT CVIBuffer_Mesh_Instance::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}
#pragma endregion


void CVIBuffer_Mesh_Instance::Update(const vector<_float4x4>& worldMatrices)
{
	if (worldMatrices.size() > m_iNumInstance)
		return;

	D3D11_MAPPED_SUBRESOURCE SubResource;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	for (size_t i = 0; i < worldMatrices.size(); ++i)
	{
		// VTXMESH_INSTANCE 구조체에 맞게 월드 행렬 복사
		memcpy(&m_pVertexInstances[i].vRight, &worldMatrices[i].m[0], sizeof(_float4));
		memcpy(&m_pVertexInstances[i].vUp, &worldMatrices[i].m[1], sizeof(_float4));
		memcpy(&m_pVertexInstances[i].vLook, &worldMatrices[i].m[2], sizeof(_float4));
		memcpy(&m_pVertexInstances[i].vTranslation, &worldMatrices[i].m[3], sizeof(_float4));
	}

	memcpy(SubResource.pData, m_pVertexInstances, sizeof(VTXMESH_INSTANCE) * worldMatrices.size());
	m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CVIBuffer_Mesh_Instance::Resize(_uint iNumInstance)
{
	if (iNumInstance == 0)
		return E_FAIL;

	// 기존 데이터 임시 저장 (필요한 경우)
	VTXMESH_INSTANCE* pOldInstances = nullptr;
	_uint iOldInstanceCount = 0;

	if (m_pVertexInstances != nullptr) {
		iOldInstanceCount = min(m_iNumInstance, iNumInstance);
		pOldInstances = new VTXMESH_INSTANCE[iOldInstanceCount];
		memcpy(pOldInstances, m_pVertexInstances, sizeof(VTXMESH_INSTANCE) * iOldInstanceCount);

		// 기존 메모리 해제
		Safe_Delete_Array(m_pVertexInstances);
	}

	// 새 인스턴스 버퍼 메모리 할당
	m_iNumInstance = iNumInstance;
	m_pVertexInstances = new VTXMESH_INSTANCE[m_iNumInstance];
	ZeroMemory(m_pVertexInstances, sizeof(VTXMESH_INSTANCE) * m_iNumInstance);

	// 기존 데이터 복원 (필요한 경우)
	if (pOldInstances != nullptr) {
		memcpy(m_pVertexInstances, pOldInstances, sizeof(VTXMESH_INSTANCE) * iOldInstanceCount);
		Safe_Delete_Array(pOldInstances);
	}

	// 새 인스턴스 초기화 (새로 추가된 인스턴스만)
	for (_uint i = iOldInstanceCount; i < m_iNumInstance; ++i) {
		m_pVertexInstances[i].vRight = { 1.0f, 0.f, 0.f, 0.f };
		m_pVertexInstances[i].vUp = { 0.f, 1.f, 0.f, 0.f };
		m_pVertexInstances[i].vLook = { 0.f, 0.f, 1.f, 0.f };
		m_pVertexInstances[i].vTranslation = { (i % 20) * 2.f, 0.f, (i / 20) * 2.f, 1.0f };
	}

	// 편집 데이터도 리사이즈
	m_EditData.resize(m_iNumInstance);
	for (_uint i = iOldInstanceCount; i < m_iNumInstance; ++i) {
		m_EditData[i].vPosition = _float3((i % 20) * 2.f, 0.f, (i / 20) * 2.f);
		m_EditData[i].vRotation = _float3(0.f, 0.f, 0.f);
		m_EditData[i].vScale = _float3(1.f, 1.f, 1.f);
	}

	// 기존 GPU 버퍼 해제
	Safe_Release(m_pVBInstance);

	// 버퍼 설명 업데이트
	m_VBInstanceDesc.ByteWidth = m_iNumInstance * m_iVertexInstanceStride;

	// 새 버퍼 생성iNumInstance
	m_VBInstanceSubresourceData.pSysMem = m_pVertexInstances;
	return m_pDevice->CreateBuffer(&m_VBInstanceDesc, &m_VBInstanceSubresourceData, &m_pVBInstance);
}

HRESULT CVIBuffer_Mesh_Instance::ApplyChanges()
{
	if (m_EditData.empty() || m_EditData.size() != m_iNumInstance)
		return E_FAIL;

	// 각 인스턴스마다 SRT 행렬 계산하여 적용
	for (_uint i = 0; i < m_iNumInstance; ++i) {
		// 스케일, 회전, 이동 행렬 계산
		XMMATRIX scaleMat = XMMatrixScaling(
			m_EditData[i].vScale.x,
			m_EditData[i].vScale.y,
			m_EditData[i].vScale.z
		);

		XMMATRIX rotMat = XMMatrixRotationRollPitchYaw(
			XMConvertToRadians(m_EditData[i].vRotation.x),
			XMConvertToRadians(m_EditData[i].vRotation.y),
			XMConvertToRadians(m_EditData[i].vRotation.z)
		);

		XMMATRIX transMat = XMMatrixTranslation(
			m_EditData[i].vPosition.x,
			m_EditData[i].vPosition.y,
			m_EditData[i].vPosition.z
		);

		// 최종 행렬 계산 (S * R * T)
		XMMATRIX worldMat = scaleMat * rotMat * transMat;

		// 행렬을 인스턴스 데이터에 저장
		XMStoreFloat4(&m_pVertexInstances[i].vRight, worldMat.r[0]);
		XMStoreFloat4(&m_pVertexInstances[i].vUp, worldMat.r[1]);
		XMStoreFloat4(&m_pVertexInstances[i].vLook, worldMat.r[2]);
		XMStoreFloat4(&m_pVertexInstances[i].vTranslation, worldMat.r[3]);
	}

	// GPU 버퍼 업데이트
	D3D11_MAPPED_SUBRESOURCE SubResource;
	HRESULT hr = m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	if (FAILED(hr))
		return hr;

	memcpy(SubResource.pData, m_pVertexInstances, sizeof(VTXMESH_INSTANCE) * m_iNumInstance);
	m_pContext->Unmap(m_pVBInstance, 0);

	return S_OK;
}

void CVIBuffer_Mesh_Instance::EditInstance(_uint iIndex)
{
	if (iIndex >= m_iNumInstance || m_EditData.size() <= iIndex) {
		m_EditData.resize(m_iNumInstance);
	}

	// 위치 편집
	ImGui::DragFloat3("Position", &m_EditData[iIndex].vPosition.x, 0.1f);

	// 회전 편집 (오일러 각)
	ImGui::DragFloat3("Rotation", &m_EditData[iIndex].vRotation.x, 0.1f);

	// 크기 편집
	ImGui::DragFloat3("Scale", &m_EditData[iIndex].vScale.x, 0.1f, 0.1f, 10.0f);
}

HRESULT CVIBuffer_Mesh_Instance::ResetInstances()
{
	m_EditData.resize(m_iNumInstance);

	// 기본 그리드 패턴으로 위치 재설정
	for (_uint i = 0; i < m_iNumInstance; ++i) {
		m_EditData[i].vPosition = _float3((i % 20) * 2.f, 0.f, (i / 20) * 2.f);
		m_EditData[i].vRotation = _float3(0.f, 0.f, 0.f);
		m_EditData[i].vScale = _float3(1.f, 1.f, 1.f);
	}

	// 변경 사항 적용
	return ApplyChanges();
}

CVIBuffer_Mesh_Instance* CVIBuffer_Mesh_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Mesh_Instance* pInstance = new CVIBuffer_Mesh_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Mesh_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Mesh_Instance::Clone(void* pArg)
{
	CVIBuffer_Mesh_Instance* pInstance = new CVIBuffer_Mesh_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Mesh_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Mesh_Instance::Free()
{
	__super::Free();

	if(true == m_bIsCloned)
		Safe_Delete_Array(m_pVertexInstances);
}
