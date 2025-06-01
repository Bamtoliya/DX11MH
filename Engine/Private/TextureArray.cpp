#include "TextureArray.h"
#include "Shader.h"

#pragma region Constructor/Destructor
CTextureArray::CTextureArray(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CTextureArray::CTextureArray(const CTextureArray& Prototype)
	: CComponent{ Prototype }
	, m_SRVs{ Prototype.m_SRVs }
	, m_iNumTextures{ Prototype.m_iNumTextures }
	, m_strTextureFilePath{ Prototype.m_strTextureFilePath }
	, m_originalToActualIndexMap{ Prototype.m_originalToActualIndexMap }
	, m_pIndexMapTexture{ Prototype.m_pIndexMapTexture }
	, m_pIndexMapSRV{ Prototype.m_pIndexMapSRV }
{
	for (auto& pSRV : m_SRVs)
		Safe_AddRef(pSRV);

	Safe_AddRef(m_pTextureArrayResource);
	Safe_AddRef(m_pIndexMapTexture);
	Safe_AddRef(m_pIndexMapSRV);
}

CTextureArray* CTextureArray::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTextureArray* pInstance = new CTextureArray(pDevice, pContext);
	return pInstance;
}

CTextureArray* CTextureArray::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures)
{
	CTextureArray* pInstance = new CTextureArray(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed to Created : CTextureArray");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTextureArray::Clone(void* pArg)
{
	CTextureArray* pInstance = new CTextureArray(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTextureArray");
		Safe_Release(pInstance);
	}

	return pInstance;
}



HRESULT CTextureArray::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTexturesToLoad)
{
	// 기존 리소스 해제
	for (auto& pSRV : m_SRVs) Safe_Release(pSRV);
	m_SRVs.clear();
	Safe_Release(m_pTextureArrayResource);
	Safe_Release(m_pIndexMapSRV);
	Safe_Release(m_pIndexMapTexture);

	m_strTextureFilePath = pTextureFilePath;
	m_originalToActualIndexMap.clear();
	_uint iActualIndex = 0; // 실제로 로드된 텍스처(배열 슬라이스)의 수

	_tchar szTexturePath[MAX_PATH] = {};
	_tchar szExt[MAX_PATH] = {};
	_wsplitpath_s(pTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

	std::vector<ID3D11Texture2D*> loadedIndividualTextures; // 임시로 개별 텍스처 저장
	D3D11_TEXTURE2D_DESC firstTexDesc = {};

	for (_uint i = 0; i < iNumTexturesToLoad; ++i)
	{
		wsprintf(szTexturePath, pTextureFilePath, i);
		if (!DoesFileExist(szTexturePath)) continue;

		ID3D11Resource* pResource = nullptr;
		ID3D11ShaderResourceView* pTempSRV = nullptr; // 로더가 생성하는 임시 SRV
		HRESULT hr = E_FAIL;

		if (false == lstrcmp(TEXT(".dds"), szExt)) {
			hr = CreateDDSTextureFromFile(m_pDevice, szTexturePath, &pResource, &pTempSRV);
		}
		else {
			hr = CreateWICTextureFromFile(m_pDevice, m_pContext, szTexturePath, &pResource, &pTempSRV);
		}

		if (SUCCEEDED(hr) && pResource) {
			ID3D11Texture2D* pTexture2D = nullptr;
			hr = pResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pTexture2D));
			if (SUCCEEDED(hr) && pTexture2D) {
				D3D11_TEXTURE2D_DESC currentTexDesc;
				pTexture2D->GetDesc(&currentTexDesc);

				if (iActualIndex == 0) {
					firstTexDesc = currentTexDesc;
				}
				else {
					// 모든 텍스처는 동일한 크기, 포맷, 밉 레벨을 가져야 함
					if (currentTexDesc.Width != firstTexDesc.Width ||
						currentTexDesc.Height != firstTexDesc.Height ||
						currentTexDesc.Format != firstTexDesc.Format ||
						currentTexDesc.MipLevels != firstTexDesc.MipLevels) {
						MSG_BOX("Texture array elements have inconsistent properties.");
						Safe_Release(pTexture2D);
						Safe_Release(pResource);
						Safe_Release(pTempSRV);
						// 이전 로드된 텍스처들도 해제
						for (auto& tex : loadedIndividualTextures) Safe_Release(tex);
						loadedIndividualTextures.clear();
						return E_FAIL;
					}
				}
				loadedIndividualTextures.push_back(pTexture2D); // pTexture2D는 나중에 해제
				m_originalToActualIndexMap[iActualIndex++] = i;
			}
			else {
				if (pTexture2D) Safe_Release(pTexture2D);
			}
			Safe_Release(pResource);
			Safe_Release(pTempSRV); // 임시 SRV 해제
		}
	}

	if (iActualIndex == 0) { // 로드된 텍스처가 없음
		m_iNumTextures = 0;
		return S_OK;
	}

	// Texture2DArray 생성
	D3D11_TEXTURE2D_DESC textureArrayDesc = firstTexDesc;
	textureArrayDesc.ArraySize = iActualIndex;
	textureArrayDesc.Usage = D3D11_USAGE_DEFAULT; // CopySubresourceRegion을 위해 DEFAULT로 설정
	textureArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	// 필요하다면 D3D11_RESOURCE_MISC_GENERATE_MIPS 플래그와 D3D11_BIND_RENDER_TARGET 추가 (밉맵 자동 생성 시)
	// textureArrayDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS; // 예시
	textureArrayDesc.CPUAccessFlags = 0;

	HRESULT hr = m_pDevice->CreateTexture2D(&textureArrayDesc, nullptr, &m_pTextureArrayResource);
	if (FAILED(hr)) {
		MSG_BOX("Failed to create Texture2DArray resource.");
		for (auto& tex : loadedIndividualTextures) Safe_Release(tex);
		return E_FAIL;
	}

	// 개별 텍스처 데이터를 Texture2DArray의 각 슬라이스로 복사
	for (_uint slice = 0; slice < iActualIndex; ++slice) {
		ID3D11Texture2D* pSourceTexture = loadedIndividualTextures[slice];
		for (_uint mipLevel = 0; mipLevel < firstTexDesc.MipLevels; ++mipLevel) {
			UINT destSubresource = D3D11CalcSubresource(mipLevel, slice, firstTexDesc.MipLevels);
			UINT srcSubresource = D3D11CalcSubresource(mipLevel, 0, firstTexDesc.MipLevels); // 개별 텍스처는 ArraySlice가 0
			m_pContext->CopySubresourceRegion(m_pTextureArrayResource, destSubresource, 0, 0, 0, pSourceTexture, srcSubresource, nullptr);
		}
	}

	// (선택 사항) Texture2DArray에 대해 밉맵 자동 생성 (위에서 MiscFlags 및 BindFlags 설정 필요)
	// if (textureArrayDesc.MiscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS) {
	//    m_pContext->GenerateMips(pSRV_for_TextureArray); // SRV가 먼저 필요함
	// }

	// Texture2DArray에 대한 SRV 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureArrayDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = textureArrayDesc.MipLevels; // -1은 모든 밉 레벨
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = textureArrayDesc.ArraySize;

	ID3D11ShaderResourceView* pArraySRV = nullptr;
	hr = m_pDevice->CreateShaderResourceView(m_pTextureArrayResource, &srvDesc, &pArraySRV);
	if (FAILED(hr)) {
		MSG_BOX("Failed to create SRV for Texture2DArray.");
		Safe_Release(m_pTextureArrayResource);
		for (auto& tex : loadedIndividualTextures) Safe_Release(tex);
		return E_FAIL;
	}
	m_SRVs.push_back(pArraySRV); // 이제 m_SRVs[0]에 Texture2DArray의 SRV가 저장됨

	// 임시로 로드했던 개별 텍스처들 해제
	for (auto& tex : loadedIndividualTextures) Safe_Release(tex);
	loadedIndividualTextures.clear();

	m_iNumTextures = iActualIndex; // 배열의 슬라이스 수

	if (iActualIndex > 0) {
		if (FAILED(CreateAndCacheIndexMapTexture())) {
			MSG_BOX("Failed to create index map texture.");
			// 필요시 여기서 추가 에러 처리
			return E_FAIL;
		}
	}

	return S_OK;
}


HRESULT CTextureArray::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTextureArray::CreateAndCacheIndexMapTexture()
{
	// 기존 인덱스 맵 리소스 해제
	Safe_Release(m_pIndexMapSRV);
	Safe_Release(m_pIndexMapTexture);

	if (m_originalToActualIndexMap.empty()) {
		return S_OK; // 매핑할 정보가 없으면 생성할 필요 없음
	}

	// 원본 인덱스 중 최대값을 찾아 1D 텍스처의 너비 결정
	_uint maxOriginalIndex = 0;
	if (!m_originalToActualIndexMap.empty()) {
		// C++11 이상에서 std::map의 키는 정렬되어 있으므로, rbegin()->first로 최대 키를 가져올 수 있음.
		// 또는 루프를 돌려 찾아야 함.
		for (const auto& pair : m_originalToActualIndexMap) {
			if (pair.first > maxOriginalIndex) {
				maxOriginalIndex = pair.first;
			}
		}
	}

	_uint textureWidth = static_cast<_uint>(maxOriginalIndex + 1);
	if (textureWidth == 0) return S_OK; // 매핑할 인덱스가 없음

	std::vector<float> gpuIndexData(textureWidth, -1.0f); // 기본값 -1.0f (유효하지 않은 매핑)

	for (const auto& pair : m_originalToActualIndexMap) {
		if (pair.first >= 0 && static_cast<_uint>(pair.first) < textureWidth) {
			gpuIndexData[pair.first] = static_cast<float>(pair.second); // 원본 인덱스 위치에 실제 SRV 인덱스 저장
		}
	}

	D3D11_TEXTURE1D_DESC texDesc = {};
	texDesc.Width = textureWidth;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_FLOAT; // 실제 인덱스 저장용
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	// texDesc.CPUAccessFlags = 0;
	// texDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = gpuIndexData.data();
	// initData.SysMemPitch = 0; // 1D 텍스처는 SysMemPitch 불필요
	// initData.SysMemSlicePitch = 0; // 1D 텍스처는 SysMemSlicePitch 불필요

	HRESULT hr = m_pDevice->CreateTexture1D(&texDesc, &initData, &m_pIndexMapTexture);
	if (FAILED(hr)) {
		MSG_BOX("Failed to create IndexMapTexture (Texture1D)");
		return E_FAIL;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	srvDesc.Texture1D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture1D.MostDetailedMip = 0;

	hr = m_pDevice->CreateShaderResourceView(m_pIndexMapTexture, &srvDesc, &m_pIndexMapSRV);
	if (FAILED(hr)) {
		MSG_BOX("Failed to create SRV for IndexMapTexture");
		Safe_Release(m_pIndexMapTexture); // 생성된 텍스처도 해제
		return E_FAIL;
	}

	return S_OK;
}

void CTextureArray::Free()
{
	__super::Free();

	for (auto& pSRV : m_SRVs)
		Safe_Release(pSRV);

	m_SRVs.clear();

	Safe_Release(m_pTextureArrayResource);

	Safe_Release(m_pIndexMapTexture);
	Safe_Release(m_pIndexMapSRV);


}
#pragma endregion

#pragma region Save&Load

HRESULT CTextureArray::Serialize(json& outJson)
{
	return S_OK;
}

HRESULT CTextureArray::Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

HRESULT CTextureArray::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CTextureArray";
	outJson["TextureFilePath"] = WideToUtf8(m_strTextureFilePath.c_str()).c_str();
	outJson["NumTexture"] = m_iNumTextures;
	return S_OK;
}

HRESULT CTextureArray::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	using namespace simdjson;

	string_view texturePathSV;

	if (jsonData["TextureFilePath"].get_string().get(texturePathSV) != simdjson::SUCCESS) {
		MSG_BOX("CTextureArray::Prototype_Deserialize - 'TextureFilePath' field is missing or not a string.");
		return E_FAIL;
	}

	string texturePathStr(texturePathSV);

	size_t iNumTextures = 0;

	if (jsonData["NumTexture"].get_uint64().get(iNumTextures) != simdjson::SUCCESS) {
		MSG_BOX("CTextureArray::Prototype_Deserialize - 'NumTexture' field is missing or not a number.");
		return E_FAIL;
	}
	Initialize_Prototype(Utf8ToWide(texturePathStr).c_str(), static_cast<_uint>(iNumTextures));

	return S_OK;
}

#pragma endregion

HRESULT CTextureArray::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iTextureIndex)
{
	if (iTextureIndex >= m_iNumTextures)
		return E_FAIL;

	return pShader->Bind_SRV(pConstantName, m_SRVs[iTextureIndex]);
}

HRESULT CTextureArray::Bind_ShaderResourceArray(CShader* pShader, const _char* pConstantName)
{
	return S_OK;
}

HRESULT CTextureArray::Bind_IndexMap(CShader* pShader, const _char* pConstantName)
{
	if (nullptr == pShader || nullptr == m_pIndexMapSRV) {
		return E_FAIL;
	}
	return pShader->Bind_SRV(pConstantName, m_pIndexMapSRV);
}

HRESULT CTextureArray::RenderImGui()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("TextureArray", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Numbers of Texture: %d", m_iNumTextures);
		ImGui::Text(WideToUtf8(m_strTextureFilePath.c_str()).c_str());
		//for (_uint i = 0; i < m_iNumTextures; ++i)
		//{
		//	ImGui::Indent();
		//	if (ImGui::CollapsingHeader(to_string(i).c_str()))
		//	{
		//		ImGui::Image((ImTextureID)(m_SRVs[i]), ImVec2(100, 100), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(0.5f, 0.5f, 0.5f, 1)); // 테두리 색상 추가
		//		if (ImGui::IsItemHovered()) {
		//			ImGui::BeginTooltip();
		//			ImGui::Text("SRV: 0x%p", (void*)m_SRVs[i]);
		//			ImGui::Image((ImTextureID)((void*)m_SRVs[i]), ImVec2(256, 256)); // 툴팁에 더 큰 이미지
		//			ImGui::EndTooltip();
		//		}
		//	}
		//	ImGui::Unindent();
		//}
	}
	ImGui::PopID();
	return S_OK;
}




