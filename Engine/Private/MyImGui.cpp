#include "MyImGui.h"
#include "GameInstance.h"
#include "Transform_Imgui.h"

using ImGuiCallBack = function<void()>;

#pragma region HelperFuntions
#pragma endregion

CMyImGui::CMyImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pGameInstance   { CGameInstance::Get_Instance() }
    , m_pDevice { pDevice }
    , m_pContext  { pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CMyImGui::Initialize(void* pArg)
{
    return S_OK;
}

void CMyImGui::Priority_Update(_float fTimeDelta)
{
}

void CMyImGui::Update(_float fTimeDelta)
{

}

void CMyImGui::Late_Update(_float fTimeDelta)
{
}

HRESULT CMyImGui::Pre_Render()
{
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    return S_OK;
}

HRESULT CMyImGui::Render()
{
    Pre_Render();

    bool show_demo_window = true;
    bool show_another_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }

    Post_Render();
    return S_OK;
}

HRESULT CMyImGui::Post_Render()
{
    ImGui::Render();
    /*const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
    m_pContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
    m_pContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);*/
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    return S_OK;
}

CMyImGui* CMyImGui::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CMyImGui* pInstance = new CMyImGui(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
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