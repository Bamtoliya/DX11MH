#include "MyImGui_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"
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
    for (auto& pair : m_MyImGuis)
    {
        Safe_Release(pair.second);
    }
    m_MyImGuis.clear();
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
#pragma endregion



void CMyImGui_Manager::Update(_float fTimeDelta)
{
    //for(auto& e : )
    for (auto& e : m_MyImGuis)
    {
        e.second->Update(fTimeDelta);
    }
    if (GetAsyncKeyState(VK_SHIFT) & GetAsyncKeyState(VK_TAB) & 0x8000)
    {

    }
    m_pGameObject = m_pGameInstance->Get_PickedObject();
}

HRESULT CMyImGui_Manager::Pre_Render()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    DrawToolBar(&m_bToolBar);
    return S_OK;
}

HRESULT CMyImGui_Manager::Render()
{
    Pre_Render();

    DrawDockingSpace(&m_bDockingSpace);

    for (auto& e : m_MyImGuis)
    {
        e.second->Render();
    }

    //DrawImGuizmo();

    _bool show = true;
    DrawAssetBrowser(&show);
    Post_Render();
    return S_OK;
}

HRESULT CMyImGui_Manager::Post_Render()
{
    ImGui::Render();
    /*const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
    m_pContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
    m_pContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);*/
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    return S_OK;
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
    static fs::path baseAssetPath = fs::absolute("../../Assets"); // 기본 애셋 경로 설정
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

void CMyImGui_Manager::DrawToolBar(bool* p_open)
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            // 파일 메뉴 아이템들
            if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
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
                printf("Toggle Asset Browser\n");
                // show_asset_browser = !show_asset_browser; // 실제 변수 토글
            }
            if (ImGui::MenuItem("Inspector", NULL /*, &show_inspector */)) {
                printf("Toggle Inspector\n");
                // show_inspector = !show_inspector;
            }
            ImGui::Separator();
            if (ImGui::MenuItem("ImGui Demo Window", NULL /*, &show_demo_window */)) {
                printf("Toggle ImGui Demo Window\n");
                // show_demo_window = !show_demo_window;
            }
            // ... 다른 창 메뉴 ...
            ImGui::EndMenu(); // Window 메뉴 끝
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
    // --- 메인 메뉴바 생성 끝 ---
}

void CMyImGui_Manager::DrawDockingSpace(bool* p_open)
{
    if (p_open)
    {
        // --- 도크스페이스 생성 (메뉴바 다음에 호출) ---
    // 메인 뷰포트 가져오기
        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        // 도킹 공간 플래그
        // 메뉴바가 있을 경우 DockSpaceOverViewport는 자동으로 메뉴바 아래 영역에 맞춰 생성됩니다.
        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

        // 뷰포트 위에 도킹 공간 생성
        ImGuiID dockspace_id = ImGui::GetID("MyFullscreenDockSpace");
        // DockSpaceOverViewport 호출 시 viewport 포인터를 두 번째 인자로 전달합니다.
        ImGui::DockSpaceOverViewport(dockspace_id, viewport, dockspace_flags);
    }
}

void CMyImGui_Manager::DrawImGuizmo()
{
    if (m_pGameObject)
    {
        CTransform* pTransform = static_cast<CTransform*>(m_pGameObject->Get_Component(g_strTransformTag)); // 2. Transform 컴포넌트 가져오기
        if (pTransform)
        {
            // 3. 카메라 행렬 가져오기
            D3D11_VIEWPORT vp;
            UINT numViewports = 1;
            m_pContext->RSGetViewports(&numViewports, &vp); // Get the main viewport
            _matrix viewMatrix = XMMatrixIdentity(); 
            
            _matrix projMatrix = XMMatrixOrthographicLH(vp.Width, vp.Height, 0.0f, 1.f); //m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);
            
            // 4. ImGuizmo 설정
            ImGuizmo::SetOrthographic(true); // 3D 뷰이므로 false

            // --- 중요: 기즈모 렌더링 영역 설정 ---
            // 예시: 현재 ImGui 윈도우 영역을 사용 (게임 뷰 윈도우가 별도로 있다면 해당 윈도우 정보 사용)
            ImVec2 windowPos = ImGui::GetWindowPos();
            ImVec2 windowSize = ImGui::GetWindowSize();
            ImVec2 contentMin = ImGui::GetWindowContentRegionMin(); // 내용 영역 시작점
            ImVec2 contentMax = ImGui::GetWindowContentRegionMax(); // 내용 영역 끝점
            float contentWidth = contentMax.x - contentMin.x;
            float contentHeight = contentMax.y - contentMin.y;
            ImGuizmo::SetRect(windowPos.x + contentMin.x, windowPos.y + contentMin.y, contentWidth, contentHeight);
            // -- 또는 렌더링 뷰포트 정보 직접 사용 --
            
            ImGuizmo::SetRect(vp.TopLeftX, vp.TopLeftY, vp.Width, vp.Height);


            // 5. 조작 모드 설정 (예시: ImGui 버튼 사용)
            static ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE;
            static ImGuizmo::MODE currentMode = ImGuizmo::WORLD;

            // --- ImGui 윈도우 내에 조작 모드 변경 버튼 추가 ---
            ImGui::Begin("Gizmo Controls"); // 별도 윈도우 또는 인스펙터 내부에 추가
            if (ImGui::RadioButton("Translate", currentOperation == ImGuizmo::TRANSLATE)) currentOperation = ImGuizmo::TRANSLATE; ImGui::SameLine();
            if (ImGui::RadioButton("Rotate", currentOperation == ImGuizmo::ROTATE)) currentOperation = ImGuizmo::ROTATE; ImGui::SameLine();
            if (ImGui::RadioButton("Scale", currentOperation == ImGuizmo::SCALE)) currentOperation = ImGuizmo::SCALE;

            if (currentOperation != ImGuizmo::SCALE)
            {
                if (ImGui::RadioButton("World", currentMode == ImGuizmo::WORLD)) currentMode = ImGuizmo::WORLD; ImGui::SameLine();
                if (ImGui::RadioButton("Local", currentMode == ImGuizmo::LOCAL)) currentMode = ImGuizmo::LOCAL;
            }
            else {
                currentMode = ImGuizmo::LOCAL; // Scale은 보통 Local 모드 사용
            }
            // -- 스냅 기능 추가 (선택 사항) --
            static bool useSnap = false;
            ImGui::Checkbox("Snap", &useSnap); ImGui::SameLine();
            static float snapValue[3] = { 0.1f, 0.1f, 0.1f }; // 스냅 값 (이동, 회전(도), 스케일)
            switch (currentOperation)
            {
            case ImGuizmo::TRANSLATE: ImGui::InputFloat3("Snap T", &snapValue[0]); break;
            case ImGuizmo::ROTATE:    ImGui::InputFloat("Snap R", &snapValue[0]); break;
            case ImGuizmo::SCALE:     ImGui::InputFloat("Snap S", &snapValue[0]); break;
            }
            ImGui::End(); // Gizmo Controls 윈도우 끝
            // ---------------------------------------------

            // 6. 객체의 월드 행렬 가져오기 및 ImGuizmo 용 float[16] 배열로 변환
            _matrix objectMatrix = pTransform->Get_WorldMatrix();
            float matrixPtr[16];
            XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(matrixPtr), objectMatrix);

            // View, Projection 행렬도 float[16]으로 변환
            float viewPtr[16];
            XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(viewPtr), viewMatrix);
            float projPtr[16];
            XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(projPtr), projMatrix);

            // 7. ImGuizmo 조작 함수 호출
            float* pSnap = useSnap ? snapValue : nullptr;
            if (ImGuizmo::Manipulate(viewPtr, projPtr, currentOperation, currentMode, matrixPtr, nullptr, pSnap /* snap, bounds */))
            {
                // 8. 조작 결과가 있다면 Transform 컴포넌트 업데이트
                _matrix updatedMatrix = XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(matrixPtr));
                pTransform->Set_WorldMatrix(updatedMatrix);
            }
        }
    }
}

HRESULT CMyImGui_Manager::Resize(_uint iWinSizeX, _uint iWinSizeY)
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    viewport->PlatformRequestResize = true;
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(static_cast<float>(iWinSizeX), static_cast<float>(iWinSizeY));
    // DPI 스케일링을 사용하지 않는 경우 보통 1.0f
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    return S_OK;
}

HRESULT CMyImGui_Manager::Add_ImGui(const _wstring& key, CMyImGui* pMyImGui)
{
    if (pMyImGui == nullptr)
        return E_FAIL;

    auto [iterator, success] = m_MyImGuis.try_emplace(key, pMyImGui);

    if (!success) 
        return E_FAIL;

    return S_OK;
}
