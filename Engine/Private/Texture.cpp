    #include "Texture.h"
    #include "Shader.h"
    #include "GameInstance.h"
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
	    m_originalToActualIndexMap[(_uint)i] = iActualIndex++;
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

        // 추가로 해제해야 할 리소스들
        Safe_Release(m_pTextureArrayResource);
        Safe_Release(m_pIndexMapTexture);
        Safe_Release(m_pIndexMapSRV);
    }
    #pragma endregion

    #pragma region Save&Load

    HRESULT CTexture::Serialize(json& outJson)
    {
    // 부모 클래스 직렬화 호출
    if (FAILED(__super::Serialize(outJson)))
        return E_FAIL;

    // 텍스처 파일 경로 저장
    outJson["TextureFilePath"] = WideToUtf8(m_strTextureFilePath.c_str()).c_str();

    // 텍스처 수 저장
    outJson["NumTextures"] = m_iNumTextures;

    outJson["UseTextureRegion"] = m_bUseRegion;
    if (m_bUseRegion)
    {
        json regionJson = json::array();
        regionJson.push_back(m_vTextureRegion.x);
        regionJson.push_back(m_vTextureRegion.y);
        regionJson.push_back(m_vTextureRegion.z);
        regionJson.push_back(m_vTextureRegion.w);
        outJson["TextureRegion"] = regionJson;
    }

    // 프로토타입 정보 저장 (변경된 경우에만)
    if (!m_strPrototypeTag.empty())
    {
        outJson["PrototypeTag"] = WideToUtf8(m_strPrototypeTag.c_str()).c_str();
    }

    return S_OK;
    }

    HRESULT CTexture::Deserialize(simdjson::ondemand::object & jsonData)
    {
        using namespace simdjson;

        // 부모 클래스 역직렬화 호출
        if (FAILED(__super::Deserialize(jsonData)))
            return E_FAIL;

        bool useRegion = false;
        if (jsonData["UseTextureRegion"].get_bool().get(useRegion) == simdjson::SUCCESS)
    {
        m_bUseRegion = useRegion;

        if (m_bUseRegion)
        {
            simdjson::ondemand::array regionArray;
            if (jsonData["TextureRegion"].get_array().get(regionArray) == simdjson::SUCCESS)
            {
                std::vector<double> values;
                for (auto value : regionArray)
                {
                    double val;
                    if (value.get_double().get(val) == simdjson::SUCCESS)
                    {
                        values.push_back(val);
                    }
                }

                if (values.size() == 4)
                {
                    m_vTextureRegion.x = static_cast<float>(values[0]);
                    m_vTextureRegion.y = static_cast<float>(values[1]);
                    m_vTextureRegion.z = static_cast<float>(values[2]);
                    m_vTextureRegion.w = static_cast<float>(values[3]);
                }
            }
        }
    }

        // 프로토타입 정보 확인 (프로토타입 태그가 있는 경우)
        string_view prototypeTagSV;
        uint64_t prototypeLevelIndex = 0;

        if (jsonData["PrototypeTag"].get_string().get(prototypeTagSV) == simdjson::SUCCESS)
    {
        // 프로토타입 정보 복원
        _wstring strPrototypeTag = Utf8ToWide(string(prototypeTagSV));

        // GameInstance를 통해 프로토타입 찾기
        CComponent* pProtoTexture = m_pGameInstance->Find_Prototype(
            PROTOTYPE::TYPE_COMPONENT,
            static_cast<_uint>(prototypeLevelIndex),
            strPrototypeTag);

        if (pProtoTexture)
        {
            CTexture* pTexture = dynamic_cast<CTexture*>(pProtoTexture);
            if (pTexture)
            {
                // 기존 리소스 해제
                for (auto& pSRV : m_SRVs)
                    Safe_Release(pSRV);
                m_SRVs.clear();

                // 새 프로토타입의 정보 복사
                m_strTextureFilePath = pTexture->m_strTextureFilePath;
                m_iNumTextures = pTexture->m_iNumTextures;
                m_strPrototypeTag = strPrototypeTag;

                // SRV 복사 (AddRef 필요)
                for (auto& pSRV : pTexture->m_SRVs)
                {
                    Safe_AddRef(pSRV);
                    m_SRVs.push_back(pSRV);
                }

                return S_OK;
            }
        }
    }

        // 프로토타입 정보가 없거나 찾지 못한 경우, 파일 경로에서 직접 로드
        string_view texturePathSV;
        if (jsonData["TextureFilePath"].get_string().get(texturePathSV) == simdjson::SUCCESS)
    {
        string texturePathStr(texturePathSV);
        uint64_t numTextures = 1;
        jsonData["NumTextures"].get_uint64().get(numTextures); // 실패해도 기본값 1 사용

        // 기존 리소스 해제
        for (auto& pSRV : m_SRVs)
            Safe_Release(pSRV);
        m_SRVs.clear();

        // 텍스처 로드
        return Initialize_Prototype(Utf8ToWide(texturePathStr).c_str(), static_cast<_uint>(numTextures));
    }

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

    void CTexture::Set_TextureRegionPixels(_float fLeft, _float fTop, _float fRight, _float fBottom, _uint iTextureIndex)
    {
    if (iTextureIndex >= m_iNumTextures)
        return;

    // 픽셀 좌표를 _float4로 변환
    _float4 pixelCoords = { fLeft, fTop, fRight, fBottom };

    // 픽셀 좌표를 UV 좌표로 변환
    m_vTextureRegion = PixelsToUV(pixelCoords, iTextureIndex);
    }

    void CTexture::Set_TextureRegionUV(_float fLeft, _float fTop, _float fRight, _float fBottom)
    {
    m_vTextureRegion = _float4(fLeft, fTop, fRight, fBottom);
    }

    _float4 CTexture::PixelsToUV(_float4 pixelCoords, _uint iTextureIndex)
    {
    if (iTextureIndex >= m_iNumTextures)
        return _float4(0.0f, 0.0f, 1.0f, 1.0f);

    // 텍스처 크기 얻기
    D3D11_TEXTURE2D_DESC textureDesc;
    ID3D11Texture2D* pTexture2D = nullptr;

    m_SRVs[iTextureIndex]->GetResource((ID3D11Resource**)&pTexture2D);
    if (pTexture2D)
    {
        pTexture2D->GetDesc(&textureDesc);
        pTexture2D->Release();

        // 픽셀 좌표를 UV 좌표(0~1)로 변환
        _float4 uvCoords;
        uvCoords.x = max(0.0f, min(1.0f, (float)pixelCoords.x / textureDesc.Width));
        uvCoords.y = max(0.0f, min(1.0f, (float)pixelCoords.y / textureDesc.Height));
        uvCoords.z = max(0.0f, min(1.0f, (float)pixelCoords.z / textureDesc.Width));
        uvCoords.w = max(0.0f, min(1.0f, (float)pixelCoords.w / textureDesc.Height));

        return uvCoords;
    }
    // 실패 시 전체 텍스처 영역 반환
    return _float4(0.0f, 0.0f, 1.0f, 1.0f);
    }

    _float2 CTexture::Get_TextureSize(_uint iTextureIndex)
    {
    if (iTextureIndex >= m_iNumTextures)
        return _float2(0, 0);

    D3D11_TEXTURE2D_DESC textureDesc;
    ID3D11Texture2D* pTexture2D = nullptr;
    m_SRVs[iTextureIndex]->GetResource((ID3D11Resource**)&pTexture2D);
    if (pTexture2D)
    {
        pTexture2D->GetDesc(&textureDesc);
        pTexture2D->Release();
        return _float2((_float)textureDesc.Width, (_float)textureDesc.Height);
    }
    return _float2(0, 0);
    }

    HRESULT CTexture::Bind_ShaderResourceWithRegion(CShader* pShader, const _char* pConstantName, const _char* pRegionConstantName, _uint iTextureIndex)
    {
    if (iTextureIndex >= m_iNumTextures)
        return E_FAIL;

    // 텍스처 바인딩
    if (FAILED(pShader->Bind_SRV(pConstantName, m_SRVs[iTextureIndex])))
        return E_FAIL;

    pShader->Bind_RawValue("g_bUseTextureRegion", &m_bUseRegion, sizeof(_bool)); // 영역 사용 활성화
    // 텍스처 영역 정보도 셰이더에 전달
    if (FAILED(pShader->Bind_RawValue(pRegionConstantName, &m_vTextureRegion, sizeof(_float4))))
        return E_FAIL;

    return S_OK;
    }

    #pragma endregion

    HRESULT CTexture::ReloadTexture(const _tchar* pTextureFilePath, _uint iNumTextures)
    {
    // 기존 리소스 해제
    for (auto& pSRV : m_SRVs)
	    Safe_Release(pSRV);
    m_SRVs.clear();

    // 새 텍스처 로드
    return Initialize_Prototype(pTextureFilePath, iNumTextures);
    }

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
        // 현재 텍스처 정보 표시 (기존 코드)
        ImGui::Text("Current Texture Path: %s", WideToUtf8(m_strTextureFilePath.c_str()).c_str());
        ImGui::Text("Numbers of Texture: %d", m_iNumTextures);

        // 텍스처 영역 설정 UI 추가
        if (ImGui::CollapsingHeader("Texture Region", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // 텍스처 영역 사용 여부 체크박스
            ImGui::Checkbox("Use Texture Region", &m_bUseRegion);

            if (m_bUseRegion)
            {
                // 텍스처 크기 정보 가져오기 (첫 번째 텍스처만 사용)
                _float2 textureSize = Get_TextureSize(0);

                // 현재 영역 값을 픽셀 단위로 변환 (UI 표시용)
                int pixelLeft = static_cast<int>(m_vTextureRegion.x * textureSize.x);
                int pixelTop = static_cast<int>(m_vTextureRegion.y * textureSize.y);
                int pixelRight = static_cast<int>(m_vTextureRegion.z * textureSize.x);
                int pixelBottom = static_cast<int>(m_vTextureRegion.w * textureSize.y);

                // 픽셀 단위로 영역 설정 UI
                bool regionChanged = false;
                ImGui::Text("Texture Size: %.0f x %.0f", textureSize.x, textureSize.y);

                // 픽셀 값 슬라이더 (정수형으로 표시)
                regionChanged |= ImGui::SliderInt("Left (Pixels)", &pixelLeft, 0, static_cast<int>(textureSize.x));
                regionChanged |= ImGui::SliderInt("Top (Pixels)", &pixelTop, 0, static_cast<int>(textureSize.y));
                regionChanged |= ImGui::SliderInt("Right (Pixels)", &pixelRight, pixelLeft, static_cast<int>(textureSize.x));
                regionChanged |= ImGui::SliderInt("Bottom (Pixels)", &pixelBottom, pixelTop, static_cast<int>(textureSize.y));

                // 영역 리셋 버튼
                if (ImGui::Button("Reset Region"))
                {
                    m_vTextureRegion = _float4(0.0f, 0.0f, 1.0f, 1.0f);
                    regionChanged = false;
                }

                // 영역이 변경되었으면 UV 좌표로 변환하여 적용
                if (regionChanged)
                {
                    // 변경된 픽셀 값을 UV 좌표로 변환
                    m_vTextureRegion.x = max(0.0f, min(1.0f, pixelLeft / textureSize.x));
                    m_vTextureRegion.y = max(0.0f, min(1.0f, pixelTop / textureSize.y));
                    m_vTextureRegion.z = max(0.0f, min(1.0f, pixelRight / textureSize.x));
                    m_vTextureRegion.w = max(0.0f, min(1.0f, pixelBottom / textureSize.y));
                }

                // 현재 영역의 UV 좌표 표시
                ImGui::Text("UV Coordinates: (%.3f, %.3f) - (%.3f, %.3f)",
                    m_vTextureRegion.x, m_vTextureRegion.y,
                    m_vTextureRegion.z, m_vTextureRegion.w);

                // 텍스처 영역 미리보기
                if (!m_SRVs.empty())
                {
                    ImGui::Separator();
                    ImGui::Text("Region Preview:");

                    // 전체 텍스처 미리보기 (영역 표시)
                    const float previewWidth = 300.0f;
                    const float previewHeight = 300.0f;

                    // 텍스처 크기에 따라 비율 유지
                    float aspectRatio = textureSize.x / textureSize.y;
                    ImVec2 imageSize;

                    if (aspectRatio > 1.0f) {
                        // 가로가 더 긴 경우
                        imageSize = ImVec2(previewWidth, previewWidth / aspectRatio);
                    }
                    else {
                        // 세로가 더 긴 경우
                        imageSize = ImVec2(previewHeight * aspectRatio, previewHeight);
                    }

                    // 이미지 위치 계산
                    ImVec2 curPos = ImGui::GetCursorScreenPos();
                    ImVec2 contentRegion = ImGui::GetContentRegionAvail();

                    // 이미지를 중앙에 배치하기 위한 오프셋 계산 (선택 사항)
                    float offsetX = (contentRegion.x - imageSize.x) * 0.5f;
                    if (offsetX > 0) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);

                    // 전체 텍스처 이미지 표시
                    ImGui::Image((ImTextureID)m_SRVs[0], imageSize);

                    // 드래그로 영역 선택 기능
                    if (ImGui::IsItemHovered())
                    {
                        // 마우스 왼쪽 버튼을 누른 상태에서 드래그 시작
                        if (ImGui::IsMouseDown(0) && !ImGui::IsMouseDragging(0)) {
                            // 드래그 시작 위치 저장
                            ImVec2 mousePos = ImGui::GetMousePos();
                            m_fDragStartX = (mousePos.x - curPos.x - offsetX) / imageSize.x;
                            m_fDragStartY = (mousePos.y - curPos.y) / imageSize.y;

                            // 유효 범위 내로 제한
                            m_fDragStartX = max(0.0f, min(1.0f, m_fDragStartX));
                            m_fDragStartY = max(0.0f, min(1.0f, m_fDragStartY));
                        }

                        // 드래그 중
                        if (ImGui::IsMouseDragging(0)) {
                            ImVec2 mousePos = ImGui::GetMousePos();
                            float currentX = (mousePos.x - curPos.x - offsetX) / imageSize.x;
                            float currentY = (mousePos.y - curPos.y) / imageSize.y;

                            // 유효 범위 내로 제한
                            currentX = max(0.0f, min(1.0f, currentX));
                            currentY = max(0.0f, min(1.0f, currentY));

                            // 드래그 영역을 UV 좌표로 설정
                            m_vTextureRegion.x = min(m_fDragStartX, currentX);
                            m_vTextureRegion.y = min(m_fDragStartY, currentY);
                            m_vTextureRegion.z = max(m_fDragStartX, currentX);
                            m_vTextureRegion.w = max(m_fDragStartY, currentY);

                            // 영역 사용 활성화
                            m_bUseRegion = true;
                        }

                        // 마우스 휠 클릭으로 전체 영역 선택
                        if (ImGui::IsMouseClicked(2)) { // 휠 클릭
                            m_vTextureRegion = _float4(0.0f, 0.0f, 1.0f, 1.0f);
                        }

                        // 도움말 툴팁 표시
                        ImGui::BeginTooltip();
                        ImGui::Text("Left-drag: Select region");
                        ImGui::Text("Middle-click: Reset to full texture");
                        ImGui::EndTooltip();
                    }

                    // 선택 영역 오버레이 그리기
                    ImDrawList* drawList = ImGui::GetWindowDrawList();
                    ImVec2 rectMin(
                        curPos.x + offsetX + m_vTextureRegion.x * imageSize.x,
                        curPos.y + m_vTextureRegion.y * imageSize.y
                    );
                    ImVec2 rectMax(
                        curPos.x + offsetX + m_vTextureRegion.z * imageSize.x,
                        curPos.y + m_vTextureRegion.w * imageSize.y
                    );

                    // 선택 영역 강조 표시 (빨간색 테두리)
                    drawList->AddRect(rectMin, rectMax, IM_COL32(255, 0, 0, 255), 0.0f, 0, 2.0f);

                    // 선택된 영역만 확대해서 보여주기 (선택 사항)
                    if (m_bUseRegion)
                    {
                        ImGui::Separator();
                        ImGui::Text("Selected Region:");

                        // 선택된 영역만 표시 (UV 좌표 사용)
                        ImGui::Image(
                            (ImTextureID)m_SRVs[0],
                            ImVec2(200, 200),
                            ImVec2(m_vTextureRegion.x, m_vTextureRegion.y),
                            ImVec2(m_vTextureRegion.z, m_vTextureRegion.w)
                        );
                    }
                }
            }
        }

        // 프로토타입 텍스처 선택 기능
        if (m_pGameInstance)
        {
            // 레벨 선택 콤보박스
            static int selectedLevel = 0;
            const char* levelItems[] = { "STATIC", "LOADING", "LOGO", "UI", "MAP", "EFFECT", "ANIMATION", "GAMEPLAY", "TEST" };
            ImGui::Combo("Level", &selectedLevel, levelItems, IM_ARRAYSIZE(levelItems));

            // 선택된 프로토타입 텍스처 태그 저장 변수
            static wstring selectedProtoTag;

            // 프로토타입 목록 렌더링 및 선택 처리 (Inspector 클래스 방식 참고)
            ImGui::Text("Select Texture Prototype:");
            if (ImGui::BeginListBox("##TexturePrototypes"))
            {
                // 오타 수정: Render_ProtxotypeManagerImGui -> Render_PrototypeManagerImGui
                m_pGameInstance->Render_PrototypeManagerImGui(ENUM_CLASS(selectedLevel), L"Texture", selectedProtoTag);
                ImGui::EndListBox();
            }

            // 선택한 프로토타입이 있을 경우에만 적용 버튼 활성화
            // RenderImGui 함수 내의 프로토타입 적용 부분을 수정
            if (ImGui::Button("Apply Selected Prototype"))
            {
                // 선택한 프로토타입으로 텍스처 변경
                CComponent* pProtoTexture = m_pGameInstance->Find_Prototype(PROTOTYPE::TYPE_COMPONENT, static_cast<_uint>(selectedLevel), selectedProtoTag);
                if (pProtoTexture)
                {
                    CTexture* pTexture = dynamic_cast<CTexture*>(pProtoTexture);
                    if (pTexture)
                    {
                        // 기존 리소스 해제
                        for (auto& pSRV : m_SRVs)
                            Safe_Release(pSRV);
                        m_SRVs.clear();

                        // 새 프로토타입의 정보 복사
                        m_strTextureFilePath = pTexture->m_strTextureFilePath;
                        m_iNumTextures = pTexture->m_iNumTextures;

                        // 프로토타입 정보 저장 추가
                        m_strPrototypeTag = selectedProtoTag;

                        // SRV 복사 (AddRef 필요)
                        for (auto& pSRV : pTexture->m_SRVs)
                        {
                            Safe_AddRef(pSRV);
                            m_SRVs.push_back(pSRV);
                        }

                        ImGui::OpenPopup("Success");
                    }
                    else
                    {
                        ImGui::OpenPopup("Error");
                    }
                }
                else
                {
                    ImGui::OpenPopup("Error");
                }
            }

            // 성공/실패 팝업
            if (ImGui::BeginPopupModal("Success", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Texture changed successfully!");
                if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }

            if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Failed to change texture. Selected prototype not found or not a texture.");
                if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
        }
        else
        {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Select a texture prototype from the list above.");
        }

        // 기존 텍스처 미리보기 표시
        ImGui::Separator();
        ImGui::Text("Current Textures:");
        for (_uint i = 0; i < m_iNumTextures; ++i)
        {
            ImGui::Indent();
            if (ImGui::CollapsingHeader(to_string(i).c_str()))
            {
                ImGui::Image((ImTextureID)(m_SRVs[i]), ImVec2(100, 100), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(0.5f, 0.5f, 0.5f, 1));
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text("SRV: 0x%p", (void*)m_SRVs[i]);
                    ImGui::Image((ImTextureID)((void*)m_SRVs[i]), ImVec2(256, 256));
                    ImGui::EndTooltip();
                }
            }
            ImGui::Unindent();
        }
    }
    ImGui::PopID();
    return S_OK;
    }