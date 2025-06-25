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
	m_VBInstanceDesc.StructureByteStride = 0;

	if (FAILED(m_pDevice->CreateBuffer(&m_VBInstanceDesc, nullptr, &m_pVBInstance)))
		return E_FAIL;

	// 5. 시스템 메모리 할당 (기존 Prototype 로직)
	m_pVertexInstances = new VTXMESH_INSTANCE[m_iNumInstance];
	ZeroMemory(m_pVertexInstances, sizeof(VTXMESH_INSTANCE) * m_iNumInstance);

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
		// VTXMESH_INSTANCE 구조체에 맞게 월드 행렬 복사
		memcpy(&m_pVertexInstances[i].vRight, &worldMatrices[i].m[0], sizeof(_float4));
		memcpy(&m_pVertexInstances[i].vUp, &worldMatrices[i].m[1], sizeof(_float4));
		memcpy(&m_pVertexInstances[i].vLook, &worldMatrices[i].m[2], sizeof(_float4));
		memcpy(&m_pVertexInstances[i].vTranslation, &worldMatrices[i].m[3], sizeof(_float4));
	}

	memcpy(SubResource.pData, m_pVertexInstances, sizeof(VTXMESH_INSTANCE) * worldMatrices.size());
	m_pContext->Unmap(m_pVBInstance, 0);
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
