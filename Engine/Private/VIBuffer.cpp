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
    , m_iStride                 { Prototype.m_iStride }
    , m_iNumVertexBuffers       { Prototype.m_iNumVertexBuffers }
    , m_pIB                     { Prototype.m_pIB }
    , m_iIndicesByte            { Prototype.m_iIndicesByte }
    , m_iNumPrimitive           { Prototype.m_iNumPrimitive }
    , m_iNumIndicesPerPrimitive { Prototype.m_iNumIndicesPerPrimitive }
    , m_eFormat                 { Prototype.m_eFormat }
    , m_eTopology               { Prototype.m_eTopology }
    , m_BufferDesc              { Prototype.m_BufferDesc }
    , m_SubResourceData         { Prototype.m_SubResourceData }
    , m_pVerticesPos            { Prototype.m_pVerticesPos }
{
    Safe_AddRef(m_pVB);
    Safe_AddRef(m_pIB);
}

void CVIBuffer::Free()
{
    __super::Free();

    if (!m_bIsCloned)
        Safe_Delete_Array(m_pVerticesPos);

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


HRESULT CVIBuffer::Render()
{
    ID3D11Buffer*   pBuffer[]   = { m_pVB, };
    _uint	        iStrides[]  = { m_iStride, };
    _uint           iOffsets[]  = { 0, };

    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pBuffer, iStrides, iOffsets);
    m_pContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
    m_pContext->IASetPrimitiveTopology(m_eTopology);

    m_pContext->DrawIndexed(m_iNumIndicesPerPrimitive * m_iNumPrimitive, 0, 0);

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
