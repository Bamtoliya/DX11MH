#pragma once
#include "ToolBar.h"
#include "GameInstance.h"
#include "Level_Loading.h"

#pragma region For Converting FBX to Model
#include "Material.h"
#include "Mesh.h";
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
        if (ifd::FileDialog::Instance().HasResult()) {
            std::string res = ifd::FileDialog::Instance().GetResult().u8string();
            Convert_Model(ifd::FileDialog::Instance().GetResult().wstring().c_str());
            printf("OPEN[%s]\n", res.c_str());
        }
        ifd::FileDialog::Instance().Close();
    }
    if (m_bShow)
    {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                // 파일 메뉴 아이템들
                if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
                    ifd::FileDialog::Instance().Open("TextureOpenDialog", "Open a texture", "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga;*.dds){.png,.jpg,.jpeg,.bmp,.tga,.dds},.*", true, ASSETPATH);
                    
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
                if (ImGui::MenuItem("Convert Model", "Ctrl+Alt+C")) {
                    ifd::FileDialog::Instance().Open("ModelOpenDialog", "Open a Model", "model file (*.fbx){.fbx},.*", true, ASSETPATH);
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

HRESULT CToolBar::Save_Scene()
{
    return S_OK;
}

HRESULT CToolBar::Convert_Model(const _tchar* pFilePath)
{
    fs::path pTargetPath = pFilePath;
    fs::path pBasePath = fs::current_path().parent_path().parent_path();
    pTargetPath = fs::relative(pTargetPath, pBasePath);
    fs::path pFinalPath = "../../";
    pFinalPath.append(pTargetPath.replace_extension(".model").generic_wstring());

    _uint		iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
    Assimp::Importer asImporter;
    const aiScene* pAiScene = asImporter.ReadFile(WideToUtf8(pFilePath).c_str(), iFlag);

    ofstream outputFile(pFinalPath, ios::binary);
    if (outputFile.is_open())
    {
		_uint iNumMeshes = pAiScene->mNumMeshes;
		_uint iNumMaterials = pAiScene->mNumMaterials;
        _uint iNumAnimations = pAiScene->mNumAnimations;
        outputFile.write(reinterpret_cast<const char*>(&pAiScene->mNumMeshes), sizeof(_uint));
        outputFile.write(reinterpret_cast<const char*>(&pAiScene->mNumMaterials), sizeof(_uint));
        outputFile.write(reinterpret_cast<const char*>(&pAiScene->mNumAnimations), sizeof(_uint));
        
        for (_uint i = 0; i < iNumMeshes; ++i)
        {
            aiMesh* pAiMesh = pAiScene->mMeshes[i];
            _uint iNumVertices = pAiMesh->mNumVertices;
			_uint iNumBones = pAiMesh->mNumBones;
            outputFile.write(reinterpret_cast<const char*>(&pAiMesh->mMaterialIndex), sizeof(_uint));
            outputFile.write(reinterpret_cast<const char*>(&pAiMesh->mPrimitiveTypes), sizeof(_uint));
            outputFile.write(reinterpret_cast<const char*>(&pAiMesh->mNumBones), sizeof(_uint));
            outputFile.write(reinterpret_cast<const char*>(&iNumVertices), sizeof(_uint));
            _uint iNumIndcies = pAiMesh->mNumFaces * pAiMesh->mFaces->mNumIndices;
            
            outputFile.write(reinterpret_cast<const char*>(&iNumIndcies), sizeof(_uint));
            //outputFile.write(reinterpret_cast<const char*>(pAiMesh->mBones), )
            outputFile.write(reinterpret_cast<const char*>(&pAiMesh->mFaces->mIndices), sizeof(_uint) * iNumIndcies);
            outputFile.write(reinterpret_cast<const char*>(&pAiMesh->mVertices[0]), sizeof(_float3) * iNumVertices);
            outputFile.write(reinterpret_cast<const char*>(&pAiMesh->mNormals[0]), sizeof(_float3) * iNumVertices);
            outputFile.write(reinterpret_cast<const char*>(&pAiMesh->mTangents[0]), sizeof(_float3) * iNumVertices);
            outputFile.write(reinterpret_cast<const char*>(&pAiMesh->mTextureCoords[0]), sizeof(_float2) * iNumVertices);
            //outputFile.write()
            if (iNumBones > 0)
            {
                for (_uint j = 0; j < iNumBones; ++j)
                {
                    pAiMesh->mBones[j]->mName;
                    pAiMesh->mBones[j]->mOffsetMatrix;
                    pAiMesh->mBones[j]->mNode;
                    pAiMesh->mBones[j]->mNumWeights;
                    pAiMesh->mBones[j]->mWeights->mWeight;
					pAiMesh->mBones[j]->mWeights->mVertexId;
                }
                //outputFile.write(reinterpret_cast<const char*>(&pAiMesh->mBones, sizeof();
            }
            if (pAiMesh->mNumAnimMeshes > 0)
            {

            }
            //outputFile.write(reinterpret_cast<const char*>(pAiMesh->mNum), sizeof(_uint));
            //pAiMesh.
            //for(int )
            //pAiMesh->mNumBones;
            int a = 10;
        }

        for (_uint i = 0; i < iNumMaterials; ++i)
        {
            aiMaterial* pAiMaterial = pAiScene->mMaterials[i];
            for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; ++j)
            {
                _uint iNumTextureCount = pAiMaterial->GetTextureCount(static_cast<aiTextureType>(j));
                outputFile.write(reinterpret_cast<const char*>(&iNumTextureCount), sizeof(_uint));
                for (_uint k = 0; k < pAiMaterial->GetTextureCount(static_cast<aiTextureType>(j)); ++k)
                {
                    aiString strTexturePath = {};
                    pAiMaterial->GetTexture(static_cast<aiTextureType>(j), k, &strTexturePath);
                    outputFile.write(reinterpret_cast<const char*>(&strTexturePath.length), sizeof(_uint));
                    outputFile.write(reinterpret_cast<const char*>(strTexturePath.C_Str()), sizeof(_char) * strTexturePath.length);
                    outputFile.write(reinterpret_cast<const char*>(&j), sizeof(_uint));
                }
            }
        }


        if (iNumAnimations > 0)
        {
            for (_uint i = 0; i < iNumAnimations; ++i)
            {
                aiAnimation* pAiAnimation = pAiScene->mAnimations[i];
                //int a = 10;
            }
        }
        outputFile.close();
    }

    //순서
    //메쉬
    ////버텍스
	////인덱스
    ////본
    //마테리얼
    ////텍스쳐
    //애니메이션
    ////채널
    ////키프레임
	//////트랜스폼

    _uint testNum[3] = {};
    ifstream inputFile(pFinalPath, ios::binary);
    CMaterial::MODEL_MAT* pModelMat = {};
    if(inputFile.is_open())
    {
        inputFile.read(reinterpret_cast<char*>(&testNum[0]), sizeof(_uint) * 3);
        pModelMat = new CMaterial::MODEL_MAT[testNum[2]];
        for (_uint i = 0; i < testNum[1]; ++i)
        {
            aiString strTexturePath = {};
            inputFile.read(reinterpret_cast<char*>(&pModelMat[i].iNumTexture), sizeof(_uint));
            pModelMat[i].strTextureName = new string[pModelMat[i].iNumTexture];
            pModelMat[i].iTypeTexture = new _uint[pModelMat[i].iNumTexture];
            for (_uint j = 0; j < pModelMat[i].iNumTexture; ++i)
            {
                _uint strLength = {};
                inputFile.read(reinterpret_cast<char*>(&strLength), sizeof(_uint));
                pModelMat[i].strTextureName[j].resize(strLength);
                inputFile.read(reinterpret_cast<char*>(&pModelMat[i].strTextureName[j][0]), sizeof(_char) * strLength);
                inputFile.read(reinterpret_cast<char*>(&pModelMat[i].iTypeTexture[j]), sizeof(_uint));
            }
        } 


        Safe_Delete_Array(pModelMat);
        inputFile.close();
    }

    return S_OK;
}

HRESULT CToolBar::Load_Scene()
{
    return S_OK;
}

HRESULT CToolBar::Save_Map()
{
    return S_OK;
}

HRESULT CToolBar::Load_Map()
{
    return S_OK;
}

HRESULT CToolBar::Save_Prefab()
{
    return E_NOTIMPL;
}

HRESULT CToolBar::Load_Prefab()
{
    return E_NOTIMPL;
}

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
