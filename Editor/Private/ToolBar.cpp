#pragma once
#include "ToolBar.h"
#include "GameInstance.h"
#include "Level_Loading.h"

#pragma region For Converting FBX to Model
#include "Material.h"
#include "Mesh.h"
#include "Bone.h"
#include "Animation.h"
#include "Model.h"
#pragma endregion

namespace fs = std::filesystem;

CToolBar::CToolBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMyImGui { pDevice, pContext }
{
}

HRESULT CToolBar::Initialize(void* pArg)
{
    m_bShow = true;
    IntializeFileDialogCallBack();
    return S_OK;
}

void CToolBar::Update(_float fTimeDelta)
{

}

HRESULT CToolBar::Render()
{
#pragma region 팝업그리기
    if (ifd::FileDialog::Instance().IsDone("TextureOpenDialog")) {
        if (ifd::FileDialog::Instance().HasResult()) {
            std::string res = ifd::FileDialog::Instance().GetResult().u8string();
            fs::path filePath = res;
            filePath = filePath.relative_path();
            printf("OPEN[%s]\n", res.c_str());
        }
        ifd::FileDialog::Instance().Close();
    }

    if (ifd::FileDialog::Instance().IsDone("ModelOpenDialog")) {
        static bool bAnim = false;

        if (ifd::FileDialog::Instance().HasResult()) {
            std::string res = ifd::FileDialog::Instance().GetResult().u8string();
            Convert_Model(ifd::FileDialog::Instance().GetResult().wstring().c_str());
            printf("OPEN[%s]\n", res.c_str());
        }
        ifd::FileDialog::Instance().Close();
    }

    if (ifd::FileDialog::Instance().IsDone("PrototypeSaveDialog")) {
        static bool bAnim = false;

        if (ifd::FileDialog::Instance().HasResult()) {
            std::string res = ifd::FileDialog::Instance().GetResult().u8string();
            Save_Prototypes(ENUM_CLASS(m_eLevelIndex), ifd::FileDialog::Instance().GetResult().wstring().c_str());
            printf("SAVE[%s]\n", res.c_str());
        }
        ifd::FileDialog::Instance().Close();
    }

    if (ifd::FileDialog::Instance().IsDone("PrototypeLoadDialog")) {
        static bool bAnim = false;

        if (ifd::FileDialog::Instance().HasResult()) {
            std::string res = ifd::FileDialog::Instance().GetResult().u8string();
            Load_Prototypes(ENUM_CLASS(m_eLevelIndex), ifd::FileDialog::Instance().GetResult().wstring().c_str());
            printf("OPEN[%s]\n", res.c_str());
        }
        ifd::FileDialog::Instance().Close();
    }

    if (ifd::FileDialog::Instance().IsDone("GameObjectSaveDialog")) {
        static bool bAnim = false;

        if (ifd::FileDialog::Instance().HasResult()) {
            std::string res = ifd::FileDialog::Instance().GetResult().u8string();
            Save_GameObjects(ENUM_CLASS(m_eLevelIndex), ifd::FileDialog::Instance().GetResult().wstring().c_str());
            printf("SAVE[%s]\n", res.c_str());
        }
        ifd::FileDialog::Instance().Close();
    }

    if (ifd::FileDialog::Instance().IsDone("GameObjectLoadDialog")) {
        static bool bAnim = false;

        if (ifd::FileDialog::Instance().HasResult()) {
            std::string res = ifd::FileDialog::Instance().GetResult().u8string();
            Load_GameObjects(ENUM_CLASS(m_eLevelIndex), ifd::FileDialog::Instance().GetResult().wstring().c_str());
            printf("OPEN[%s]\n", res.c_str());
        }
        ifd::FileDialog::Instance().Close();
    }
#pragma endregion
    if (m_bShow)
    {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                // 파일 메뉴 아이템들
                if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
                    ifd::FileDialog::Instance().Open("TextureOpenDialog", "Open a texture", "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga;*.dds){.png,.jpg,.jpeg,.bmp,.tga,.dds},.*", true, SAVEPATH);
                    
                    // 새 씬 로직
                    printf("Clicked File->New Scene\n");
                }
                if (ImGui::MenuItem("Open Scene...", "Ctrl+O")) {
                    // 씬 열기 로직 (예: 파일 다이얼로그 띄우기)
                    printf("Clicked File->Open Scene\n");
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Save", "Ctrl+S")) {
                    // 저장 로직
                    printf("Clicked File->Save\n");
                }
                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
                    // 다른 이름으로 저장 로직
                    printf("Clicked File->Save As\n");
                }
                ImGui::Separator();
                // 레벨 선택 콤보 박스 추가
                const char* level_names[] = { "Static", "UI", "Map", "Effect", "Animation", "GamePlay", "All"}; // LEVEL enum에 맞게 이름 정의
                LEVEL level_values[] = { LEVEL::STATIC, LEVEL::UI, LEVEL::MAP, LEVEL::EFFECT, LEVEL::ANIMATION, LEVEL::GAMEPLAY, LEVEL::END }; // 실제 LEVEL enum 값
                static int current_level_idx = 0; // 기본 선택 인덱스 (STATIC)

                // m_eLevelToSavePrototypes와 일치하는 인덱스 찾기
                for (int i = 0; i < IM_ARRAYSIZE(level_values); ++i) {
                    if (level_values[i] == m_eLevelIndex) {
                        current_level_idx = i;
                        break;
                    }
                }

                ImGui::PushItemWidth(120); // 콤보 박스 너비 조절
                if (ImGui::Combo("Level to Save", &current_level_idx, level_names, IM_ARRAYSIZE(level_names))) {
                    m_eLevelIndex = level_values[current_level_idx];
                }
                ImGui::PopItemWidth();
                if(ImGui::MenuItem("Save Prototypes", "Ctrl+Shift+S")) {
					ifd::FileDialog::Instance().Save("PrototypeSaveDialog", "Save Prototypes", "json file (*.json){.json},.*", SAVEPATH);
				}
                if (ImGui::MenuItem("Load Prototypes", "Ctrl+Shift+L")) {
                    ifd::FileDialog::Instance().Open("PrototypeLoadDialog", "Load Prototypes", "json file (*.json){.json},.*", true, SAVEPATH);
                }
                ImGui::Separator();

                if (ImGui::MenuItem("Save GameObjects", "Ctrl+Shift+S")) {
                    ifd::FileDialog::Instance().Save("GameObjectSaveDialog", "Save Prototypes", "json file (*.json){.json},.*", SAVEPATH);
                }

                ImGui::Text("Clear");
                ImGui::SameLine();
                ImGui::Toggle("Merge", &m_bMergeLoad);
                if (ImGui::MenuItem("Load GameObjects", "Ctrl+Shift+L")) {
                    ifd::FileDialog::Instance().Open("GameObjectLoadDialog", "Load Prototypes", "json file (*.json){.json},.*", true, SAVEPATH);
                }

                ImGui::Separator();
                ImGui::Text("NonAnimation");
                ImGui::SameLine();
                ImGui::Toggle("Animation", &m_bConvertAnim);
                if (ImGui::MenuItem("Convert Model", "Ctrl+Alt+C")) {
                    ifd::FileDialog::Instance().Open("ModelOpenDialog", "Open a Model", "model file (*.fbx){.fbx},.*", true, SAVEPATH);
                    printf("Clicked File->Convert Model\n");
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit")) {
                    // 종료 로직 (예: 애플리케이션 종료 플래그 설정)
                    printf("Clicked File->Exit\n");
                    // exit_requested = true; // 예시
                }
                ImGui::EndMenu(); // File 메뉴 끝
            }

            if (ImGui::BeginMenu("Edit")) {
                // 편집 메뉴 아이템들
                if (ImGui::MenuItem("Undo", "Ctrl+Z", false, false)) { // 세 번째: 선택 여부, 네 번째: 비활성화 여부 (지금은 비활성화)
                    // 실행 취소 로직 (아직 구현 안 됨)
                    printf("Clicked Edit->Undo (Not implemented)\n");
                }
                if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) { // 비활성화
                    // 다시 실행 로직 (아직 구현 안 됨)
                    printf("Clicked Edit->Redo (Not implemented)\n");
                }
                // ... 다른 편집 메뉴 ...
                ImGui::EndMenu(); // Edit 메뉴 끝
            }

            if (ImGui::BeginMenu("Window")) {
                // 창 메뉴 아이템들 (각 창의 표시 여부를 토글)
                // ImGui::MenuItem의 세 번째 인자로 bool 포인터를 전달하면 체크박스처럼 동작합니다.
                if (ImGui::MenuItem("Asset Browser", NULL /* 단축키 없음 */ /*, &show_asset_browser */)) {
                    m_pGameInstance->ToggleShowImGui(L"Asset Browser");
                }
                if (ImGui::MenuItem("Inspector", NULL /*, &show_inspector */)) {
                    m_pGameInstance->ToggleShowImGui(L"Inspector");
                }
                if (ImGui::MenuItem("Hierarchy", NULL /*, &show_inspector */)) {
                    m_pGameInstance->ToggleShowImGui(L"Hierarchy");
                }
                if (ImGui::MenuItem("Viewport2D", NULL /*, &show_inspector */)) {
                    m_pGameInstance->ToggleShowImGui(L"Viewport2D");
                }
                if (ImGui::MenuItem("Viewport3D", NULL /*, &show_inspector */)) {
                    m_pGameInstance->ToggleShowImGui(L"Viewport3D");
                }
                ImGui::Separator();
                ImGui::EndMenu(); // Window 메뉴 끝
            }

            if (ImGui::BeginMenu("Levels")) {
                // 도움말 메뉴 아이템들
                if (ImGui::MenuItem("UI Tool")) {
                    if (FAILED(m_pGameInstance->Change_Level(ENUM_CLASS(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::UI))))
                        return E_FAIL;
                    //m_pGameInstance->Change_Level()
                    printf("Clicked Help->About\n");
                    // open_about_popup = true; // 예시
                }
                if (ImGui::MenuItem("Map Tool")) {
                    if (FAILED(m_pGameInstance->Change_Level(ENUM_CLASS(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::MAP))))
                        return E_FAIL;
                    printf("Clicked Help->About\n");
                    // open_about_popup = true; // 예시
                }
                if (ImGui::MenuItem("Effect Tool")) {
                    if (FAILED(m_pGameInstance->Change_Level(ENUM_CLASS(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::EFFECT))))
                        return E_FAIL;
                    printf("Clicked Help->About\n");
                    // open_about_popup = true; // 예시
                }

                if (ImGui::MenuItem("Animation Tool")) {
                    if (FAILED(m_pGameInstance->Change_Level(ENUM_CLASS(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::ANIMATION))))
                        return E_FAIL;
                    printf("Clicked Help->About\n");
                    // open_about_popup = true; // 예시
                }

                if (ImGui::MenuItem("Gameplay")) {
                    printf("Clicked Help->About\n");
                    // open_about_popup = true; // 예시
                }
                ImGui::EndMenu(); // Help 메뉴 끝
            }

            if (ImGui::BeginMenu("Help")) {
                // 도움말 메뉴 아이템들
                if (ImGui::MenuItem("About My Editor...")) {
                    // 정보 대화상자 표시 로직
                    printf("Clicked Help->About\n");
                    // open_about_popup = true; // 예시
                }
                ImGui::EndMenu(); // Help 메뉴 끝
            }

            ImGui::EndMainMenuBar(); // 메인 메뉴바 끝
        }
    }

    return S_OK;
}

void CToolBar::IntializeFileDialogCallBack()
{
    ifd::FileDialog::Instance().CreateTexture =
        [this](uint8_t* data, int w, int h, char fmt) -> void* {

        if (!m_pDevice) return nullptr;

        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Width = w;
        desc.Height = h;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = (fmt == 0) ? DXGI_FORMAT_B8G8R8A8_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;

        ID3D11Texture2D* pTexture = nullptr;
        D3D11_SUBRESOURCE_DATA subResource;
        subResource.pSysMem = data;
        subResource.SysMemPitch = desc.Width * 4;
        subResource.SysMemSlicePitch = 0;
        if (FAILED(m_pDevice->CreateTexture2D(&desc, &subResource, &pTexture))) {
            return nullptr;
        }

        ID3D11ShaderResourceView* pSRV = nullptr;
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;
        if (FAILED(m_pDevice->CreateShaderResourceView(pTexture, &srvDesc, &pSRV))) {
            return nullptr;
        }
        pTexture->Release();

        return (void*)pSRV;
        };

    ifd::FileDialog::Instance().DeleteTexture =
        [](void* tex) {
        if (tex) {
            ID3D11ShaderResourceView* pSRV = reinterpret_cast<ID3D11ShaderResourceView*>(tex);
            pSRV->Release();
        }
        };
}

#pragma region Convert fbx Model to Binary

HRESULT CToolBar::Convert_Model(const _tchar* pFilePath)
{
    //순서
    //본노드
    //메쉬
    ////인덱스
    ////버텍스
    ////본정보
    //마테리얼
    ////텍스쳐
    //애니메이션
    ////채널
    ////키프레임
    //////트랜스폼
    fs::path pTargetPath = pFilePath;
    fs::path pBasePath = fs::current_path().parent_path().parent_path();
    pTargetPath = fs::relative(pTargetPath, pBasePath);
    fs::path pFinalPath = "../../";
    pFinalPath.append(pTargetPath.replace_extension(".model").generic_wstring());

    _uint		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
    if (!m_bConvertAnim)
        iFlag |= aiProcess_PreTransformVertices;
    Assimp::Importer asImporter;
    const aiScene* pAiScene = asImporter.ReadFile(WideToUtf8(pFilePath).c_str(), iFlag);

    ofstream outputFile;
    outputFile.open(pFinalPath, std::ios::binary | std::ios::trunc);
    if (outputFile.is_open())
    {
        _uint iNumMeshes = pAiScene->mNumMeshes;
        _uint iNumMaterials = pAiScene->mNumMaterials;
        _uint iNumAnimations = pAiScene->mNumAnimations;
        outputFile.write(reinterpret_cast<const char*>(&iNumMeshes), sizeof(_uint));
        outputFile.write(reinterpret_cast<const char*>(&iNumMaterials), sizeof(_uint));
        outputFile.write(reinterpret_cast<const char*>(&iNumAnimations), sizeof(_uint));
#pragma region WriteBones
        Write_BoneNode(outputFile, pAiScene->mRootNode, -1);
#pragma endregion

#pragma region WriteMeshes
        for (_uint i = 0; i < iNumMeshes; ++i)
        {
            aiMesh* pAiMesh = pAiScene->mMeshes[i];
            _uint iNumVertices = pAiMesh->mNumVertices;
            _uint iNumBones = pAiMesh->mNumBones;
            outputFile.write(reinterpret_cast<const char*>(&pAiMesh->mName.length), sizeof(_uint));
            outputFile.write(reinterpret_cast<const char*>(pAiMesh->mName.C_Str()), sizeof(_char) * pAiMesh->mName.length);
            outputFile.write(reinterpret_cast<const char*>(&pAiMesh->mMaterialIndex), sizeof(_uint));
            outputFile.write(reinterpret_cast<const char*>(&iNumVertices), sizeof(_uint));
            outputFile.write(reinterpret_cast<const char*>(&iNumBones), sizeof(_uint));
            _uint iNumIndcies = pAiMesh->mNumFaces * pAiMesh->mFaces->mNumIndices;
            outputFile.write(reinterpret_cast<const char*>(&iNumIndcies), sizeof(_uint));

            //outputFile.write(reinterpret_cast<const char*>(&pAiMesh->mFaces->mIndices[0]), sizeof(_uint) * iNumIndcies);

            for (_uint j = 0; j < pAiMesh->mNumVertices; ++j)
            {
                outputFile.write(reinterpret_cast<const char*>(&pAiMesh->mVertices[j]), sizeof(_float3));
                outputFile.write(reinterpret_cast<const char*>(&pAiMesh->mNormals[j]), sizeof(_float3));
                outputFile.write(reinterpret_cast<const char*>(&pAiMesh->mTangents[j]), sizeof(_float3));
                outputFile.write(reinterpret_cast<const char*>(&pAiMesh->mTextureCoords[0][j]), sizeof(_float2));
            }

            if (m_bConvertAnim)
            {
                for (_uint j = 0; j < iNumBones; ++j)
                {
                    aiBone* pAiBone = pAiMesh->mBones[j];
                    outputFile.write(reinterpret_cast<const char*>(&pAiBone->mName.length), sizeof(_uint));
                    outputFile.write(reinterpret_cast<const char*>(pAiBone->mName.C_Str()), sizeof(_char) * pAiBone->mName.length);
                    outputFile.write(reinterpret_cast<const char*>(&pAiBone->mOffsetMatrix), sizeof(_float4x4));
                    outputFile.write(reinterpret_cast<const char*>(&pAiBone->mNumWeights), sizeof(_uint));
                    for (_uint k = 0; k < pAiBone->mNumWeights; ++k)
                    {
                        outputFile.write(reinterpret_cast<const char*>(&pAiBone->mWeights[k].mVertexId), sizeof(_uint));
                        outputFile.write(reinterpret_cast<const char*>(&pAiBone->mWeights[k].mWeight), sizeof(_float));
                    }
                }
            }

#pragma region Indices

            _uint iIndex = { 0 };
            for (_uint j = 0; j < pAiMesh->mNumFaces; ++j)
            {
                outputFile.write(reinterpret_cast<const char*>(&pAiMesh->mFaces[j].mIndices[0]), sizeof(_uint));
                outputFile.write(reinterpret_cast<const char*>(&pAiMesh->mFaces[j].mIndices[1]), sizeof(_uint));
                outputFile.write(reinterpret_cast<const char*>(&pAiMesh->mFaces[j].mIndices[2]), sizeof(_uint));
            }
#pragma endregion
        }


#pragma endregion

#pragma region WriteMatrials
        for (_uint i = 0; i < iNumMaterials; ++i)
        {
            aiMaterial* pAiMaterial = pAiScene->mMaterials[i];
            _uint iNumTextures = 0;
            aiString strMaterialName = pAiMaterial->GetName();
            outputFile.write(reinterpret_cast<const char*>(&strMaterialName.length), sizeof(_uint));
            outputFile.write(reinterpret_cast<const char*>(strMaterialName.C_Str()), sizeof(_char) * strMaterialName.length);
            for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; ++j)
            {
                _uint iNumTextureCount = pAiMaterial->GetTextureCount(static_cast<aiTextureType>(j));
                outputFile.write(reinterpret_cast<const char*>(&iNumTextureCount), sizeof(_uint));
                for (_uint k = 0; k < iNumTextureCount; ++k)
                {
                    aiString strTexturePath = {};
                    pAiMaterial->GetTexture(static_cast<aiTextureType>(j), k, &strTexturePath);
                    outputFile.write(reinterpret_cast<const char*>(&strTexturePath.length), sizeof(_uint));
                    outputFile.write(reinterpret_cast<const char*>(strTexturePath.C_Str()), sizeof(_char) * strTexturePath.length);
                }
            }
        }
#pragma endregion

#pragma region WriteAnimations
        if (m_bConvertAnim)
        {
            for (_uint i = 0; i < iNumAnimations; ++i)
            {
                aiAnimation* pAiAnimation = pAiScene->mAnimations[i];
                _uint iNumChannels = pAiAnimation->mNumChannels;
                outputFile.write(reinterpret_cast<const char*>(&pAiAnimation->mNumChannels), sizeof(_uint));
                outputFile.write(reinterpret_cast<const char*>(&pAiAnimation->mName.length), sizeof(_uint));
                outputFile.write(reinterpret_cast<const char*>(pAiAnimation->mName.C_Str()), sizeof(_char) * pAiAnimation->mName.length);
                outputFile.write(reinterpret_cast<const char*>(&pAiAnimation->mDuration), sizeof(_double));
                outputFile.write(reinterpret_cast<const char*>(&pAiAnimation->mTicksPerSecond), sizeof(_double));
                for (_uint j = 0; j < iNumChannels; j++)
                {
                    aiNodeAnim* pAiNodeAnim = pAiAnimation->mChannels[j];
                    outputFile.write(reinterpret_cast<const char*>(&pAiNodeAnim->mNodeName.length), sizeof(_uint));
                    outputFile.write(reinterpret_cast<const char*>(pAiNodeAnim->mNodeName.C_Str()), sizeof(_char) * pAiNodeAnim->mNodeName.length);
                    outputFile.write(reinterpret_cast<const char*>(&pAiNodeAnim->mNumPositionKeys), sizeof(_uint));
                    outputFile.write(reinterpret_cast<const char*>(&pAiNodeAnim->mNumRotationKeys), sizeof(_uint));
                    outputFile.write(reinterpret_cast<const char*>(&pAiNodeAnim->mNumScalingKeys), sizeof(_uint));
                    _uint iNumKeyFrames = max(pAiNodeAnim->mNumPositionKeys, pAiNodeAnim->mNumRotationKeys);
                    iNumKeyFrames = max(pAiNodeAnim->mNumScalingKeys, iNumKeyFrames);
                    outputFile.write(reinterpret_cast<const char*>(&iNumKeyFrames), sizeof(_uint));
                    for (_uint k = 0; k < iNumKeyFrames; ++k)
                    {
                        if (k < pAiNodeAnim->mNumPositionKeys)
                        {
                            outputFile.write(reinterpret_cast<const char*>(&pAiNodeAnim->mPositionKeys[k].mTime), sizeof(_double));
                            outputFile.write(reinterpret_cast<const char*>(&pAiNodeAnim->mPositionKeys[k].mValue), sizeof(_float3));
                        }
                        if (k < pAiNodeAnim->mNumRotationKeys)
                        {
                            outputFile.write(reinterpret_cast<const char*>(&pAiNodeAnim->mRotationKeys[k].mTime), sizeof(_double));
                            outputFile.write(reinterpret_cast<const char*>(&pAiNodeAnim->mRotationKeys[k].mValue.x), sizeof(_float));
                            outputFile.write(reinterpret_cast<const char*>(&pAiNodeAnim->mRotationKeys[k].mValue.y), sizeof(_float));
                            outputFile.write(reinterpret_cast<const char*>(&pAiNodeAnim->mRotationKeys[k].mValue.z), sizeof(_float));
                            outputFile.write(reinterpret_cast<const char*>(&pAiNodeAnim->mRotationKeys[k].mValue.w), sizeof(_float));
                        }
                        if (k < pAiNodeAnim->mNumScalingKeys)
                        {
                            outputFile.write(reinterpret_cast<const char*>(&pAiNodeAnim->mScalingKeys[k].mTime), sizeof(_double));
                            outputFile.write(reinterpret_cast<const char*>(&pAiNodeAnim->mScalingKeys[k].mValue), sizeof(_float3));
                        }
                    }
                }
            }
        }
#pragma endregion
        outputFile.close();
    }



    return S_OK;
}

HRESULT CToolBar::Write_BoneNode(ofstream& ofs, aiNode* pAiNode, _int iParentBoneIndex)
{
    ofs.write(reinterpret_cast<const char*>(&pAiNode->mName.length), sizeof(_uint));
    ofs.write(reinterpret_cast<const char*>(pAiNode->mName.C_Str()), sizeof(_char) * pAiNode->mName.length);
    ofs.write(reinterpret_cast<const char*>(&pAiNode->mTransformation), sizeof(_float4x4));
    ofs.write(reinterpret_cast<const char*>(&iParentBoneIndex), sizeof(_int));
    ofs.write(reinterpret_cast<const char*>(&pAiNode->mNumChildren), sizeof(_uint));

    int iParentIndex = iParentBoneIndex + 1;
    for (_uint i = 0; i < pAiNode->mNumChildren; ++i)
    {
        Write_BoneNode(ofs, pAiNode->mChildren[i], iParentIndex);
    }
    return S_OK;
}
#pragma endregion

#pragma region Save&Load Prototypes
HRESULT CToolBar::Save_Prototypes(_uint iLevelIndex, const _tchar* pFilePath)
{
    if (m_pGameInstance == nullptr)
        return E_FAIL;
    json data = {};
    if (iLevelIndex == ENUM_CLASS(LEVEL::END))
        m_pGameInstance->Save_Prototypes(data);
    else
        m_pGameInstance->Save_Prototypes(iLevelIndex, data);
    fstream outputFile;
    outputFile.open(pFilePath, std::ios::out | std::ios::trunc);
    if (!outputFile.is_open()) {
        //printf("Failed to open file for writing: %s\n", pFilePath);
        return E_FAIL;
    }
    else
    {
        outputFile << data.dump(4); // 4는 들여쓰기 공백 수
        outputFile.close();
        //printf("Prototypes saved to %s\n", pFilePath.);
    }
    return S_OK;
}

HRESULT CToolBar::Load_Prototypes(_uint iLevelIndex, const _tchar* pFilePath)
{
    if (m_pGameInstance == nullptr)
        return E_FAIL;
    m_pGameInstance->Load_Prototypes(iLevelIndex, pFilePath);
    return S_OK;
}
#pragma endregion

#pragma region Save&Load GameObjects

HRESULT CToolBar::Save_GameObjects(_uint iLevelIndex, const _tchar* pFilePath)
{
    if (m_pGameInstance == nullptr)
        return E_FAIL;
    json data = {};
    if (iLevelIndex == ENUM_CLASS(LEVEL::END))
        m_pGameInstance->Save_GameObjects(data);
    else
        m_pGameInstance->Save_GameObjects(iLevelIndex, data);
    fstream outputFile;
    outputFile.open(pFilePath, std::ios::out | std::ios::trunc);
    if (!outputFile.is_open()) {
        //printf("Failed to open file for writing: %s\n", pFilePath);
        return E_FAIL;
    }
    else
    {
        outputFile << data.dump(4); // 4는 들여쓰기 공백 수
        outputFile.close();
        //printf("Prototypes saved to %s\n", pFilePath.);   
    }
    return S_OK;
}

HRESULT CToolBar::Load_GameObjects(_uint iLevelIndex, const _tchar* pFilePath)
{
    if (m_pGameInstance == nullptr)
        return E_FAIL;

    if (!m_bMergeLoad)
        m_pGameInstance->Clear_GameObjects(iLevelIndex);

    m_pGameInstance->Load_GameObjects(iLevelIndex, pFilePath);
    return S_OK;
}

#pragma endregion








CToolBar* CToolBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CToolBar* pInstance = new CToolBar(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CToolBar");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CToolBar::Free()
{
    __super::Free();
}
