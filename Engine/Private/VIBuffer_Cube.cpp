#include "VIBuffer_Cube.h"

CVIBuffer_Cube::CVIBuffer_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer_Cube& Prototype)
	: CVIBuffer{ Prototype }
{

}

CVIBuffer_Cube* CVIBuffer_Cube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Cube::Clone(void* pArg)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Cube::Free()
{
	__super::Free();

}



HRESULT CVIBuffer_Cube::Initialize_Prototype()
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = 8;
	m_iVertexStride = sizeof(VTXCUBETEX);
	m_iNumIndices = 36;
	m_iIndexStride = sizeof(_ushort);
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
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

	VTXCUBETEX* pVertices = new VTXCUBETEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXCUBETEX) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];

	m_pVertexPositions[0] = pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[0].vTexcoord = pVertices[0].vPosition;
	//pVertices[0].vNormal = { 0.f, 0.f, 0.f };

	m_pVertexPositions[1] = pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[1].vTexcoord = pVertices[1].vPosition;
	//pVertices[1].vNormal = { 0.f, 0.f, 0.f };

	m_pVertexPositions[2] = pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[2].vTexcoord = pVertices[2].vPosition;
	//pVertices[2].vNormal = { 0.f, 0.f, 0.f };

	m_pVertexPositions[3] = pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[3].vTexcoord = pVertices[3].vPosition;
	//pVertices[3].vNormal = { 0.f, 0.f, 0.f };

	m_pVertexPositions[4] = pVertices[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[4].vTexcoord = pVertices[4].vPosition;
	//pVertices[4].vNormal = { 0.f, 0.f, 0.f };

	m_pVertexPositions[5] = pVertices[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[5].vTexcoord = pVertices[5].vPosition;
	//pVertices[5].vNormal = { 0.f, 0.f, 0.f };

	m_pVertexPositions[6] = pVertices[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[6].vTexcoord = pVertices[6].vPosition;
	//pVertices[6].vNormal = { 0.f, 0.f, 0.f };

	m_pVertexPositions[6] = pVertices[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[7].vTexcoord = pVertices[7].vPosition;
	//pVertices[7].vNormal = { 0.f, 0.f, 0.f };
	
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

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);
	/* +x */
	pIndices[0] = 1; pIndices[1] = 5; pIndices[2] = 6;
	pIndices[3] = 1; pIndices[4] = 6; pIndices[5] = 2;

	/* -x */
	pIndices[6] = 4; pIndices[7] = 0; pIndices[8] = 3;
	pIndices[9] = 4; pIndices[10] = 3; pIndices[11] = 7;


	/* +y */
	pIndices[12] = 4; pIndices[13] = 5; pIndices[14] = 1;
	pIndices[15] = 4; pIndices[16] = 1; pIndices[17] = 0;

	/* -y */
	pIndices[18] = 3; pIndices[19] = 2; pIndices[20] = 6;
	pIndices[21] = 3; pIndices[22] = 6; pIndices[23] = 7;

	/* +z */
	pIndices[24] = 5; pIndices[25] = 4; pIndices[26] = 7;
	pIndices[27] = 5; pIndices[28] = 7; pIndices[29] = 6;

	/* -z */
	pIndices[30] = 0; pIndices[31] = 1; pIndices[32] = 2;
	pIndices[33] = 0; pIndices[34] = 2; pIndices[35] = 3;

	D3D11_SUBRESOURCE_DATA		IBInitialData{};
	IBInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBBufferDesc, &IBInitialData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	
#pragma endregion

#pragma region NORMAL
	// 각 면의 노멀 계산 (인덱스 버퍼 기반)
	//D3DXVECTOR3 faceNormals[12]; // 12개의 삼각형 면
	//for (int i = 0; i < 12; ++i)
	//{
	//	D3DXVECTOR3 v0 = pVertices[pIndices[i * 3]].vPosition;
	//	D3DXVECTOR3 v1 = pVertices[pIndices[i * 3 + 1]].vPosition;
	//	D3DXVECTOR3 v2 = pVertices[pIndices[i * 3 + 2]].vPosition;

	//	D3DXVECTOR3 edge1 = v1 - v0;
	//	D3DXVECTOR3 edge2 = v2 - v0;
	//	D3DXVec3Cross(&faceNormals[i], &edge1, &edge2); // 외적
	//	D3DXVec3Normalize(&faceNormals[i], &faceNormals[i]); // 정규화
	//}

	//// 각 정점의 노멀 계산 (면 노멀 평균)
	//for (int i = 0; i < 8; ++i)
	//{
	//	D3DXVECTOR3 vertexNormal = { 0.f, 0.f, 0.f };
	//	int numFaces = 0;

	//	// 정점 i가 사용되는 모든 면을 찾아서 노멀을 더함
	//	for (int j = 0; j < 12; ++j) // 12 triangles
	//	{
	//		if (pIndices[j * 3] == i || pIndices[j * 3 + 1] == i || pIndices[j * 3 + 2] == i)
	//		{
	//			vertexNormal += faceNormals[j];
	//			numFaces++;
	//		}
	//	}

	//	// 평균 계산 및 정규화
	//	vertexNormal /= (float)numFaces;
	//	D3DXVec3Normalize(&pVertices[i].vNormal, &vertexNormal);
	//}

	//m_pVB->Unlock();
#pragma endregion
	return S_OK;
}

HRESULT CVIBuffer_Cube::Initialize(void* pArg)
{
	return S_OK;
}

#pragma region Save&Load

HRESULT CVIBuffer_Cube::Serialize(json& outJson)
{
	return S_OK;
}

HRESULT CVIBuffer_Cube::Deserialize(simdjson::ondemand::object & jsonData)
{
	return S_OK;
}

HRESULT CVIBuffer_Cube::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CVIBuffer_Cube";
	return S_OK;
}

HRESULT CVIBuffer_Cube::Prototype_Deserialize(simdjson::ondemand::object & jsonData)
{

	return S_OK;
}

#pragma endregion
