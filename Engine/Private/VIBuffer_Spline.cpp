#include "VIBuffer_Spline.h"
#pragma region Constructor/Destructor
CVIBuffer_Spline::CVIBuffer_Spline(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Spline::CVIBuffer_Spline(const CVIBuffer_Spline& Prototype)
    : CVIBuffer{ Prototype }
{

}

CVIBuffer_Spline* CVIBuffer_Spline::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Spline* pInstance = new CVIBuffer_Spline(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Spline");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CVIBuffer_Spline* CVIBuffer_Spline::Clone(void* pArg)
{
	CVIBuffer_Spline* pInstance = new CVIBuffer_Spline(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Spline");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Spline::Free()
{
    __super::Free();
}
#pragma endregion

#pragma region Initialize
HRESULT CVIBuffer_Spline::Initialize_Prototype()
{
	// 초기화 시점에는 정점이 없으므로, 버퍼는 나중에 동적으로 생성 및 업데이트합니다.
	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXPOS); // 위치 정보만 필요합니다. (색상은 셰이더에서 제어)
	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
	m_iNumIndices = 0; // 인덱스 버퍼는 사용하지 않습니다.

	return S_OK;
}

HRESULT CVIBuffer_Spline::Initialize(void* pArg)
{
    return S_OK;
}
HRESULT CVIBuffer_Spline::Late_Initialize()
{
	__super::Late_Initialize();
	return S_OK;
}
#pragma endregion

#pragma region Save&Load

HRESULT CVIBuffer_Spline::Serialize(json& outJson)
{
	// VIBuffer_Spline에서는 실제 정점 데이터를 저장할 필요가 없습니다.
	// 실제 데이터는 Spline_Component에서 관리하며 동적으로 생성됩니다.
	if (FAILED(__super::Serialize(outJson)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Spline::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Spline::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CVIBuffer_Spline";
	return S_OK;
}

HRESULT CVIBuffer_Spline::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{

	return S_OK;
}

#pragma endregion


HRESULT CVIBuffer_Spline::Update_Spline(const vector<_float3>& vPoints)
{
	if (vPoints.size() < 2)
	{
		m_iNumVertices = 0;
		return S_OK; // 점이 2개 미만이면 선을 그릴 수 없음
	}

	// 기존 버퍼 해제
	Safe_Release(m_pVB);

	m_iNumVertices = static_cast<_uint>(vPoints.size());

	// 정점 버퍼 생성
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC; // 업데이트가 가능하도록 DYNAMIC으로 설정
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU에서 쓸 수 있도록 설정
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	vector<VTXPOS> vertices(m_iNumVertices);
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		vertices[i].vPosition = vPoints[i];
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = vertices.data();

	if (FAILED(Create_VertexBuffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Spline::Render()
{
    if (m_iNumVertices == 0)
        return S_OK;

    // 버퍼를 바인딩합니다
    Bind_Buffers();
    
    // DrawIndexed 대신 Draw를 사용하여 정점 버퍼의 모든 정점을 그립니다
    m_pContext->Draw(m_iNumVertices, 0);

    return S_OK;
}

HRESULT CVIBuffer_Spline::RenderImGui()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("VIBuffer_Spline", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 읽기 전용 정보 표시
		ImGui::Text("Vertex Count: %d", m_iNumVertices);
		ImGui::Text("Vertex Stride: %d bytes", m_iVertexStride);

		// 토폴로지 정보 표시
		const char* topologyNames[] = {
			"UNDEFINED", "POINTLIST", "LINELIST", "LINESTRIP",
			"TRIANGLELIST", "TRIANGLESTRIP" };
		int topologyIndex = (m_ePrimitiveTopology <= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP)
			? m_ePrimitiveTopology : 0;
		ImGui::Text("Topology: %s", topologyNames[topologyIndex]);
	}
	ImGui::PopID();
    return S_OK;
}