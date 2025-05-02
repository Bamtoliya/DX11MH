#include "VIBuffer.h"
#pragma region Constructor/Destructor
CVIBuffer::CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent { pDevice, pContext }
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& Prototype)
    : CComponent                { Prototype }
    , m_pVB                     { Prototype.m_pVB }
    , m_iNumVertices            { Prototype.m_iNumVertices }
    , m_iVertexStride           { Prototype.m_iVertexStride }
    , m_iNumVertexBuffers       { Prototype.m_iNumVertexBuffers }
    , m_pIB                     { Prototype.m_pIB }
    , m_iNumIndices             { Prototype.m_iNumIndices }
    , m_iIndexStride            { Prototype.m_iIndexStride }
    , m_iNumPrimitive           { Prototype.m_iNumPrimitive }
    , m_iNumIndicesPerPrimitive { Prototype.m_iNumIndicesPerPrimitive }
    , m_eIndexFormat            { Prototype.m_eIndexFormat }
    , m_ePrimitiveTopology      { Prototype.m_ePrimitiveTopology }
    /*, m_BufferDesc              { Prototype.m_BufferDesc }
    , m_SubResourceData         { Prototype.m_SubResourceData }*/
    , m_pVertexPositions        { Prototype.m_pVertexPositions }
{
    Safe_AddRef(m_pVB);
    Safe_AddRef(m_pIB);
}

void CVIBuffer::Free()
{
    __super::Free();

    if (!m_bIsCloned)
		Safe_Delete_Array(m_pVertexPositions);

    Safe_Release(m_pVB);
    Safe_Release(m_pIB);
}
#pragma endregion

#pragma region Initialize
HRESULT CVIBuffer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{
    return S_OK;
}
#pragma endregion

HRESULT CVIBuffer::Bind_Buffers()
{
    ID3D11Buffer*   pVertexBuffers[]   = { m_pVB, };
    _uint	        iVertexStrides[]  = { m_iVertexStride, };
    _uint           iOffsets[]  = { 0, };

    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
    m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	/*
	LPCSTR SemanticName;
    UINT SemanticIndex;
    DXGI_FORMAT Format;
    UINT InputSlot;
    UINT AlignedByteOffset;
    D3D11_INPUT_CLASSIFICATION InputSlotClass;
    UINT InstanceDataStepRate;
	*/
	/*D3D11_INPUT_ELEMENT_DESC		Elements[6] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	_uint		iNumElements = 2;*/

	//m_pDevice->CreateInputLayout(Elements, iNumElements, );

	//m_pContext->IASetInputLayout();

    return S_OK;
}

HRESULT CVIBuffer::Render()
{
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer::Create_VertexBuffer()
{
    return m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVB);
}

HRESULT CVIBuffer::Create_IndexBuffer()
{
    return m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pIB);
}
