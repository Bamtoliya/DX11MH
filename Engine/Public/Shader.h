#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
#pragma region Constructor/Destructor
private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& Prototype);
	virtual ~CShader() = default;
public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual CComponent* Clone(void* pArg) override;
public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual void Free() override;
#pragma endregion

public:
	HRESULT Begin(_uint iPassIndex);
	HRESULT Bind_RawValue(const _char* pConstantName, const void* pValue, _uint iLength);
	HRESULT Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix);	
	HRESULT Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices);
	HRESULT Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV);
	HRESULT Bind_SRVs(const _char* pConstantName, ID3D11ShaderResourceView** ppSRVs, _uint iNumSRVs);

public:
	virtual HRESULT RenderImGui() override;

private:
	ID3DX11Effect*				m_pEffect = { nullptr };
	_uint						m_iNumPasses = {};
	vector<ID3D11InputLayout*>	m_InputLayouts;
	_wstring					m_strShaderFile = {};

private:
	_bool						m_bTiling;

	_float2						m_fOffset = {};
	_float2						m_fScale = { 1.f, 1.f };
};

NS_END