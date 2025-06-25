#pragma once
#include "Component.h"
NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer abstract : public CComponent
{
#pragma region Constructor/Destructor
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& Prototype);
	virtual ~CVIBuffer() = default;
public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free();
#pragma endregion

#pragma region Initialize
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
#pragma endregion

public:
	virtual HRESULT Bind_Buffers();
	virtual HRESULT Render();
	virtual HRESULT RenderImGui() override;
	_float3 Get_LocalSize() { return m_vLocalSize; }
	void Set_LocalSize(const _float3& vSize) { memcpy(&m_vLocalSize, &vSize, sizeof(_float3)); }
	void Set_LocalSize(_float x = 1.f, _float y = 1.f, _float z = 1.f) { m_vLocalSize = { x, y, z }; }
	const _float3* Get_VertexPositions() const { return m_pVertexPositions; }
	const _uint Get_NumVertices() const { return m_iNumVertices; }
	const _uint Get_NumIndices() const { return m_iNumIndices; }
	ID3D11Buffer* Get_VertexBuffer() { return m_pVB; }
	ID3D11Buffer* Get_IndexBuffer() { return m_pIB; }
	const _uint Get_NumVertexBuffers() const { return m_iNumVertexBuffers; }
	const _uint Get_NumPrimitive() const { return m_iNumPrimitive; }
	const _uint Get_NumIndicesPerPrimitive() const { return m_iNumIndicesPerPrimitive; }
	const D3D11_BUFFER_DESC Get_IndexBufferDesc() const { return m_IndexBufferDesc; }

protected:
	HRESULT Create_VertexBuffer();
	HRESULT Create_IndexBuffer();

protected:

	//Vertex Buffer
	ID3D11Buffer*	m_pVB				= { nullptr };
	_uint			m_iNumVertices = {};
	_uint			m_iVertexStride = {};
	_uint			m_iNumVertexBuffers = { 0 };

	//Index Buffer
	ID3D11Buffer*	m_pIB						= { nullptr };
	_uint			m_iNumIndices = {};
	_uint			m_iIndexStride = {};
	_uint			m_iNumPrimitive				= { 0 };
	_uint			m_iNumIndicesPerPrimitive	= { 0 };

	DXGI_FORMAT					m_eIndexFormat = {};
	D3D11_PRIMITIVE_TOPOLOGY	m_ePrimitiveTopology = {};

protected:
	D3D11_BUFFER_DESC		m_BufferDesc		= {};
	D3D11_BUFFER_DESC		m_IndexBufferDesc = {};
	D3D11_SUBRESOURCE_DATA	m_SubResourceData	= {};

protected:
	_float3*				m_pVertexPositions = { nullptr };
	_uint*					m_iVertexIndicies = { nullptr };

protected:
	_float3					m_vLocalSize = { 1.0f, 1.0f, 1.0f };
};
NS_END