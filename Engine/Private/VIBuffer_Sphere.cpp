#include "VIBuffer_Sphere.h"

CVIBuffer_Sphere::CVIBuffer_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CVIBuffer_Sphere::CVIBuffer_Sphere(const CVIBuffer_Sphere& Prototype)
	: CVIBuffer{ Prototype }
	, m_fRadius{ Prototype.m_fRadius }
	, m_iSlices{ Prototype.m_iSlices }
	, m_iStacks{ Prototype.m_iStacks }
{
}


HRESULT CVIBuffer_Sphere::Initialize_Prototype()
{
	m_fRadius = 1.0f;
	m_iSlices = 32;
	m_iStacks = 16;
	m_iNumVertices = (m_iSlices + 1) * (m_iStacks + 1);
	m_iVertexStride = sizeof(VTXNORTEX);
	m_iNumIndices = m_iSlices * m_iStacks * 2 * 3;
	m_iIndexStride = sizeof(_uint);
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	D3D11_BUFFER_DESC			VBBufferDesc{};
	VBBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VBBufferDesc.CPUAccessFlags = /*D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE*/0;
	VBBufferDesc.StructureByteStride = m_iVertexStride;
	VBBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA		VBInitialData{};

	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	for (int stack = 0; stack <= m_iStacks; ++stack)
	{
		float theta = (float)stack / m_iStacks * XM_PI - XM_PI / 2.0f; // -PI/2 ~ PI/2 (위도)
		float sinTheta = sinf(theta);
		float cosTheta = cosf(theta);
		for (int slice = 0; slice <= m_iSlices; ++slice)
		{
			float phi = (float)slice / m_iSlices * XM_PI * 2; // 0 ~ 2PI (경도)
			float sinPhi = sinf(phi);
			float cosPhi = cosf(phi);
			int vertexIndex = stack * (m_iSlices + 1) + slice;


			// 위치 계산
			m_pVertexPositions[vertexIndex].x = pVertices[vertexIndex].vPosition.x = m_fRadius * cosTheta * cosPhi;
			m_pVertexPositions[vertexIndex].y = pVertices[vertexIndex].vPosition.y = m_fRadius * sinTheta;
			m_pVertexPositions[vertexIndex].z = pVertices[vertexIndex].vPosition.z = m_fRadius * cosTheta * sinPhi;

			// 법선 계산 (구의 중심에서 정점을 향하는 벡터)
			pVertices[vertexIndex].vNormal = pVertices[vertexIndex].vPosition;
			DirectX::XMVECTOR vNormal = DirectX::XMLoadFloat3(&pVertices[vertexIndex].vNormal);
			// 2. XMVECTOR를 정규화합니다.
			vNormal = DirectX::XMVector3Normalize(vNormal);
			// 3. 정규화된 XMVECTOR를 다시 XMFLOAT3 타입으로 저장합니다.
			DirectX::XMStoreFloat3(&pVertices[vertexIndex].vNormal, vNormal);

			// 텍스처 좌표 (선택 사항)
			pVertices[vertexIndex].vTexcoord.x = (float)slice / m_iSlices;
			pVertices[vertexIndex].vTexcoord.y = (float)stack / m_iStacks;
		}
	}

	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBBufferDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region INDEX_BUFFER
	D3D11_BUFFER_DESC			IBBufferDesc{};
	IBBufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	IBBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IBBufferDesc.CPUAccessFlags = /*D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE*/0;
	IBBufferDesc.StructureByteStride = m_iIndexStride;
	IBBufferDesc.MiscFlags = 0;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	int index = 0;
	for (int stack = 0; stack < m_iStacks; ++stack)
	{
		for (int slice = 0; slice < m_iSlices; ++slice)
		{
			// Calculate indices for the two triangles forming a quad
			int p0 = stack * (m_iSlices + 1) + slice;
			int p1 = p0 + 1;
			int p2 = (stack + 1) * (m_iSlices + 1) + slice;
			int p3 = p2 + 1;

			//int index = (stack * (m_iSlices + 1) + slice) * 6;
			// First triangle
			pIndices[index++] = p0;
			pIndices[index++] = p2;
			pIndices[index++] = p1;

			// Second triangle
			pIndices[index++] = p1;
			pIndices[index++] = p2;
			pIndices[index++] = p3;
		}
	}
#pragma endregion
	return S_OK;
}

HRESULT CVIBuffer_Sphere::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Sphere* CVIBuffer_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Sphere* pInstance = new CVIBuffer_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Sphere::Clone(void* pArg)
{
	CVIBuffer_Sphere* pInstance = new CVIBuffer_Sphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Sphere::Free()
{
	__super::Free();

}
