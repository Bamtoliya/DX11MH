#include "Texture.h"
#include "Shader.h"
#pragma region Constructor/Destructor
CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CTexture::CTexture(const CTexture& Prototype)
	: CComponent{ Prototype }
	, m_SRVs{ Prototype.m_SRVs }
	, m_iNumTextures{ Prototype.m_iNumTextures }
	, m_strTextureFilePath{ Prototype.m_strTextureFilePath }
{
	for (auto& pSRV : m_SRVs)
		Safe_AddRef(pSRV);

}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);
	return pInstance;
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed to Created : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures)
{
	_tchar			szTexturePath[MAX_PATH] = {};
	_tchar			szExt[MAX_PATH] = {};
	m_strTextureFilePath = pTextureFilePath;
	m_originalToActualIndexMap.clear();
	_uint 		iActualIndex = 0;

	Safe_Release(m_pIndexMapSRV);
	Safe_Release(m_pIndexMapTexture);

	_wsplitpath_s(pTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

	for (size_t i = 0; i < iNumTextures; i++)
	{
		ID3D11ShaderResourceView* pSRV = { nullptr };

		wsprintf(szTexturePath, pTextureFilePath, i);

		if (!DoesFileExist(szTexturePath))
			continue; // 파일이 존재하지 않으면 다음 반복으로 넘어감

		HRESULT			hr = {};

		if (false == lstrcmp(TEXT(".dds"), szExt))
			hr = CreateDDSTextureFromFile(m_pDevice, szTexturePath, nullptr, &pSRV);

		else if (false == lstrcmp(TEXT(".tga"), szExt))
		{
			MSG_BOX("Failed to TGA");
			hr = E_FAIL;
		}
		else
			hr = CreateWICTextureFromFile(m_pDevice, szTexturePath, nullptr, &pSRV);

		if (FAILED(hr))
			return E_FAIL;

		m_SRVs.push_back(pSRV);
		m_originalToActualIndexMap[i] = iActualIndex++;
	}

	m_iNumTextures = iActualIndex;

	return S_OK;
}


HRESULT CTexture::Initialize(void* pArg)
{
	return S_OK;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pSRV : m_SRVs)
		Safe_Release(pSRV);

	m_SRVs.clear();
}
#pragma endregion

#pragma region Save&Load

HRESULT CTexture::Serialize(json& outJson)
{
	return S_OK;
}

HRESULT CTexture::Deserialize(simdjson::ondemand::object & jsonData)
{
	return S_OK;
}

HRESULT CTexture::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CTexture";
	outJson["TextureFilePath"] = WideToUtf8(m_strTextureFilePath.c_str()).c_str();
	outJson["NumTexture"] = m_iNumTextures;
	return S_OK;
}

HRESULT CTexture::Prototype_Deserialize(simdjson::ondemand::object & jsonData)
{
	using namespace simdjson;

	string_view texturePathSV;

	if (jsonData["TextureFilePath"].get_string().get(texturePathSV) != simdjson::SUCCESS) {
		MSG_BOX("CTexture::Prototype_Deserialize - 'TextureFilePath' field is missing or not a string.");
		return E_FAIL;
	}

	string texturePathStr(texturePathSV);

	size_t iNumTextures = 0;

	if (jsonData["NumTexture"].get_uint64().get(iNumTextures) != simdjson::SUCCESS) {
		MSG_BOX("CTexture::Prototype_Deserialize - 'NumTexture' field is missing or not a number.");
		return E_FAIL;
	}
	Initialize_Prototype(Utf8ToWide(texturePathStr).c_str(), static_cast<_uint>(iNumTextures));

	return S_OK;
}

#pragma endregion

HRESULT CTexture::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iTextureIndex)
{
	if (iTextureIndex >= m_iNumTextures)
		return E_FAIL;

	return pShader->Bind_SRV(pConstantName, m_SRVs[iTextureIndex]);	
}

HRESULT CTexture::Bind_ShaderResources(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_SRVs(pConstantName, &m_SRVs.front(), m_iNumTextures);	
}

HRESULT CTexture::Bind_ShaderResourceArray(CShader* pShader, const _char* pConstantName)
{
	return S_OK;
}

HRESULT CTexture::Bind_IndexMap(CShader* pShader, const _char* pConstantName)
{
	if (nullptr == pShader || nullptr == m_pIndexMapSRV) {
		return E_FAIL;
	}
	return pShader->Bind_SRV(pConstantName, m_pIndexMapSRV);
}

HRESULT CTexture::RenderImGui()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Texture", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Numbers of Texture: %d", m_iNumTextures);
		ImGui::Text(WideToUtf8(m_strTextureFilePath.c_str()).c_str());
		for (_uint i = 0; i < m_iNumTextures; ++i)
		{
			ImGui::Indent();
			if (ImGui::CollapsingHeader(to_string(i).c_str()))
			{
				ImGui::Image((ImTextureID)(m_SRVs[i]), ImVec2(100, 100), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(0.5f, 0.5f, 0.5f, 1)); // 테두리 색상 추가
				if (ImGui::IsItemHovered()) {
					ImGui::BeginTooltip();
					ImGui::Text("SRV: 0x%p", (void*)m_SRVs[i]);
					ImGui::Image((ImTextureID)((void*)m_SRVs[i]), ImVec2(256, 256)); // 툴팁에 더 큰 이미지
					ImGui::EndTooltip();
				}
			}
			ImGui::Unindent();
		}
		
		
		
	}
	ImGui::PopID();
	return S_OK;
}


