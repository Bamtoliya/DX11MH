#include "MyImGui_Manager.h"
#include "GameInstance.h"
#include "MyImGui.h"

using ImGuiCallBack = function<void()>;

namespace fs = std::filesystem;

#pragma region HelperFuntions
#pragma endregion

#pragma region Constructor/Destructor
CMyImGui_Manager::CMyImGui_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pGameInstance{ CGameInstance::Get_Instance() }
    , m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CMyImGui_Manager::Initialize(HWND hwnd)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Docking

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    return S_OK;
}

CMyImGui_Manager* CMyImGui_Manager::Create(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMyImGui_Manager* pInstance = new CMyImGui_Manager(pDevice, pContext);

    if (FAILED(pInstance->Initialize(hWnd)))
    {
        MSG_BOX("Failed to Created : CMyImGui_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMyImGui_Manager::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    for (auto& pair : m_vecMyImGui)
    {
        Safe_Release(pair.second);
    }
    m_vecMyImGui.clear();
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
#pragma endregion



void CMyImGui_Manager::Update(_float fTimeDelta)
{
    //for(auto& e : )
    for (auto& e : m_vecMyImGui)
    {
        e.second->Update(fTimeDelta);
    }
    if (GetAsyncKeyState(VK_SHIFT) & GetAsyncKeyState(VK_TAB) & 0x8000)
    {

    }
}

HRESULT CMyImGui_Manager::Render()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    for (auto& e : m_vecMyImGui)
    {
        e.second->Render();
    }

    bool show_demo_window = true;
    bool show_another_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    //// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    //if (show_demo_window)
    //    ImGui::ShowDemoWindow(&show_demo_window);

    //// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    //{
    //    static float f = 0.0f;
    //    static int counter = 0;

    //    ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

    //    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    //    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
    //    ImGui::Checkbox("Another Window", &show_another_window);

    //    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    //    ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    //    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
    //        counter++;
    //    ImGui::SameLine();
    //    ImGui::Text("counter = %d", counter);

    //    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    //    ImGui::End();
    //}
    // --- 사전 설정 (초기화 시 한 번만) ---
    // ImGuiIO& io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // 도킹 활성화
    // ------------------------------------

    // 메인 뷰포트 가져오기
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    // 도킹 공간 플래그 (선택 사항)
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode; // 중앙 노드 비활성 시 입력 통과

    // 뷰포트 위에 도킹 공간 생성
    // 이 함수는 내부적으로 전체 화면 크기의 투명한 창을 만들고 그 위에 도킹 노드를 설정합니다.
    ImGuiID dockspace_id = ImGui::GetID("MyFullscreenDockSpace");
    ImGui::DockSpaceOverViewport(dockspace_id, viewport, dockspace_flags, nullptr); // 세 번째 인자는 창 클래스(고급)

    // --- 이제 다른 ImGui 창들을 Begin/End로 그리면 ---
    // --- 이 도킹 공간에 붙이거나 플로팅 상태로 둘 수 있습니다 ---
    // 예:
    

    _bool show = true;
    DrawGridWindow(&show);
    DrawAssetBrowser(&show);

    // 만약 이 전체 화면 도킹 공간 자체에 무언가를 그리고 싶다면,
    // 약간의 트릭이 필요하거나 (예: 매우 큰 더미 창을 중앙 노드에 도킹)
    // 혹은 이 방법은 다른 창을 도킹시키는 배경 용도로만 사용하는 것이 일반적입니다.

    // 3. Show another simple window.
    //if (show_another_window)
    //{
    //    // 도킹 기능을 사용하려면 ImGui 초기화 시 io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; 설정이 필요합니다.

    //    // 창을 시작합니다. 이 창이 도킹 공간의 컨테이너 역할을 합니다.
    //    ImGui::Begin("Another Window (DockSpace)", &show_another_window); // 창 이름을 좀 더 명확하게 변경하는 것이 좋습니다.

    //    // 도킹 공간 ID를 가져옵니다.
    //    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");

    //    // 도킹 공간 플래그를 설정합니다.
    //    // ImGuiDockNodeFlags_PassthruCentralNode 플래그는 중앙 노드가 비어 있을 때
    //    // 뒤의 창과 상호작용할 수 있게 해줍니다 (메인 뷰포트를 덮는 경우 유용).
    //    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    //    // 현재 창 내부에 도킹 공간을 만듭니다.
    //    // ImVec2(0.0f, 0.0f)는 사용 가능한 전체 공간을 사용하라는 의미입니다.
    //    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    //    /*
    //    // 주석 처리: 도킹 공간으로 사용하는 창 내부에 직접 컨트롤을 넣는 것은
    //    //             다른 창이 도킹되면 가려질 수 있으므로 일반적이지 않습니다.
    //    //             창 자체의 닫기 버튼 (Begin 함수의 두 번째 인자)으로 충분할 수 있습니다.
    //    if (ImGui::Button("Close Me"))
    //        show_another_window = false;
    //    */

    //    // 창을 닫습니다.
    //    ImGui::End();
    //}

    ImGui::Render();
    /*const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
    m_pContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
    m_pContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);*/
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return S_OK;
}



void CMyImGui_Manager::DrawGridWindow(bool* p_open)
{
    if (!ImGui::Begin("Zoomable Centered Grid with Bounds", p_open)) {
        ImGui::End();
        return;
    }

    static float zoom = 1.0f;
    const float zoom_speed = 0.1f;
    const float min_zoom = 0.01f; // 최소 줌을 더 작게 설정해야 작은 해상도 영역도 보임
    const float max_zoom = 10.0f;

    ImGuiIO& io = ImGui::GetIO();
    if (ImGui::IsWindowHovered()) {
        float wheel = io.MouseWheel;
        if (wheel != 0) {
            float scale_delta = wheel * zoom_speed * zoom;
            zoom += scale_delta;
            zoom = fmaxf(min_zoom, fminf(zoom, max_zoom));
        }
    }

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImVec2 win_pos = ImGui::GetWindowPos();
    ImVec2 content_min_rel = ImGui::GetWindowContentRegionMin();
    ImVec2 content_max_rel = ImGui::GetWindowContentRegionMax();
    ImVec2 canvas_p0 = ImVec2(win_pos.x + content_min_rel.x, win_pos.y + content_min_rel.y);
    ImVec2 canvas_p1 = ImVec2(win_pos.x + content_max_rel.x, win_pos.y + content_max_rel.y);
    ImVec2 canvas_size = ImVec2(canvas_p1.x - canvas_p0.x, canvas_p1.y - canvas_p0.y);
    ImVec2 canvas_center = ImVec2(canvas_p0.x + canvas_size.x * 0.5f, canvas_p0.y + canvas_size.y * 0.5f);

    const float grid_base_spacing = 25.0f;
    const float apparent_spacing = grid_base_spacing * zoom;
    const ImU32 grid_color = IM_COL32(200, 200, 200, 40);
    const ImU32 grid_thick_color = IM_COL32(200, 200, 200, 80);
    const int thick_line_interval = 4;

    // 그리드 그리기 (이전 코드와 유사)
    if (apparent_spacing > 1.0f) {
        // 수직선
        for (float x = canvas_center.x; x <= canvas_p1.x; x += apparent_spacing) {
            int line_index = static_cast<int>(roundf(fabsf(x - canvas_center.x) / apparent_spacing));
            bool is_thick_line = (line_index > 0 && line_index % thick_line_interval == 0);
            draw_list->AddLine(ImVec2(x, canvas_p0.y), ImVec2(x, canvas_p1.y), is_thick_line ? grid_thick_color : grid_color);
        }
        for (float x = canvas_center.x - apparent_spacing; x >= canvas_p0.x; x -= apparent_spacing) {
            int line_index = static_cast<int>(roundf(fabsf(x - canvas_center.x) / apparent_spacing));
            bool is_thick_line = (line_index > 0 && line_index % thick_line_interval == 0);
            draw_list->AddLine(ImVec2(x, canvas_p0.y), ImVec2(x, canvas_p1.y), is_thick_line ? grid_thick_color : grid_color);
        }
        // 수평선
        for (float y = canvas_center.y; y <= canvas_p1.y; y += apparent_spacing) {
            int line_index = static_cast<int>(roundf(fabsf(y - canvas_center.y) / apparent_spacing));
            bool is_thick_line = (line_index > 0 && line_index % thick_line_interval == 0);
            draw_list->AddLine(ImVec2(canvas_p0.x, y), ImVec2(canvas_p1.x, y), is_thick_line ? grid_thick_color : grid_color);
        }
        for (float y = canvas_center.y - apparent_spacing; y >= canvas_p0.y; y -= apparent_spacing) {
            int line_index = static_cast<int>(roundf(fabsf(y - canvas_center.y) / apparent_spacing));
            bool is_thick_line = (line_index > 0 && line_index % thick_line_interval == 0);
            draw_list->AddLine(ImVec2(canvas_p0.x, y), ImVec2(canvas_p1.x, y), is_thick_line ? grid_thick_color : grid_color);
        }
        // 중앙선
        draw_list->AddLine(ImVec2(canvas_center.x, canvas_p0.y), ImVec2(canvas_center.x, canvas_p1.y), grid_thick_color);
        draw_list->AddLine(ImVec2(canvas_p0.x, canvas_center.y), ImVec2(canvas_p1.x, canvas_center.y), grid_thick_color);
    }

    // --- 여기서부터 해상도 영역 그리기 ---
    // 1. 목표 해상도 정의 (예: 16:9 비율)
    const float targetResolutionWidth = 1920.0f;  // 줌 1.0일 때의 가로 크기
    const float targetResolutionHeight = 1080.0f; // 줌 1.0일 때의 세로 크기

    // 2. 현재 줌에 맞춰 화면에 표시될 크기 계산
    const float scaledWidth = targetResolutionWidth * zoom;
    const float scaledHeight = targetResolutionHeight * zoom;

    // 3. 중앙을 기준으로 사각형의 좌상단, 우하단 좌표 계산
    ImVec2 rectP0 = ImVec2(canvas_center.x - scaledWidth * 0.5f,
        canvas_center.y - scaledHeight * 0.5f);
    ImVec2 rectP1 = ImVec2(canvas_center.x + scaledWidth * 0.5f,
        canvas_center.y + scaledHeight * 0.5f);

    // 4. 초록색 외곽선 그리기
    const ImU32 rectColor = IM_COL32(0, 255, 0, 255); // 완전한 초록색
    const float rectThickness = 2.0f;                 // 선 두께

    // AddRect(좌상단, 우하단, 색상, 둥근정도=0, 플래그=없음, 두께)
    draw_list->AddRect(rectP0, rectP1, rectColor, 0.0f, ImDrawFlags_None, rectThickness);
    // --- 해상도 영역 그리기 끝 ---


    // (선택) 그리드 및 사각형 위에 다른 ImGui 위젯 배치
    ImGui::SetCursorPos(ImVec2(content_min_rel.x + 10, content_min_rel.y + 10));
    ImGui::Text("Zoomable Centered Grid with Bounds");
    ImGui::Text("Zoom: %.2f", zoom);
    ImGui::Text("Target Res: %.0fx%.0f", targetResolutionWidth, targetResolutionHeight);
    ImGui::Text("Scaled Size: %.1fx%.1f", scaledWidth, scaledHeight);
    ImGui::SliderFloat("Zoom Slider", &zoom, min_zoom, max_zoom);

    ImGui::End();
}

void CMyImGui_Manager::DrawAssetBrowser(bool* p_open)
{
    if (!ImGui::Begin("Asset Browser", p_open)) {
        ImGui::End();
        return;
    }

    // 상태 유지를 위한 static 변수
    // 중요: "./Assets" 디렉토리가 실행 파일 위치 기준으로 존재해야 합니다.
    // 또는 실제 프로젝트의 애셋 경로로 변경하세요.
    static fs::path baseAssetPath = fs::absolute("./Assets"); // 기본 애셋 경로 설정
    static fs::path currentPath = baseAssetPath;
    static std::string selectedAsset = ""; // 선택된 파일 이름
    static std::string selectedFolder = ""; // 선택된 폴더 이름 (클릭 피드백용)

    // 현재 경로 표시
    ImGui::Text("Current Path: ");
    ImGui::SameLine();
    // 경로가 너무 길면 줄바꿈될 수 있도록 TextWrapped 사용
    ImGui::TextWrapped("%s", currentPath.string().c_str());

    ImGui::Separator();

    // 상위 폴더로 이동 버튼 ("..")
    if (currentPath != baseAssetPath && currentPath.has_parent_path()) {
        if (ImGui::Selectable("[..] (Parent Directory)", false, ImGuiSelectableFlags_AllowDoubleClick)) {
            if (ImGui::IsMouseDoubleClicked(0)) { // 더블 클릭 시 이동
                currentPath = currentPath.parent_path();
                selectedAsset = ""; // 선택 해제
                selectedFolder = "";
            }
        }
    }

    // 현재 경로의 디렉토리와 파일 목록 읽기
    std::vector<fs::path> directories;
    std::vector<fs::path> files;

    try {
        for (const auto& entry : fs::directory_iterator(currentPath)) {
            if (entry.is_directory()) {
                directories.push_back(entry.path());
            }
            else if (entry.is_regular_file()) {
                // (선택 사항) 특정 확장자만 표시할 경우 여기서 필터링
                // if (entry.path().extension() == ".png" || entry.path().extension() == ".jpg") {
                files.push_back(entry.path());
                // }
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error accessing path: %s", e.what());
    }

    // 이름순으로 정렬 (선택 사항)
    std::sort(directories.begin(), directories.end());
    std::sort(files.begin(), files.end());

    // 폴더 목록 표시
    for (const auto& dirPath : directories) {
        std::string dirName = dirPath.filename().string();
        std::string label = "[D] " + dirName; // 폴더임을 표시
        bool isSelected = (selectedFolder == dirName);

        if (ImGui::Selectable(label.c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick)) {
            // 선택 시 피드백을 위해 selectedFolder 업데이트
            selectedFolder = dirName;
            selectedAsset = ""; // 파일 선택 해제

            if (ImGui::IsMouseDoubleClicked(0)) { // 더블 클릭 시 해당 폴더로 이동
                currentPath = dirPath;
                selectedFolder = ""; // 이동 후 선택 해제
            }
        }
        // (구현 TBD) 드래그 앤 드롭 소스 (폴더)
        // (구현 TBD) 컨텍스트 메뉴 (폴더 우클릭)
    }

    // 파일 목록 표시
    for (const auto& filePath : files) {
        std::string fileName = filePath.filename().string();
        std::string label = "[F] " + fileName; // 파일임을 표시
        bool isSelected = (selectedAsset == fileName);

        if (ImGui::Selectable(label.c_str(), isSelected)) {
            // 파일 선택 시 selectedAsset 업데이트
            selectedAsset = fileName;
            selectedFolder = ""; // 폴더 선택 해제
            // (구현 TBD) 파일 선택 시 인스펙터 업데이트 등의 로직 호출
        }
        // (구현 TBD) 드래그 앤 드롭 소스 (파일 애셋)
        // (구현 TBD) 컨텍스트 메뉴 (파일 우클릭)
    }

    ImGui::Separator();
    ImGui::Text("Selected: %s", selectedAsset.empty() ? "None" : selectedAsset.c_str());

    ImGui::End();
}

HRESULT CMyImGui_Manager::AddtoImGuiManager(const _wstring key, CMyImGui* pMyImGui)
{
    if (pMyImGui == nullptr)
        return E_FAIL;

    auto [iterator, success] = m_vecMyImGui.try_emplace(key, pMyImGui);

    if (!success) 
        return E_FAIL;

    return S_OK;
}
