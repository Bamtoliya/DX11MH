#pragma once
#include "Component.h"
BEGIN(Engine)
class ENGINE_DLL CVIBuffer abstract : public CComponent
{
#pragma region Constructor/Destructor
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& Prototype);
	virtual ~CVIBuffer() = default;
public:
	virtual CVIBuffer* Clone(void* pArg) = 0;
	virtual void Free();
#pragma endregion

#pragma region Initialize
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
#pragma endregion

public:
	HRESULT Render();

protected:
	HRESULT Create_VertexBuffer();
	HRESULT Create_IndexBuffer();

protected:

	//Vertex Buffer
	ID3D11Buffer*	m_pVB				= { nullptr };
	_uint			m_iNumVertices		= { 0 };
	_uint			m_iStride			= { 0 };
	_uint			m_iNumVertexBuffers = { 0 };

	//Index Buffer
	ID3D11Buffer*	m_pIB						= { nullptr };
	_uint			m_iIndicesByte				= { 0 };
	_uint			m_iNumPrimitive				= { 0 };
	_uint			m_iNumIndicesPerPrimitive	= { 0 };

	DXGI_FORMAT					m_eFormat = {};
	D3D11_PRIMITIVE_TOPOLOGY	m_eTopology = {};

protected:
	D3D11_BUFFER_DESC		m_BufferDesc		= {};
	D3D11_SUBRESOURCE_DATA	m_SubResourceData	= {};

protected:
	_float3*				m_pVerticesPos = { nullptr };
};
END