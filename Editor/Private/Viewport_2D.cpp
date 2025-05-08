#include "Viewport_2D.h"
#include "GameInstance.h"
#include "Texture.h"

CViewport_2D::CViewport_2D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMyImGui { pDevice, pContext }
{
}

CViewport_2D* CViewport_2D::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CViewport_2D* pInstance = new CViewport_2D(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CInspector");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CViewport_2D::Free()
{
    __super::Free();
    Safe_Release(m_pTextureCom);
}

HRESULT CViewport_2D::Initialize(void* pArg)
{
    m_bShow = true;
    m_bShowImage = true;
    m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, L"../../Assets/Texture/ui_common/uiRANGE_img_hologram3.dds", 1);
    if (m_pTextureCom == nullptr)
        return E_FAIL;
    //m_pSRV = m_pTextureCom->Get_SRV(0);
    m_pSRV = m_pGameInstance->Get_BackBufferSRV();
    m_vImageGridPosition = { -(g_iWinSizeX/2.f), -(g_iWinSizeY / 2.f)}; // 그리드 (0,0) 기준 좌상단 좌표
    m_vImageGridSize = { 1920.f, 1080.f };   // 그리드 단위 크기

    return S_OK;
}

void CViewport_2D::Priority_Update(_float fTimeDelta)
{
}

void CViewport_2D::Update(_float fTimeDelta)
{

}

void CViewport_2D::Late_Update(_float fTimeDelta)
{
}

// 그리드 좌표 -> 화면 좌표 변환 헬퍼 함수 구현
ImVec2 CViewport_2D::GridToScreen(const ImVec2& gridPos, const ImVec2& canvas_center, float zoom) {
    // 그리드 좌표계 원점 = canvas_center
    // 스케일 = zoom
    return ImVec2(canvas_center.x + gridPos.x * zoom, canvas_center.y + gridPos.y * zoom);
}

HRESULT CViewport_2D::Render()
{
    // 창 이름 변경 제안 (기능이 추가되었으므로)
    if (!ImGui::Begin("2D Viewport", &m_bShow)) {
        ImGui::End();
        return E_FAIL;
    }

    //MenuToolBar

    // --- 상태 변수 ---
    static float zoom = 1.0f;
    static ImVec2 panning = ImVec2(0.0f, 0.0f); // 그리드 원점 이동량 (패닝 오프셋) 추가
    static char mouse_cursor[128] = "Click on the grid area"; // 클릭 정보 표시용 문자열 추가
    static char click_info[128] = "Click on the grid area"; // 클릭 정보 표시용 문자열 추가
    static char click_color[128] = "Click on the grid area"; // 클릭 정보 표시용 문자열 추가
    

    // 줌 설정값
    const float zoom_speed = 0.1f;
    const float min_zoom = 0.01f;
    const float max_zoom = 10.0f;

    // --- 입력 처리 ---
    ImGuiIO& io = ImGui::GetIO();
    // IsWindowHovered: 팝업이나 다른 활성 아이템에 의해 가려지지 않았을 때도 Hover 상태를 감지하도록 플래그 추가 가능성 고려
    bool is_window_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

    if (is_window_hovered) { // 창 위에 마우스가 있을 때만 입력 처리
        // 줌 처리 (기존 코드)
        float wheel = io.MouseWheel;
        if (wheel != 0) {
            float scale_delta = wheel * zoom_speed * zoom;
            zoom += scale_delta;
            zoom = fmaxf(min_zoom, fminf(zoom, max_zoom));
        }

        // 패닝 처리 (가운데 마우스 버튼 드래그) --- 추가된 부분 ---
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
            panning += io.MouseDelta; // 마우스 이동량만큼 패닝 오프셋 누적
        }
    }

    // --- 좌표 및 그리기 준비 ---
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImVec2 win_pos = ImGui::GetWindowPos();
    ImVec2 content_min_rel = ImGui::GetWindowContentRegionMin();
    ImVec2 content_max_rel = ImGui::GetWindowContentRegionMax();
    ImVec2 canvas_p0 = ImVec2(win_pos.x + content_min_rel.x, win_pos.y + content_min_rel.y);
    ImVec2 canvas_p1 = ImVec2(win_pos.x + content_max_rel.x, win_pos.y + content_max_rel.y);
    ImVec2 canvas_size = ImVec2(canvas_p1.x - canvas_p0.x, canvas_p1.y - canvas_p0.y);

    // 중앙점 계산 시 패닝 오프셋 적용 --- 수정된 부분 ---
    ImVec2 initial_center = ImVec2(canvas_p0.x + canvas_size.x * 0.5f, canvas_p0.y + canvas_size.y * 0.5f);
    ImVec2 canvas_center = ImVec2(initial_center.x + panning.x, initial_center.y + panning.y);

    // 클릭 처리 및 좌표 변환 --- 추가된 부분 ---
    if (is_window_hovered) {
        ImVec2 mouse_pos_in_screen = io.MousePos;
        // 화면 좌표 -> 그리드 좌표 변환 (패닝된 중앙점과 줌 고려)
        ImVec2 mouse_pos_relative_to_center = ImVec2(mouse_pos_in_screen.x - canvas_center.x, mouse_pos_in_screen.y - canvas_center.y);
        ImVec2 mouse_pos_in_grid = ImVec2(mouse_pos_relative_to_center.x / zoom, mouse_pos_relative_to_center.y / zoom);
        

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            ImVec2 TexturePos = ImVec2(mouse_pos_in_grid.x + g_iWinSizeX * 0.5f, mouse_pos_in_grid.y + g_iWinSizeY * 0.5f);
            snprintf(mouse_cursor, sizeof(mouse_cursor), "Mouse Clicked at Screen(%.1f, %.1f)", TexturePos.x, TexturePos.y);
            snprintf(click_info, sizeof(click_info), "Left Clicked at Grid(%.1f, %.1f)", mouse_pos_in_grid.x, mouse_pos_in_grid.y);
            _float4 testColor = { 1.0f, 1.0f, 1.0f, 1.0f };
            if (FAILED(m_pGameInstance->ReadPickingTargetPixel(TexturePos.x, TexturePos.y, &testColor)))
                return E_FAIL;
            snprintf(click_color, sizeof(click_color), "Left Clicked Color(%.1f, %.1f, %.1f, %.1f)", testColor.x, testColor.y, testColor.z, testColor.w);


            //m_pGameInstance->PickObjectByColor(&testColor);

            int a = 10;
            // 여기서 last_left_click_grid_pos 같은 변수에 저장하거나 다른 작업 수행 가능
        }
        else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            snprintf(click_info, sizeof(click_info), "Right Clicked at Grid(%.1f, %.1f)", mouse_pos_in_grid.x, mouse_pos_in_grid.y);
            // 여기서 last_right_click_grid_pos 같은 변수에 저장하거나 다른 작업 수행 가능
        }
    }

    // --- 그리기 ---
    const float grid_base_spacing = 25.0f;
    const float apparent_spacing = grid_base_spacing * zoom;
    const ImU32 grid_color = IM_COL32(200, 200, 200, 40);
    const ImU32 grid_thick_color = IM_COL32(200, 200, 200, 80);
    const int thick_line_interval = 4;

    // 그리드 그리기 (canvas_center가 패닝을 반영하므로 로직 자체는 동일)
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

    // 해상도 영역 그리기 (canvas_center가 패닝을 반영하므로 로직 자체는 동일)
    const float targetResolutionWidth = 1920.0f;
    const float targetResolutionHeight = 1080.0f;
    const float scaledWidth = targetResolutionWidth * zoom;
    const float scaledHeight = targetResolutionHeight * zoom;
    ImVec2 rectP0 = ImVec2(canvas_center.x - scaledWidth * 0.5f, canvas_center.y - scaledHeight * 0.5f);
    ImVec2 rectP1 = ImVec2(canvas_center.x + scaledWidth * 0.5f, canvas_center.y + scaledHeight * 0.5f);
    const ImU32 rectColor = IM_COL32(0, 255, 0, 255);
    const float rectThickness = 2.0f;
    draw_list->AddRect(rectP0, rectP1, rectColor, 0.0f, ImDrawFlags_None, rectThickness);

    m_pSRV = m_pGameInstance->Get_SRV(L"RT_Picking");
    if (m_bShowImage && m_pSRV) // 이미지를 표시하고 SRV가 유효할 때만
    {
        // 1. 이미지의 그리드 좌표 계산 (m_vImageGridPosition이 좌상단이라고 가정)
        ImVec2 imageGridMin = { m_vImageGridPosition.x, m_vImageGridPosition.y };
        ImVec2 imageGridMax = { m_vImageGridPosition.x + m_vImageGridSize.x, m_vImageGridPosition.y + m_vImageGridSize.y };

        // 2. 그리드 좌표를 화면 좌표로 변환
        ImVec2 imageScreenMin = GridToScreen(imageGridMin, canvas_center, zoom);
        ImVec2 imageScreenMax = GridToScreen(imageGridMax, canvas_center, zoom);

        // 3. ImDrawList::AddImage() 호출하여 그리기
        draw_list->AddImage(
            (ImTextureID)m_pSRV,      // 텍스처의 Shader Resource View
            imageScreenMin,     // 화면 좌상단 좌표
            imageScreenMax,     // 화면 우하단 좌표
            ImVec2(0, 0),       // UV 시작 좌표 (텍스처 전체 사용)
            ImVec2(1, 1),       // UV 끝 좌표 (텍스처 전체 사용)
            IM_COL32_WHITE      // 틴트 색상 (흰색, 불투명). IM_COL32(R,G,B,A) 형태로 지정 가능.
        );

        // (선택 사항) 이미지 주변에 테두리 그리기 (디버깅용)
        draw_list->AddRect(imageScreenMin, imageScreenMax, IM_COL32(255, 255, 0, 255));
    }

    


    // --- UI 위젯 (상태 표시 및 제어) ---
    // 커서 위치를 창 좌상단 기준으로 설정 (정보 표시용)
    ImGui::SetCursorPos(ImVec2(content_min_rel.x + 10, content_min_rel.y + 10));
    ImGui::BeginGroup(); // 정보 텍스트 그룹화 (배경 추가 등에 용이)
    ImGui::Text("Interactive Grid");
    ImGui::Text("Zoom: %.2f (Mouse Wheel)", zoom);
    ImGui::Text("Pan Offset: (%.1f, %.1f) (Right Mouse Click & Drag)", panning.x, panning.y); // 패닝 정보 표시 추가
    ImGui::TextUnformatted(mouse_cursor); // 클릭 정보 표시
    ImGui::TextUnformatted(click_info); // 클릭 정보 표시
    ImGui::TextUnformatted(click_color); // 클릭 정보 표시
    if (ImGui::Button("Reset View")) { // 리셋 버튼 추가
        zoom = 1.0f;
        panning = ImVec2(0.0f, 0.0f);
        snprintf(click_info, sizeof(click_info), "View Reset");
    }
    ImGui::EndGroup();

    // 줌 슬라이더는 정보 텍스트와 분리하여 다른 곳에 배치할 수도 있음
    ImGui::SetCursorPosX(content_min_rel.x + 10); // 예시
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10); // 예시
    ImGui::SliderFloat("Zoom Slider", &zoom, min_zoom, max_zoom);

   

    ImGui::End(); // 창 끝
    return S_OK;
}
