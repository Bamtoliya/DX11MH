#include "MyImGui.h"
#include "GameInstance.h"

#pragma region HelperFuntions
#pragma endregion

CMyImGui::CMyImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pGameInstance   { CGameInstance::Get_Instance() }
    , m_pDevice         { pDevice }
    , m_pContext        { pContext }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CMyImGui::Initialize(HWND hwnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(pDevice, pContext);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    return S_OK;
}

void CMyImGui::Update(_float fTimeDelta)
{

}

HRESULT CMyImGui::Render()
{
    return S_OK;
}

CMyImGui* CMyImGui::Create(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMyImGui* pInstance = new CMyImGui(pDevice, pContext);

    if (FAILED(pInstance->Initialize(hWnd, pDevice, pContext)))
    {
        MSG_BOX("Failed to Created : CMyImGui");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMyImGui::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
