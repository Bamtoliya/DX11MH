#include "Shader.h"
#pragma region Constructor & Destructor
CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{

}

CShader::CShader(const CShader& Prototype)
	: CComponent{ Prototype }
	, m_pEffect{ Prototype.m_pEffect }
	, m_InputLayouts{ Prototype.m_InputLayouts }
	, m_strShaderFile{ Prototype.m_strShaderFile }
	, m_strLayoutName { Prototype.m_strLayoutName }
{
	Safe_AddRef(m_pEffect);
	for (auto& pInputLayout : m_InputLayouts)
		Safe_AddRef(pInputLayout);
}

CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShader* pInstance = new CShader(pDevice, pContext);
	return pInstance;
}

CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, const _char* pLayoutName)
{
	CShader* pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElements, iNumElements, pLayoutName)))
	{
		MSG_BOX("Failed to Create : CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
	CShader* pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CShader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CShader::Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, const _char* pLayoutName)
{
	_uint	iShaderFlag = {};
	m_strShaderFile = pShaderFilePath;
	m_strLayoutName = pLayoutName;

#ifdef _DEBUG
	iShaderFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iShaderFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif
	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		iShaderFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC		TechniqueDesc{};
	pTechnique->GetDesc(&TechniqueDesc);
	m_iNumPasses = TechniqueDesc.Passes;
	m_InputLayouts.reserve(m_iNumPasses);

	for (size_t i = 0; i < TechniqueDesc.Passes; i++)
	{
		ID3D11InputLayout* pInputLayout = { nullptr };
		ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(static_cast<uint32_t>(i));

		D3DX11_PASS_DESC		PassDesc{};
		pPass->GetDesc(&PassDesc);

		if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements,
			PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
			return E_FAIL;

		m_InputLayouts.push_back(pInputLayout);
	}

	return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
	return S_OK;
}

void CShader::Free()
{
	__super::Free();

	m_strShaderFile.clear();

	for (auto& pInputLayout : m_InputLayouts)
		Safe_Release(pInputLayout);

	m_InputLayouts.clear();

	Safe_Release(m_pEffect);
}
#pragma endregion


#pragma region Rendering
HRESULT CShader::Begin(_uint iPassIndex)
{
	m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);

	ID3DX11EffectPass* pPass = m_pEffect->GetTechniqueByIndex(0)->GetPassByIndex(iPassIndex);
	if (nullptr == pPass)
		return E_FAIL;

	Bind_RawValue("g_bTiling", &m_bTiling, sizeof(_bool));
	Bind_RawValue("g_vTilingScale", &m_fScale, sizeof(_float2));
	Bind_RawValue("g_vTilingOffset", &m_fOffset, sizeof(_float2));

	/* 이 쉐이더에 이 패스로 그린다. */
	/* Apply() 이전에 쉐이더에 전달해야할 모든 데이터들을 다 전달해놔야한다. */
	return pPass->Apply(0, m_pContext);
}
#pragma endregion

#pragma region Bind Resources
HRESULT CShader::Bind_RawValue(const _char* pConstantName, const void* pValue, _uint iLength)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pValue, 0, iLength);
}

HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrix(reinterpret_cast<const _float*>(pMatrix));
}

HRESULT CShader::Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrices, _uint iNumMatrices)
{

	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrixArray(reinterpret_cast<const _float*>(pMatrices), 0, iNumMatrices);
}

HRESULT CShader::Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;


	ID3DX11EffectShaderResourceVariable* pShaderResourceVariable = pVariable->AsShaderResource();
	if (nullptr == pShaderResourceVariable)
		return E_FAIL;

	return pShaderResourceVariable->SetResource(pSRV);
}

HRESULT CShader::Bind_SRVs(const _char* pConstantName, ID3D11ShaderResourceView** ppSRVs, _uint iNumSRVs)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pSRVVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVVariable)
		return E_FAIL;

	return pSRVVariable->SetResourceArray(ppSRVs, 0, iNumSRVs);
}
#pragma endregion

#pragma region Save&Load

HRESULT CShader::Serialize(json& outJson)
{
	json data;
	data["Tiling"] = m_bTiling;
	data["TilingScale"] = { m_fScale.x, m_fScale.y };
	data["TilingOffset"] = { m_fOffset.x, m_fOffset.y };
	data["Layout"] = m_strLayoutName;

	outJson["CShader"] = data;
	return S_OK;
}

HRESULT CShader::Deserialize(simdjson::ondemand::object & jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	simdjson::ondemand::object shaderObject;

	if (jsonData["CShader"].get_object().get(shaderObject) == simdjson::SUCCESS)
	{
		shaderObject["Tiling"].get_bool().get(m_bTiling);
		simdjson::ondemand::array size_array;
		shaderObject["TilingScale"].get_array().get(size_array);
		uint64_t count = 0;
		if (size_array.count_elements().get(count) == simdjson::SUCCESS && count >= 2)
		{
			auto iter = size_array.begin();
			m_fScale.x = static_cast<_float>((*iter).get_double().value_unsafe());
			++iter;
			m_fScale.y = static_cast<_float>((*iter).get_double().value_unsafe());
		}
		shaderObject["TilingOffset"].get_array().get(size_array);
		if (size_array.count_elements().get(count) == simdjson::SUCCESS && count >= 2)
		{
			auto iter = size_array.begin();
			m_fOffset.x = static_cast<_float>((*iter).get_double().value_unsafe());
			++iter;
			m_fOffset.y = static_cast<_float>((*iter).get_double().value_unsafe());
		}
	}

	return S_OK;
}

HRESULT CShader::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CShader";
	outJson["ShaderPath"] = WideToUtf8(m_strShaderFile.c_str()).c_str();
	outJson["Layout"] = m_strLayoutName;
	return S_OK;
}

HRESULT CShader::Prototype_Deserialize(simdjson::ondemand::object & jsonData)
{
	using namespace simdjson; // for string_view if not already in scope

	string_view shaderPathSV;
	// JSON 파일의 키가 "ShdaerPath"이므로 이를 사용합니다.
	if (jsonData["ShaderPath"].get_string().get(shaderPathSV) != simdjson::SUCCESS) {
		MSG_BOX("CShader::Prototype_Deserialize - 'ShaderPath' field is missing or not a string.");
		return E_FAIL;
	}
	std::wstring wsShaderPath = Utf8ToWide(std::string(shaderPathSV));

	string_view layoutTagSV;
	if (jsonData["Layout"].get_string().get(layoutTagSV) != simdjson::SUCCESS) {
		MSG_BOX("CShader::Prototype_Deserialize - 'Layout' field is missing or not a string.");
		return E_FAIL;
	}
	std::string layoutTag(layoutTagSV);

	pair<const D3D11_INPUT_ELEMENT_DESC*, _uint> layoutDetails = {};
	if (layoutTag == "VTXPOSTEX") { layoutDetails = { VTXPOSTEX::Elements, VTXPOSTEX::iNumElements }; }
	if (layoutTag == "VTXNORTEX") { layoutDetails = { VTXNORTEX::Elements, VTXNORTEX::iNumElements }; }
	if (layoutTag == "VTXMESH") { layoutDetails = { VTXMESH::Elements, VTXMESH::iNumElements }; }
	if (layoutTag == "VTXANIMMESH") { layoutDetails = { VTXANIMMESH::Elements, VTXANIMMESH::iNumElements }; }
	if (layoutTag == "VTXCUBETEX") { layoutDetails = { VTXCUBETEX::Elements, VTXCUBETEX::iNumElements }; }

	if (FAILED(Initialize_Prototype(wsShaderPath.c_str(), layoutDetails.first, layoutDetails.second, layoutTag.c_str()))) {
		return E_FAIL;
	}

	return S_OK;
}

#pragma endregion

HRESULT CShader::RenderImGui()
{
	if (ImGui::CollapsingHeader("Shader", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::TextUnformatted(WideToUtf8(m_strShaderFile.c_str()).c_str());
		ImGui::Checkbox("Tiling", &m_bTiling);
		ImGui::DragFloat2("Tiling Scale", &m_fScale.x, 0.1f, 0.0f, 100.0f);
		ImGui::SliderFloat2("Tiling Offset", &m_fOffset.x, -1.0f, 1.0f);
	}
	return S_OK;
}