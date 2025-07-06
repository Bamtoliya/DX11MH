#include "VIBuffer_Terrain.h"

namespace fs = std::filesystem;

#pragma region Constructor/Destructor
CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& Prototype)
	: CVIBuffer{ Prototype }
	, m_iNumVerticesX{ Prototype.m_iNumVerticesX }
	, m_iNumVerticesZ{ Prototype.m_iNumVerticesZ }
	, m_strHeightMapFilePath { Prototype.m_strHeightMapFilePath }
	, m_iNumMaps { Prototype.m_iNumMaps }
	, m_iGridWidth{ Prototype.m_iGridWidth }
	, m_iGridHeight{ Prototype.m_iGridHeight }
{

}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);
	return pInstance;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath, _uint iNumMaps, _uint iGridWidth, _uint iGridHeight)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pHeightMapFilePath, iNumMaps, iGridWidth, iGridHeight)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CVIBuffer_Terrain::LoadHeightMapPixelsFromDDS(const _tchar* filePath, _uint& width, _uint& height, vector<_float>& heightData)
{
	if (!m_pDevice || !filePath) return E_INVALIDARG;

	ID3D11DeviceContext* pInitContext = GetInitContext();
	_tchar			szTexturePath[MAX_PATH] = {};
	_tchar			szExt[MAX_PATH] = {};
	m_strHeightMapFilePath = filePath;

	_wsplitpath_s(filePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
	wsprintf(szTexturePath, filePath, 0);

	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImage;

	HRESULT hr = DirectX::LoadFromDDSFile(szTexturePath, DirectX::DDS_FLAGS_NONE, &metadata, scratchImage);

	_uint gridTexWidth = static_cast<_uint>(metadata.width);
	_uint gridTexHeight = static_cast<_uint>(metadata.height);

	width = gridTexWidth * m_iGridWidth + 1;
	height = gridTexHeight * m_iGridHeight + 1;
	DXGI_FORMAT gridTexFormat = metadata.format;
	//m_fHeightScale = static_cast<_float>(gridTexHeight);
	heightData.resize(width * height, 0.0f);

	// Phase 2: 각 그리드 파일을 병렬로 로드 및 데이터 복사
 	vector<future<HRESULT>> futures;

	for (_uint h = 0; h < m_iGridHeight; ++h)
	{
		for (_uint w = 0; w < m_iGridWidth; ++w)
		{
			_uint fileIndex = h * m_iGridWidth + w;
			_tchar szGridFilePath[MAX_PATH] = {};
			wsprintf(szGridFilePath, filePath, fileIndex);

			hr = DirectX::LoadFromDDSFile(szGridFilePath, DirectX::DDS_FLAGS_NONE, &metadata, scratchImage);
			if (FAILED(hr))
			{
				_tchar debugMsg[256];
				swprintf_s(debugMsg, 256, L"Failed to load DDS heightmap grid file: %s\n", szGridFilePath);
				OutputDebugString(debugMsg);
				// 모든 그리드 파일이 반드시 존재해야 하는지, 아니면 건너뛸 수 있는지에 따라 오류 처리 변경
				return E_FAIL;
			}

			// 밉 레벨 0 데이터만 사용 (가장 큰 해상도)
			const DirectX::Image* image = scratchImage.GetImage(0, 0, 0); // mipLevel, arraySlice, face
			if (!image)
			{
				MSG_BOX("Failed to get image data from loaded DDS.");
				return E_FAIL;
			}

			// 그리드 텍스처의 시작 위치 계산
			// 그리드 텍스처의 (0,0)이 전체 높이맵의 어디에 매핑될지
			// 각 그리드 텍스처는 (gridTexWidth-1) x (gridTexHeight-1) 개의 사각형을 표현한다고 보면 됩니다.
			// 따라서 다음 그리드는 (gridTexWidth-1) 만큼 이동합니다.
			_uint startGlobalX = w * (gridTexWidth);
			_uint startGlobalY = h * (gridTexHeight);


			futures.push_back(std::async(std::launch::async,
				[=, &heightData, &gridTexFormat, &width, &height]() -> HRESULT {

					DirectX::ScratchImage localScratchImage; // 각 스레드마다 로컬 ScratchImage 사용
					HRESULT local_hr = DirectX::LoadFromDDSFile(szGridFilePath, DirectX::DDS_FLAGS_NONE, nullptr, localScratchImage);
					if (FAILED(local_hr)) {
						OutputDebugStringW((std::wstring(L"Failed to load DDS heightmap grid file: ") + szGridFilePath + L"\n").c_str());
						return E_FAIL;
					}

					const DirectX::Image* image = localScratchImage.GetImage(0, 0, 0);
					if (!image) {
						OutputDebugStringW(L"Failed to get image data from loaded DDS in async task.\n");
						return E_FAIL;
					}

					const BYTE* pSrcBits = image->pixels;
					size_t rowPitch = image->rowPitch;

					// 픽셀 데이터 추출 및 병합 (heightData에 직접 쓰기)
					for (_uint y_local = 0; y_local < image->height; ++y_local)
					{
						for (_uint x_local = 0; x_local < image->width; ++x_local)
						{
							float normalizedHeight = 0.0f;
							size_t bytesPerPixel  = BitsPerPixel(gridTexFormat) / 8;
							size_t pixelOffset = (y_local * rowPitch) + (x_local * bytesPerPixel);

							switch (gridTexFormat)
							{
							case DXGI_FORMAT_R8_UNORM:
							{
								normalizedHeight = static_cast<_float>(pSrcBits[pixelOffset]) / 255.0f;
								break;
							}
							case DXGI_FORMAT_R16_UNORM:
							{
								const _ushort* pPixel = reinterpret_cast<const _ushort*>(pSrcBits + pixelOffset);
								normalizedHeight = static_cast<_float>(*pPixel) / 65535.0f;
								break;
							}
							case DXGI_FORMAT_R32_FLOAT:
							{
								const _float* pPixel = reinterpret_cast<const _float*>(pSrcBits + pixelOffset);
								normalizedHeight = *pPixel;
								break;
							}
							case DXGI_FORMAT_R8G8B8A8_UNORM:
							case DXGI_FORMAT_B8G8R8A8_UNORM:
							{
								normalizedHeight = static_cast<_float>(pSrcBits[pixelOffset]) / 255.0f; // Red/Blue 채널 (일반적으로 Red 채널을 높이로 사용)
								break;
							}
							default:
							{
								OutputDebugStringW(L"Unsupported DDS pixel format for heightmap in async task.\n");
								return E_FAIL;
							}
							}

							_uint globalX = startGlobalX + x_local;
							_uint globalY = startGlobalY + y_local;
							size_t globalIndex = static_cast<size_t>(globalY) * width + globalX;

							if (globalIndex < heightData.size()) // 경계 확인
							{
								heightData[globalIndex] = normalizedHeight;
							}
						}
					}
					return S_OK;
				}));
		}
	}

	// 모든 비동기 작업이 완료될 때까지 대기하고 결과 확인
	for (auto& future : futures)
	{
		if (FAILED(future.get())) // future.get()은 작업이 완료될 때까지 대기
		{
			MSG_BOX("One or more heightmap grid loading tasks failed.");
			return E_FAIL;
		}
	}

	// 마지막 행/열 처리 (+1 경계값)
	for (_uint y = 0; y < height - 1; ++y) {
		heightData[y * width + (width - 1)] = heightData[y * width + (width - 2)];
	}
	
	for (_uint x = 0; x < width; ++x) {
		heightData[(height - 1) * width + x] = heightData[(height - 2) * width + x];
	}

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightMapFilePath, _uint iNumMaps, _uint iGridWidth, _uint iGridHeight)
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
	Safe_Delete_Array(m_pVertexPositions);
	m_iNumMaps = iNumMaps;
	m_iGridWidth = iGridWidth;
	m_iGridHeight = iGridHeight;

	if (pHeightMapFilePath == nullptr || wcslen(pHeightMapFilePath) == 0 || m_iNumMaps != m_iGridHeight * m_iGridWidth) {
		MSG_BOX("Heightmap file path is null or empty.");
		return E_INVALIDARG;
	}

	m_strHeightMapFilePath = pHeightMapFilePath; // 경로 저장
	fs::path filePathObj(pHeightMapFilePath);
	std::wstring fileExtension = filePathObj.extension().wstring();
	std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::towlower); // 소문자로 변환

	vector<_float> heightValues; // 0.0f ~ 1.0f 범위의 정규화된 높이값 저장

	if (fileExtension == L".dds")
	{
		unsigned int ddsWidth = 0, ddsHeight = 0;
		HRESULT hr = LoadHeightMapPixelsFromDDS(pHeightMapFilePath, ddsWidth, ddsHeight, heightValues);
		if (FAILED(hr)) {
			MSG_BOX("Failed to load DDS heightmap.");
			return hr;
		}
		m_iNumVerticesX = ddsWidth;
		m_iNumVerticesZ = ddsHeight;
	}
	else if (fileExtension == L".bmp")
	{
		HANDLE hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (INVALID_HANDLE_VALUE == hFile) {
			MSG_BOX("Failed to open BMP file: ");
			return E_FAIL;	
		}

		_ulong dwByte = {};
		BITMAPFILEHEADER fh{};
		BITMAPINFOHEADER ih{};

		if (!ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr) || dwByte != sizeof fh ||
			!ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr) || dwByte != sizeof ih) {
			CloseHandle(hFile); MSG_BOX("Failed to read BMP header."); return E_FAIL;
		}

		m_iNumVerticesX = ih.biWidth;
		m_iNumVerticesZ = ih.biHeight;
 		if (m_iNumVerticesX <= 0 || m_iNumVerticesZ <= 0) {
			CloseHandle(hFile); MSG_BOX("BMP has invalid dimensions."); return E_FAIL;
		}

		_uint numPixels = m_iNumVerticesX * m_iNumVerticesZ;
		_uint* pBmpPixels = new _uint[numPixels]; // BMP는 보통 픽셀당 3 또는 4바이트 (여기서는 _uint 가정)
		if (!ReadFile(hFile, pBmpPixels, sizeof(_uint) * numPixels, &dwByte, nullptr) || dwByte != sizeof(_uint) * numPixels) {
			Safe_Delete_Array(pBmpPixels); CloseHandle(hFile); MSG_BOX("Failed to read BMP pixel data."); return E_FAIL;
		}
		CloseHandle(hFile);

		heightValues.resize(numPixels);
		for (_uint i = 0; i < numPixels; ++i) {
			// BMP의 R 채널을 높이로 사용 (0-255 범위), 정규화
			heightValues[i] = static_cast<float>(pBmpPixels[i] & 0x000000ff) / 255.0f;
		}
		Safe_Delete_Array(pBmpPixels);
	}
	else
	{
		MSG_BOX("Unsupported heightmap file format: ");
		return E_FAIL;
	}

	if (m_iNumVerticesX == 0 || m_iNumVerticesZ == 0) {
		MSG_BOX("Heightmap dimensions are zero after loading.");
		return E_FAIL;
	}

#pragma region Vertex
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXNORTEX);
	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_iIndexStride = sizeof(_uint);
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	D3D11_BUFFER_DESC			VBBufferDesc{};
	VBBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VBBufferDesc.CPUAccessFlags = /*D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE*/0;
	VBBufferDesc.StructureByteStride = m_iVertexStride;
	VBBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA		VBInitialData{};

	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	// 지형의 전체 너비와 깊이 (정점 간 간격이 1.0f라고 가정)
	_float terrainActualWidth = static_cast<_float>(m_iNumVerticesX);
	_float terrainActualDepth = static_cast<_float>(m_iNumVerticesZ);

	// 지형의 절반 너비와 절반 깊이
	_float halfWidth = terrainActualWidth / 2.0f;
	_float halfDepth = terrainActualDepth / 2.0f;

	for (size_t i = 0; i < m_iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			_uint		iIndex = (_uint)i * m_iNumVerticesX + (_uint)j;

			_float xPos = static_cast<_float>(j); //-halfWidth;
			_float zPos = static_cast<_float>(i); //-halfDepth;

			// 좌우 및 상하 반전을 위해 인덱스 계산
			_uint flipped_i = m_iNumVerticesZ - 1 - (_uint)i;
			_uint flipped_j = m_iNumVerticesX - 1 - (_uint)j;
			_float actualHeight = heightValues[flipped_i * m_iNumVerticesX + (_uint)j] * m_fHeightScale;

			pVertices[iIndex].vPosition = _float3(xPos, (actualHeight), zPos);
			m_pVertexPositions[iIndex] = pVertices[iIndex].vPosition;
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
		}
	}

	VBInitialData.pSysMem = pVertices;
#pragma endregion

#pragma region Index
	D3D11_BUFFER_DESC			IBBufferDesc{};
	IBBufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	IBBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IBBufferDesc.CPUAccessFlags = /*D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE*/0;
	IBBufferDesc.StructureByteStride = m_iIndexStride;
	IBBufferDesc.MiscFlags = 0;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint)* m_iNumIndices);

	_uint	iNumIndices = { 0 };

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint		iIndex = (_uint)i * m_iNumVerticesX + (_uint)j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector		vSourDir, vDestDir, vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			vSourDir = XMLoadFloat3(&m_pVertexPositions[iIndices[1]]) - XMLoadFloat3(&m_pVertexPositions[iIndices[0]]);
			vDestDir = XMLoadFloat3(&m_pVertexPositions[iIndices[2]]) - XMLoadFloat3(&m_pVertexPositions[iIndices[1]]);
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal
				, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal
				, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal
				, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			vSourDir = XMLoadFloat3(&m_pVertexPositions[iIndices[2]]) - XMLoadFloat3(&m_pVertexPositions[iIndices[0]]);
			vDestDir = XMLoadFloat3(&m_pVertexPositions[iIndices[3]]) - XMLoadFloat3(&m_pVertexPositions[iIndices[2]]);
			vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal
				, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal
				, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal
				, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);
		}
	}
#pragma endregion
	
#pragma region Normal
	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		XMStoreFloat3(&pVertices[i].vNormal
			, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
	}
#pragma endregion

	
	D3D11_SUBRESOURCE_DATA		IBInitialData{};
	IBInitialData.pSysMem = pIndices;


	if (FAILED(m_pDevice->CreateBuffer(&VBBufferDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);


	if (FAILED(m_pDevice->CreateBuffer(&IBBufferDesc, &IBInitialData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	heightValues.clear();


	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();
}
#pragma endregion

HRESULT CVIBuffer_Terrain::RenderImGui()
{
	ImGui::PushID(this);
	static char tempHeightMapPath[512] = "";
	const char* dndPayloadType = "DND_ASSET_PATH"; // 에셋 브라우저에서 사용할 페이로드 타입

#pragma region 팝업그리기
	if (ifd::FileDialog::Instance().IsDone("HeightMapOpenDialog_VIBufferTerrain")) {
		if (ifd::FileDialog::Instance().HasResult()) {
			fs::path absoluteFilePath = ifd::FileDialog::Instance().GetResult().u8string(); // 절대 경로 (UTF-8)
			fs::path assetRootPath = fs::absolute(ASSETPATH); // ASSETPATH를 절대 경로로 변환

			// 절대 경로를 ASSETPATH 기준으로 상대 경로로 변환
			fs::path relativeFilePath = fs::relative(absoluteFilePath, assetRootPath);
			std::string relativePathStr = ASSETPATH + relativeFilePath.generic_u8string(); // 플랫폼 독립적인 UTF-8 경로 문자열

			strncpy_s(tempHeightMapPath, sizeof(tempHeightMapPath), relativePathStr.c_str(), _TRUNCATE);
			// printf("Selected via Dialog (Relative): [%s]\n", relativePathStr.c_str()); // 디버깅용
		}
		ifd::FileDialog::Instance().Close();
	}
#pragma endregion

	if (ImGui::CollapsingHeader("VIBuffer_Terrain", ImGuiTreeNodeFlags_DefaultOpen)) // 기본적으로 열려있도록 플래그 추가
	{
		ImGui::Text("Current HeightMap: %ls", m_strHeightMapFilePath.c_str());
		ImGui::DragFloat("Height Scale", &m_fHeightScale, 0.1f, 0.1f, 1024.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		if (ImGui::Button("Edit Path##HeightMap"))
		{
			std::string utf8Path = WideToUtf8(m_strHeightMapFilePath.c_str());
			strncpy_s(tempHeightMapPath, sizeof(tempHeightMapPath), utf8Path.c_str(), _TRUNCATE);
		}

		ImGui::InputText("New Path", tempHeightMapPath, sizeof(tempHeightMapPath));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(dndPayloadType))
			{
				const char* absolutePathFromPayload = (const char*)payload->Data; // 페이로드는 절대 경로(UTF-8)라고 가정
				if (absolutePathFromPayload)
				{
					fs::path droppedAbsolutePath = absolutePathFromPayload;
					fs::path assetRootPath = fs::absolute(ASSETPATH);

					fs::path relativePath = fs::relative(droppedAbsolutePath, assetRootPath);
					std::string relativePathStr = relativePath.generic_u8string();

					strncpy_s(tempHeightMapPath, sizeof(tempHeightMapPath), relativePathStr.c_str(), _TRUNCATE);
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();
		if (ImGui::Button("Browse...##HeightMap"))
		{
			ifd::FileDialog::Instance().Open("HeightMapOpenDialog_VIBufferTerrain", "Open a texture", "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga;*.dds){.png,.jpg,.jpeg,.bmp,.tga,.dds},.*", false, ASSETPATH);
		}

		if (ImGui::Button("Apply New HeightMap"))
		{
			std::string newPathUtf8 = tempHeightMapPath;
			if (!newPathUtf8.empty())
			{
				std::wstring newPathWide = Utf8ToWide(newPathUtf8.c_str());
				if (FAILED(Initialize_Prototype(newPathWide.c_str())))
				{
					MSG_BOX("Failed to apply new HeightMap. Check file path and format.");
					// 실패 시, tempHeightMapPath를 이전 m_strHeightMapFilePath로 되돌릴 수 있습니다.
					// 예를 들어, Initialize_Prototype 실패 시 m_strHeightMapFilePath는 변경되지 않으므로
					// 다음 "Edit Path" 클릭 시 다시 올바른 경로로 tempHeightMapPath가 채워집니다.
				}
				else
				{
					MSG_BOX("New HeightMap applied successfully.");
					// 성공 시 m_strHeightMapFilePath는 이미 newPathWide로 업데이트 되었음
				}
			}
			else
			{
				MSG_BOX("New path is empty. Cannot apply.");
			}
		}
	}
	ImGui::PopID();
	return S_OK;
}

#pragma region Save&Load

HRESULT CVIBuffer_Terrain::Serialize(json& outJson)
{
	json data;
	if(FAILED(__super::Serialize(data)))
		return E_FAIL;
	data["HeightMapFilePath"] = WideToUtf8(m_strHeightMapFilePath.c_str()).c_str();
	data["HeightScale"] = m_fHeightScale;
	data["Maps"] = m_iNumMaps;
	data["GridSize"] = { m_iGridWidth, m_iGridHeight };
	outJson["CVIBuffer_Terrain"] = data;

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;

	simdjson::ondemand::object terrainJsonData;
	auto error = jsonData["CVIBuffer_Terrain"].get_object().get(terrainJsonData);
	if (error) // "CVIBuffer_Terrain" 객체 자체가 없는 경우
	{
		// CVIBuffer_Terrain 객체가 없으면 기본값으로 초기화 시도
		MSG_BOX("Warning: CVIBuffer_Terrain data not found in JSON. Initializing with default heightmap path.");
		m_strHeightMapFilePath = L""; // 기본 빈 경로
		m_fHeightScale = 10.0f;       // 기본 높이 스케일
		if (FAILED(Initialize_Prototype(m_strHeightMapFilePath.c_str())))
		{
			// 기본 경로로 초기화 실패 시 (예: 빈 경로가 허용되지 않는 경우)
			MSG_BOX("Failed to initialize with default empty heightmap path during Deserialize.");
			// 이 경우 E_FAIL을 반환하거나, 또는 게임 로직에 따라 다른 처리를 할 수 있습니다.
			// 여기서는 E_FAIL을 반환하여 로딩 실패를 알립니다.
			return E_FAIL;
		}
		return S_OK; // 기본값으로 초기화 성공 (또는 시도)
	}

	// HeightMapFilePath 처리
	string_view heightMapPath_sv;
	error = terrainJsonData["HeightMapFilePath"].get_string().get(heightMapPath_sv);
	if (error) // 키가 없거나 타입이 맞지 않는 경우
	{
		MSG_BOX("Warning: HeightMapFilePath not found or invalid in JSON. Using default empty path.");
		m_strHeightMapFilePath = L""; // 기본값: 빈 문자열
	}
	else
	{
		string heightMapPath_utf8(heightMapPath_sv);
		m_strHeightMapFilePath = Utf8ToWide(heightMapPath_utf8.c_str());
	}

	// HeightScale 처리
	double heightScaleValue;
	error = terrainJsonData["HeightScale"].get_double().get(heightScaleValue);
	if (error) // 키가 없거나 타입이 맞지 않는 경우
	{
		MSG_BOX("Warning: HeightScale not found or invalid in JSON. Using default value 10.0f.");
		m_fHeightScale = 10.0f; // 기본값
	}
	else
	{
		m_fHeightScale = static_cast<_float>(heightScaleValue);
	}

	// HeightScale 처리
	unsigned long long iNumMpas;
	error = terrainJsonData["Maps"].get_uint64().get(iNumMpas);
	if (error) // 키가 없거나 타입이 맞지 않는 경우
	{
		MSG_BOX("Warning: HeightScale not found or invalid in JSON. Using default value 10.0f.");
		m_iNumMaps = 1; // 기본값
	}
	else
	{
		m_iNumMaps = static_cast<_uint>(iNumMpas);
	}

	simdjson::ondemand::array arr;
	error = terrainJsonData["GridSize"].get_array().get(arr);
	if (error) // 키가 없거나 타입이 맞지 않는 경우
	{
		MSG_BOX("Warning: HeightScale not found or invalid in JSON. Using default value 10.0f.");
		m_iGridWidth = 1; // 기본값
		m_iGridHeight = 1; // 기본값
	}
	else
	{
		auto iter = arr.begin();
		m_iGridWidth = static_cast<_uint>((*iter).get_uint64().value_unsafe());
		++iter;
		m_iGridHeight = static_cast<_uint>((*iter).get_uint64().value_unsafe());
	}

	if (FAILED(Initialize_Prototype(m_strHeightMapFilePath.c_str(), m_iNumMaps, m_iGridWidth, m_iGridHeight)))
	{
		// 경로가 유효하지 않거나 (예: 기본값인 빈 경로) 파일 로드 실패 시
		// Initialize_Prototype 내부에서 이미 MSG_BOX를 호출할 것이므로 여기서는 추가 메시지 없이 실패 반환
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CVIBuffer_Terrain";
	outJson["HeightMapFilePath"] = WideToUtf8(m_strHeightMapFilePath.c_str()).c_str();
	return S_OK;
}

HRESULT CVIBuffer_Terrain::Prototype_Deserialize(simdjson::ondemand::object & jsonData)
{
	string_view heightMapPath_sv;
	auto error = jsonData["HeightMapFilePath"].get_string().get(heightMapPath_sv);
	if (error) {
		return E_FAIL;
	}
	string heightMapPath_utf8 = string(heightMapPath_sv);

	m_strHeightMapFilePath = Utf8ToWide(heightMapPath_utf8.c_str());

	if (FAILED(Initialize_Prototype(m_strHeightMapFilePath.c_str())))	
		return E_FAIL;

	return S_OK;
}

#pragma endregion