#pragma once
#include "ToolBar.h"
#include "GameInstance.h"
#include "Level_Loading.h"

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
                    ifd::FileDialog::Instance().Open("TextureOpenDialog", "Open a texture", "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga;*.dds){.png,.jpg,.jpeg,.bmp,.tga,.dds},.*", true);
                    
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
