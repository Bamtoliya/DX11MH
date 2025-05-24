#include "Material.h"
#include "Shader.h"

const char* AiTextureTypeToString(aiTextureType eType) {
    switch (eType) {
    case aiTextureType_NONE: return "NONE";
    case aiTextureType_DIFFUSE: return "Diffuse";
    case aiTextureType_SPECULAR: return "Specular";
    case aiTextureType_AMBIENT: return "Ambient";
    case aiTextureType_EMISSIVE: return "Emissive";
    case aiTextureType_HEIGHT: return "Height";
    case aiTextureType_NORMALS: return "Normals";
    case aiTextureType_SHININESS: return "Shininess";
    case aiTextureType_OPACITY: return "Opacity";
    case aiTextureType_DISPLACEMENT: return "Displacement";
    case aiTextureType_LIGHTMAP: return "Lightmap";
    case aiTextureType_REFLECTION: return "Reflection";
    case aiTextureType_BASE_COLOR: return "Base Color";
    case aiTextureType_NORMAL_CAMERA: return "Normal Camera";
    case aiTextureType_EMISSION_COLOR: return "Emission Color";
    case aiTextureType_METALNESS: return "Metalness";
    case aiTextureType_DIFFUSE_ROUGHNESS: return "Diffuse Roughness";
    case aiTextureType_AMBIENT_OCCLUSION: return "Ambient Occlusion";
    case aiTextureType_SHEEN: return "Sheen"; // Assimp 5.1+
    case aiTextureType_CLEARCOAT: return "Clearcoat"; // Assimp 5.1+
    case aiTextureType_TRANSMISSION: return "Transmission"; // Assimp 5.1+
    case aiTextureType_UNKNOWN: return "Unknown";
    default: return "Unhandled Type";
    }
}

#pragma region Constructor/Destructor
CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, ifstream& ifs)
{
    CMaterial* pInstance = new CMaterial(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pModelFilePath, ifs)))
    {
        MSG_BOX("Failed to Created in Binary: CMaterial");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
    CMaterial* pInstance = new CMaterial(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pModelFilePath, pAIMaterial)))
    {
        MSG_BOX("Failed to Created : CMaterial");
        Safe_Release(pInstance);
    }

    return pInstance;
}

HRESULT CMaterial::Initialize(const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
    strcpy_s(m_szName, pAIMaterial->GetName().C_Str());
    for (size_t i = 1; i < AI_TEXTURE_TYPE_MAX; i++)
    {
        _uint       iNumSRVs = pAIMaterial->GetTextureCount(static_cast<aiTextureType>(i));
        
        m_SRVs[i].reserve(iNumSRVs);

        for (size_t j = 0; j < iNumSRVs; j++)
        {
            aiString     strTexturePath;

            if (FAILED(pAIMaterial->GetTexture(static_cast<aiTextureType>(i), (_uint)j, &strTexturePath)))
                return E_FAIL;

            _char       szFullPath[MAX_PATH] = {};
            _char       szDrive[MAX_PATH] = {};
            _char       szDir[MAX_PATH] = {};
            _char       szFileName[MAX_PATH] = {};
            _char       szExt[MAX_PATH] = {};

            _splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
            _splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

            strcpy_s(szFullPath, szDrive);
            strcat_s(szFullPath, szDir);
            strcat_s(szFullPath, szFileName);
            strcat_s(szFullPath, szExt);

            _tchar      szTextureFilePath[MAX_PATH] = {};
            m_TexturePaths[i].push_back(szFullPath);
            MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szTextureFilePath, MAX_PATH);

            HRESULT         hr = { };
            ID3D11ShaderResourceView* pSRV = { nullptr };

            if (false == strcmp(szExt, ".dds"))
                hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
            else
                hr = DirectX::CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

            if (FAILED(hr))
                return E_FAIL;

            m_SRVs[i].push_back(pSRV);
        }
    }

    return S_OK;
}

HRESULT CMaterial::Initialize(const _char* pModelFilePath, ifstream& ifs)
{
    _uint iNameLength;
    ifs.read(reinterpret_cast<char*>(&iNameLength), sizeof(_uint));
    ifs.read(reinterpret_cast<char*>(&m_szName[0]), sizeof(_char) * iNameLength);
    m_szName[iNameLength] = '\0';
    for (size_t i = 1; i < AI_TEXTURE_TYPE_MAX; i++)
    {
        _uint       iNumSRVs = {};
        ifs.read(reinterpret_cast<char*>(&iNumSRVs), sizeof(_uint));

        m_SRVs[i].reserve(iNumSRVs);

        for (size_t j = 0; j < iNumSRVs; j++)
        {
            _uint iTexturePathLength = {};

            ifs.read(reinterpret_cast<char*>(&iTexturePathLength), sizeof(_uint));
            _char* szTexturePath = new _char[iTexturePathLength + 1];
            ifs.read(reinterpret_cast<char*>(&szTexturePath[0]), sizeof(_char) * iTexturePathLength);
            szTexturePath[iTexturePathLength] = '\0';

            _char       szFullPath[MAX_PATH] = {};
            _char       szDrive[MAX_PATH] = {};
            _char       szDir[MAX_PATH] = {};
            _char       szFileName[MAX_PATH] = {};
            _char       szExt[MAX_PATH] = {};

            _splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
            _splitpath_s(szTexturePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

            strcpy_s(szFullPath, szDrive);
            strcat_s(szFullPath, szDir);
            strcat_s(szFullPath, szFileName);
            strcat_s(szFullPath, szExt);

            _tchar      szTextureFilePath[MAX_PATH] = {};
            m_TexturePaths[i].push_back(szFullPath);
            MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szTextureFilePath, MAX_PATH);

            HRESULT         hr = { };
            ID3D11ShaderResourceView* pSRV = { nullptr };

            if (false == strcmp(szExt, ".dds"))
                hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
            else
                hr = DirectX::CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);

            if (FAILED(hr))
                return E_FAIL;

            m_SRVs[i].push_back(pSRV);
            Safe_Delete_Array(szTexturePath);
        }
    }

    return S_OK;
}

void CMaterial::Free()
{
    __super::Free();

    for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
    {
        for (auto& pSRV : m_SRVs[i])
            Safe_Release(pSRV);
        m_SRVs[i].clear();
    }

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
#pragma endregion

HRESULT CMaterial::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iTextureIndex)
{
    if (iTextureIndex >= m_SRVs[eType].size())
        return E_FAIL;

    return pShader->Bind_SRV(pConstantName, m_SRVs[eType][iTextureIndex]);    
}

HRESULT CMaterial::RenderImGui()
{
    // 재질 이름을 표시할 수 있다면 여기에 추가 (예: m_strMaterialName 멤버가 있다면)
    //ImGui::Text("Material Name: %s", m_strMaterialName.c_str());

    // 고유 ID를 위해 재질 객체의 주소를 사용하거나, 다른 고유한 값을 사용합니다.
    ImGui::PushID(this);

    if (ImGui::TreeNode(m_szName))
    {
        for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
        {
            aiTextureType eCurrentType = static_cast<aiTextureType>(i);

            // SRV가 실제로 있는 경우에만 해당 타입 노드를 표시합니다.
            // (m_TexturePaths는 경로만 있고 실제 로드 실패로 SRV가 없을 수 있으므로 m_SRVs 기준으로)
            bool has_valid_srv_for_type = false;
            for (const auto& srv : m_SRVs[eCurrentType]) {
                if (srv != nullptr) {
                    has_valid_srv_for_type = true;
                    break;
                }
            }

            if (has_valid_srv_for_type)
            {
                // 각 타입 노드에 고유 ID 부여
                std::string typeNodeID = "TypeNode##" + std::to_string(i);
                std::string typeLabel = std::string(AiTextureTypeToString(eCurrentType)) + " (" + std::to_string(m_SRVs[eCurrentType].size()) + ")";

                if (ImGui::TreeNodeEx(typeNodeID.c_str(), ImGuiTreeNodeFlags_DefaultOpen, "%s", typeLabel.c_str()))
                {
                    for (size_t j = 0; j < m_SRVs[eCurrentType].size(); ++j)
                    {
                        ID3D11ShaderResourceView* pSRV = m_SRVs[eCurrentType][j];

                        // 각 텍스처 항목에 고유 ID 부여
                        ImGui::PushID(static_cast<int>(j));

                        // 텍스처 경로 표시
                        if (j < m_TexturePaths[eCurrentType].size() && !m_TexturePaths[eCurrentType][j].empty()) {
                            const std::string& path = m_TexturePaths[eCurrentType][j];
                            char pathBuffer[512];
                            strncpy_s(pathBuffer, sizeof(pathBuffer), path.c_str(), _TRUNCATE);
                            ImGui::InputText("Path", pathBuffer, sizeof(pathBuffer), ImGuiInputTextFlags_ReadOnly);
                        }
                        else {
                            ImGui::TextDisabled("Path: N/A or Empty");
                        }

                        // SRV 미리보기 (ImGui::Image 사용)
                        if (pSRV != nullptr) {
                            ImGui::Image((ImTextureID)(pSRV), ImVec2(100, 100), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(0.5f, 0.5f, 0.5f, 1)); // 테두리 색상 추가
                            if (ImGui::IsItemHovered()) {
                                ImGui::BeginTooltip();
                                ImGui::Text("SRV: 0x%p", (void*)pSRV);
                                if (j < m_TexturePaths[eCurrentType].size()) {
                                    ImGui::Text("Full Path: %s", m_TexturePaths[eCurrentType][j].c_str());
                                }
                                ImGui::Image((ImTextureID)(pSRV), ImVec2(256, 256)); // 툴팁에 더 큰 이미지
                                ImGui::EndTooltip();
                            }
                        }
                        else {
                            ImGui::TextDisabled("Texture Preview N/A (SRV is Null)");
                            ImGui::Dummy(ImVec2(100, 100)); // SRV가 없을 때도 공간 차지
                        }
                        ImGui::Separator();
                        ImGui::PopID(); // 각 텍스처 항목 ID Pop
                    }
                    ImGui::TreePop();
                }
            }
        }
        ImGui::TreePop();
    }
    ImGui::PopID(); // 재질 객체 ID Pop
    return S_OK;
}