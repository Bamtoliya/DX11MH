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
		_float3 position = _float3((i % 20) * 2.f, 0.f, (i / 20) * 2.f);
		_float3 rotation = _float3(0.f, 0.f, 0.f);
		_float3 scale = _float3(1.f, 1.f, 1.f);
		//_float3 vPos = _float3((i % 20) * 2.f, 0.f, (i / 20) * 2.f);
		//pTransform->Set_State(STATE::POSITION, XMLoadFloat3(&vPos));	

		// SRT 정보로부터 행렬 생성
		CreateMatrixFromSRT(m_pVertexInstances[i], position, rotation, scale);
	}

	// 7. EditData 초기화 (호환성을 위해 유지)
	m_EditData.resize(m_iNumInstance);
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		ExtractSRTFromMatrix(m_pVertexInstances[i],
			m_EditData[i].vPosition,
			m_EditData[i].vRotation,
			m_EditData[i].vScale);
	}

	m_VBInstanceSubresourceData.pSysMem = m_pVertexInstances;

	if (FAILED(m_pDevice->CreateBuffer(&m_VBInstanceDesc, &m_VBInstanceSubresourceData, &m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}

#pragma region Save&Load
HRESULT CVIBuffer_Mesh_Instance::Serialize(json& outJson)
{
	// 부모 클래스 직렬화 실행
	if (FAILED(__super::Serialize(outJson)))
		return E_FAIL;

	// 인스턴스 개수 저장
	outJson["NumInstance"] = m_iNumInstance;

	// 인스턴스 데이터 배열 생성
	json instancesArray = json::array();

	// 각 인스턴스의 변환 행렬 저장
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		json instanceData;
		instanceData["Index"] = i;

		// 행렬 데이터 저장
		instanceData["Right"] = {
			m_pVertexInstances[i].vRight.x,
			m_pVertexInstances[i].vRight.y,
			m_pVertexInstances[i].vRight.z,
			m_pVertexInstances[i].vRight.w
		};

		instanceData["Up"] = {
			m_pVertexInstances[i].vUp.x,
			m_pVertexInstances[i].vUp.y,
			m_pVertexInstances[i].vUp.z,
			m_pVertexInstances[i].vUp.w
		};

		instanceData["Look"] = {
			m_pVertexInstances[i].vLook.x,
			m_pVertexInstances[i].vLook.y,
			m_pVertexInstances[i].vLook.z,
			m_pVertexInstances[i].vLook.w
		};

		instanceData["Translation"] = {
			m_pVertexInstances[i].vTranslation.x,
			m_pVertexInstances[i].vTranslation.y,
			m_pVertexInstances[i].vTranslation.z,
			m_pVertexInstances[i].vTranslation.w
		};

		// 배열에 인스턴스 데이터 추가
		instancesArray.push_back(instanceData);
	}

	// 인스턴스 배열을 json 객체에 추가
	outJson["Instances"] = instancesArray;

	return S_OK;
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

				// Right 벡터 로드
				auto rightArray = instanceObj["Right"].get_array();
				auto rightIter = rightArray.begin();
				m_pVertexInstances[idx].vRight.x = static_cast<_float>((*rightIter).get_double());
				m_pVertexInstances[idx].vRight.y = static_cast<_float>((*(++rightIter)).get_double());
				m_pVertexInstances[idx].vRight.z = static_cast<_float>((*(++rightIter)).get_double());
				m_pVertexInstances[idx].vRight.w = static_cast<_float>((*(++rightIter)).get_double());

				// Up 벡터 로드
				auto upArray = instanceObj["Up"].get_array();
				auto upIter = upArray.begin();
				m_pVertexInstances[idx].vUp.x = static_cast<_float>((*upIter).get_double());
				m_pVertexInstances[idx].vUp.y = static_cast<_float>((*(++upIter)).get_double());
				m_pVertexInstances[idx].vUp.z = static_cast<_float>((*(++upIter)).get_double());
				m_pVertexInstances[idx].vUp.w = static_cast<_float>((*(++upIter)).get_double());

				// Look 벡터 로드
				auto lookArray = instanceObj["Look"].get_array();
				auto lookIter = lookArray.begin();
				m_pVertexInstances[idx].vLook.x = static_cast<_float>((*lookIter).get_double());
				m_pVertexInstances[idx].vLook.y = static_cast<_float>((*(++lookIter)).get_double());
				m_pVertexInstances[idx].vLook.z = static_cast<_float>((*(++lookIter)).get_double());
				m_pVertexInstances[idx].vLook.w = static_cast<_float>((*(++lookIter)).get_double());

				// Translation 벡터 로드
				auto transArray = instanceObj["Translation"].get_array();
				auto transIter = transArray.begin();
				m_pVertexInstances[idx].vTranslation.x = static_cast<_float>((*transIter).get_double());
				m_pVertexInstances[idx].vTranslation.y = static_cast<_float>((*(++transIter)).get_double());
				m_pVertexInstances[idx].vTranslation.z = static_cast<_float>((*(++transIter)).get_double());
				m_pVertexInstances[idx].vTranslation.w = static_cast<_float>((*(++transIter)).get_double());

				++i;
			}

			// EditData 업데이트 (호환성을 위해)
			for (_uint i = 0; i < m_iNumInstance; ++i) {
				ExtractSRTFromMatrix(m_pVertexInstances[i],
					m_EditData[i].vPosition,
					m_EditData[i].vRotation,
					m_EditData[i].vScale);
			}

			// 변경된 데이터를 버퍼에 적용
			return ApplyChanges();
		}
	}
	catch (const simdjson::simdjson_error& e) {
		MSG_BOX("JSON 파싱 에러: 인스턴스 데이터 로드 실패");
		return E_FAIL;
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

HRESULT CVIBuffer_Mesh_Instance::Bind_Buffers()
{
	if (FAILED(__super::Bind_Buffers()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Mesh_Instance::Bind_Buffers(CMesh* pMesh)
{
	m_iNumIndexPerInstance = pMesh->Get_NumIndices();
	ID3D11Buffer* pVertexBuffers[] = {
		pMesh->Get_VertexBuffer(),
		m_pVBInstance,
	};

	_uint		iVertexStrides[] = {
		pMesh->Get_VertexStride(),
		m_iVertexInstanceStride

	};

	_uint		iOffsets[] = {
		0,
		0
	};
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(pMesh->Get_IndexBuffer(), m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

void CVIBuffer_Mesh_Instance::Update(const vector<_float4x4>& worldMatrices)
{
	if (worldMatrices.size() > m_iNumInstance)
		return;

	D3D11_MAPPED_SUBRESOURCE SubResource;
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	for (size_t i = 0; i < worldMatrices.size(); ++i)
	{
		// 직접 행렬 데이터 복사
		memcpy(&m_pVertexInstances[i].vRight, &worldMatrices[i].m[0], sizeof(_float4));
		memcpy(&m_pVertexInstances[i].vUp, &worldMatrices[i].m[1], sizeof(_float4));
		memcpy(&m_pVertexInstances[i].vLook, &worldMatrices[i].m[2], sizeof(_float4));
		memcpy(&m_pVertexInstances[i].vTranslation, &worldMatrices[i].m[3], sizeof(_float4));
	}

	// 변경된 모든 인스턴스 데이터를 GPU 버퍼에 복사
	memcpy(SubResource.pData, m_pVertexInstances, sizeof(VTXMESH_INSTANCE) * worldMatrices.size());
	m_pContext->Unmap(m_pVBInstance, 0);

	// EditData 동기화 (호환성을 위해 필요하다면)
	for (size_t i = 0; i < worldMatrices.size(); ++i)
	{
		ExtractSRTFromMatrix(m_pVertexInstances[i],
			m_EditData[i].vPosition,
			m_EditData[i].vRotation,
			m_EditData[i].vScale);
	}
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
	// m_EditData가 있는 경우 이를 기반으로 m_pVertexInstances 업데이트
	if (!m_EditData.empty() && m_EditData.size() == m_iNumInstance)
	{
		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			CreateMatrixFromSRT(m_pVertexInstances[i],
				m_EditData[i].vPosition,
				m_EditData[i].vRotation,
				m_EditData[i].vScale);
		}
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
	if (iIndex >= m_iNumInstance)
		return;

	// 현재 인스턴스에서 SRT 정보 추출
	_float3 position, rotation, scale;
	ExtractSRTFromMatrix(m_pVertexInstances[iIndex], position, rotation, scale);

	bool bModified = false;

	// 위치 편집
	if (ImGui::DragFloat3("Position", &position.x, 0.1f))
	{
		bModified = true;
	}

	// 회전 편집 (오일러 각)
	if (ImGui::DragFloat3("Rotation", &rotation.x, 0.1f))
	{
		bModified = true;
	}

	// 크기 편집
	if (ImGui::DragFloat3("Scale", &scale.x, 0.1f, 0.1f, 10.0f))
	{
		bModified = true;
	}

	// 변경사항이 있을 때만 갱신
	if (bModified)
	{
		// 수정된 SRT 정보로 행렬 업데이트
		CreateMatrixFromSRT(m_pVertexInstances[iIndex], position, rotation, scale);

		// GPU 버퍼 갱신
		UpdateSingleInstance(iIndex);

		// EditData 동기화 (호환성을 위해)
		m_EditData[iIndex].vPosition = position;
		m_EditData[iIndex].vRotation = rotation;
		m_EditData[iIndex].vScale = scale;
	}
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

void CVIBuffer_Mesh_Instance::ExtractSRTFromMatrix(const VTXMESH_INSTANCE& instance, _float3& outPosition, _float3& outRotation, _float3& outScale)
{
	// 4x4 행렬 구성
	_float4x4 worldMatrix;
	memcpy(&worldMatrix.m[0], &instance.vRight, sizeof(_float4));
	memcpy(&worldMatrix.m[1], &instance.vUp, sizeof(_float4));
	memcpy(&worldMatrix.m[2], &instance.vLook, sizeof(_float4));
	memcpy(&worldMatrix.m[3], &instance.vTranslation, sizeof(_float4));

	// 행렬 분해
	_vector scale, rotation, translation;
	if (XMMatrixDecompose(&scale, &rotation, &translation, XMLoadFloat4x4(&worldMatrix)))
	{
		// 위치 저장
		XMStoreFloat3(&outPosition, translation);

		// 회전 쿼터니언에서 오일러 각으로 변환
		XMMATRIX rotMat = XMMatrixRotationQuaternion(rotation);

		// 회전 행렬에서 오일러 각 추출 (라디안)
		float pitch = asinf(-XMVectorGetZ(rotMat.r[0]));
		float yaw = atan2f(XMVectorGetZ(rotMat.r[1]), XMVectorGetZ(rotMat.r[2]));
		float roll = atan2f(XMVectorGetY(rotMat.r[0]), XMVectorGetX(rotMat.r[0]));

		// 라디안에서 각도로 변환
		outRotation.x = XMConvertToDegrees(pitch);
		outRotation.y = XMConvertToDegrees(yaw);
		outRotation.z = XMConvertToDegrees(roll);

		// 스케일 저장
		XMStoreFloat3(&outScale, scale);
	}
	else
	{
		// 행렬 분해 실패 시 기본값 설정
		outPosition = _float3(instance.vTranslation.x, instance.vTranslation.y, instance.vTranslation.z);
		outRotation = _float3(0.f, 0.f, 0.f);
		outScale = _float3(1.f, 1.f, 1.f);
	}
}

void CVIBuffer_Mesh_Instance::CreateMatrixFromSRT(VTXMESH_INSTANCE& outInstance, const _float3& position, const _float3& rotation, const _float3& scale)
{
	// SRT 행렬 계산
	XMMATRIX scaleMat = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX rotMat = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(rotation.x),
		XMConvertToRadians(rotation.y),
		XMConvertToRadians(rotation.z)
	);
	XMMATRIX transMat = XMMatrixTranslation(position.x, position.y, position.z);

	// 최종 행렬 계산 (S * R * T)
	XMMATRIX worldMat = scaleMat * rotMat * transMat;

	// 행렬을 인스턴스 데이터에 저장
	XMStoreFloat4(&outInstance.vRight, worldMat.r[0]);
	XMStoreFloat4(&outInstance.vUp, worldMat.r[1]);
	XMStoreFloat4(&outInstance.vLook, worldMat.r[2]);
	XMStoreFloat4(&outInstance.vTranslation, worldMat.r[3]);
}

HRESULT CVIBuffer_Mesh_Instance::UpdateSingleInstance(_uint iIndex)
{
	if (iIndex >= m_iNumInstance)
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE SubResource;
	HRESULT hr = m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	if (FAILED(hr))
		return hr;

	// 전체 버퍼 갱신 (개별 업데이트는 D3D11에서 직접 지원하지 않음)
	memcpy(SubResource.pData, m_pVertexInstances, sizeof(VTXMESH_INSTANCE) * m_iNumInstance);
	m_pContext->Unmap(m_pVBInstance, 0);

	return S_OK;
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
