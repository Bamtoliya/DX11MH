#include "Viewport_3D.h"
#include "GameInstance.h"

CViewport_3D::CViewport_3D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMyImGui { pDevice, pContext }
{
}

CViewport_3D* CViewport_3D::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CViewport_3D* pInstance = new CViewport_3D(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CInspector");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CViewport_3D::Free()
{
    __super::Free();
}

HRESULT CViewport_3D::Initialize(void* pArg)
{
    m_bShow = true;
    m_bRenderTargetDirty = true; // 처음에는 RT를 생성해야 함

    //// Target Manager 가져오기 (GameInstance를 통해 접근한다고 가정)
    //if (FAILED(m_pGameInstance->Add_RenderTarget(
    //    m_strRenderTargetTag,
    //    1, 1, // 초기 크기
    //    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, // SRGB 포맷 사용 권장 (ImGui 호환성)
    //    _float4(0.1f, 0.1f, 0.1f, 1.0f)  // 초기 배경색
    //)))
    //    return E_FAIL;
    // 생성된 RenderTarget 포인터 가져오기 (선택적, 필요 시 사용)
    //m_pViewportRT = pTargetManager->Find_RenderTarget(m_strRenderTargetTag); // 이런 함수가 TargetManager에 있다고 가정

    return S_OK;
}

void CViewport_3D::Update(_float fTimeDelta)
{
}

HRESULT CViewport_3D::Render()
{
    // ImGui 창 시작 - 이 창이 뷰포트 영역이 됩니다.
    // 창 플래그를 조정하여 제목 표시줄, 배경 등을 없앨 수 있습니다.
    // 예: ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse; // 필요에 따라 플래그 추가
    if (!ImGui::Begin("3D Viewport", &m_bShow, window_flags))
    {
        ImGui::End();
        return E_FAIL;
    }

    // --- 상태 변수 (줌, 패닝, 클릭 정보) ---
    // static 변수는 이 함수가 여러 뷰포트에서 재사용될 경우 문제가 될 수 있습니다.
    // 실제 앱에서는 이 상태를 뷰포트별로 관리하는 것이 좋습니다. (예: 클래스 멤버)
    static float zoom = 1.0f;
    static ImVec2 panning = ImVec2(0.0f, 0.0f);
    static char click_info[128] = "Grid Ready";

    // --- 입력 처리 (줌, 패닝) ---
    ImGuiIO& io = ImGui::GetIO();
    bool is_window_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

    if (is_window_hovered) {
        // 줌 (마우스 휠)
        float wheel = io.MouseWheel;
        if (wheel != 0) {
            const float zoom_speed = 0.1f;
            const float min_zoom = 0.01f;
            const float max_zoom = 10.0f;
            float scale_delta = wheel * zoom_speed * zoom;
            zoom += scale_delta;
            zoom = fmaxf(min_zoom, fminf(zoom, max_zoom));
        }

        // 패닝 (우클릭 드래그)
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
            panning += io.MouseDelta;
        }
    }

    // --- 좌표 및 그리기 준비 ---
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 win_pos = ImGui::GetWindowPos(); // 현재 Begin된 창의 위치
    ImVec2 content_min_rel = ImGui::GetWindowContentRegionMin();
    ImVec2 content_max_rel = ImGui::GetWindowContentRegionMax();
    ImVec2 canvas_p0 = ImVec2(win_pos.x + content_min_rel.x, win_pos.y + content_min_rel.y);
    ImVec2 canvas_p1 = ImVec2(win_pos.x + content_max_rel.x, win_pos.y + content_max_rel.y);
    ImVec2 canvas_size = ImVec2(canvas_p1.x - canvas_p0.x, canvas_p1.y - canvas_p0.y);
    ImVec2 initial_center = ImVec2(canvas_p0.x + canvas_size.x * 0.5f, canvas_p0.y + canvas_size.y * 0.5f);
    ImVec2 canvas_center = ImVec2(initial_center.x + panning.x, initial_center.y + panning.y); // 패닝 적용된 중심

    // --- 클릭 처리 및 좌표 변환 ---
    if (is_window_hovered) {
        ImVec2 mouse_pos_in_screen = io.MousePos;
        ImVec2 mouse_pos_relative_to_center = ImVec2(mouse_pos_in_screen.x - canvas_center.x, mouse_pos_in_screen.y - canvas_center.y);
        ImVec2 mouse_pos_in_grid = ImVec2(mouse_pos_relative_to_center.x / zoom, mouse_pos_relative_to_center.y / zoom);

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left)) { // 드래그 중이 아닐 때의 클릭만 처리
            snprintf(click_info, sizeof(click_info), "Left Click @ Grid(%.1f, %.1f)", mouse_pos_in_grid.x, mouse_pos_in_grid.y);
        }
        else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            snprintf(click_info, sizeof(click_info), "Right Click @ Grid(%.1f, %.1f)", mouse_pos_in_grid.x, mouse_pos_in_grid.y);
        }
    }

    // --- 그리드 및 경계선 그리기 ---
    // (이전 코드와 거의 동일, canvas_center 사용)
    const float grid_base_spacing = 25.0f;
    const float apparent_spacing = grid_base_spacing * zoom;
    const ImU32 grid_color = IM_COL32(128, 128, 128, 40); // 색상 약간 변경
    const ImU32 grid_thick_color = IM_COL32(128, 128, 128, 80);
    const int thick_line_interval = 4;

    if (apparent_spacing > 1.5f) { // 최소 간격 조정
        // 수직선, 수평선, 중앙선 그리기... (이전 코드 내용 붙여넣기)
        for (float x = canvas_center.x; x <= canvas_p1.x; x += apparent_spacing) { /* ... */ }
        for (float x = canvas_center.x - apparent_spacing; x >= canvas_p0.x; x -= apparent_spacing) { /* ... */ }
        for (float y = canvas_center.y; y <= canvas_p1.y; y += apparent_spacing) { /* ... */ }
        for (float y = canvas_center.y - apparent_spacing; y >= canvas_p0.y; y -= apparent_spacing) { /* ... */ }
        draw_list->AddLine(ImVec2(canvas_center.x, canvas_p0.y), ImVec2(canvas_center.x, canvas_p1.y), grid_thick_color);
        draw_list->AddLine(ImVec2(canvas_p0.x, canvas_center.y), ImVec2(canvas_p1.x, canvas_center.y), grid_thick_color);
    }
    // 해상도 경계선 그리기... (이전 코드 내용 붙여넣기)
    const float targetResolutionWidth = 1920.0f;
    const float targetResolutionHeight = 1080.0f;
    const float scaledWidth = targetResolutionWidth * zoom;
    const float scaledHeight = targetResolutionHeight * zoom;
    ImVec2 rectP0 = ImVec2(canvas_center.x - scaledWidth * 0.5f, canvas_center.y - scaledHeight * 0.5f);
    ImVec2 rectP1 = ImVec2(canvas_center.x + scaledWidth * 0.5f, canvas_center.y + scaledHeight * 0.5f);
    const ImU32 rectColor = IM_COL32(0, 255, 0, 100); // 약간 투명하게 변경
    const float rectThickness = 1.5f;
    draw_list->AddRect(rectP0, rectP1, rectColor, 0.0f, ImDrawFlags_None, rectThickness);


    // --- 이 함수 호출 이후에 3D 렌더 결과(ImGui::Image)나 ImGuizmo 등을 그리면 ---
    // --- 이 그리드 위에 겹쳐서 보이게 됩니다. ---


    // --- (선택 사항) 디버그 정보 표시 ---
    // 화면 좌상단에 상태 정보 표시 (그리드 위에 그려짐)
    ImGui::SetCursorPos(ImVec2(content_min_rel.x + 5, content_min_rel.y + 5));
    ImGui::BeginGroup();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 0.8f)); // 약간 흐리게
    ImGui::Text("Zoom: %.2f", zoom);
    ImGui::Text("Pan: (%.0f, %.0f)", panning.x, panning.y);
    ImGui::TextUnformatted(click_info);
    ImGui::PopStyleColor();
    // 리셋 버튼은 필요하다면 다른 곳 (예: 메뉴)에서 처리하는 것이 더 깔끔할 수 있음
    // if (ImGui::Button("Reset")) { zoom = 1.0f; panning = ImVec2(0,0); }
    ImGui::EndGroup();


    ImGui::End(); // 뷰포트 창 끝
    return S_OK;
}

HRESULT CViewport_3D::EnsureRenderTargetSize(UINT width, UINT height)
{
    //if (width == 0 || height == 0) {
    //    m_bRenderTargetDirty = true; // 크기가 0이면 유효하지 않으므로 다시 생성 필요
    //    return S_OK; // 0 크기로는 생성/조정하지 않음
    //}

    //if (m_bRenderTargetDirty || m_iRenderTargetWidth != width || m_iRenderTargetHeight != height)
    //{
    //    m_iRenderTargetWidth = width;
    //    m_iRenderTargetHeight = height;

    //    CTarget_Manager* pTargetManager = m_pGameInstance->GetTargetManager();
    //    if (!pTargetManager) return E_FAIL;

    //    // TargetManager를 통해 RenderTarget 재생성 또는 크기 변경
    //    // 1. 기존 RT 제거 (만약 존재하면)
    //    pTargetManager->Remove_RenderTarget(m_strRenderTargetTag); // 이런 함수가 TargetManager에 필요

    //    // 2. 새 크기로 RT 추가
    //    HRESULT hr = pTargetManager->Add_RenderTarget(
    //        m_strRenderTargetTag,
    //        m_iRenderTargetWidth,
    //        m_iRenderTargetHeight,
    //        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, // SRGB 포맷
    //        _float4(0.1f, 0.1f, 0.1f, 1.0f)
    //    );

    //    if (FAILED(hr))
    //    {
    //        MSG_BOX("Failed to resize/recreate RenderTarget for Viewport_3D");
    //        m_bRenderTargetDirty = true; // 실패 시 다시 시도하도록 플래그 유지
    //        return hr;
    //    }

    //    // 새 RenderTarget 포인터 업데이트 (선택적)
    //    m_pViewportRT = pTargetManager->Find_RenderTarget(m_strRenderTargetTag);
    //    m_bRenderTargetDirty = false; // 성공적으로 처리되었으므로 플래그 리셋

    //    OutputDebugStringA("Resized Viewport 3D RenderTarget\n");
    //}
    return S_OK;
}

void CViewport_3D::RenderSceneToRenderTarget()
{
}

void CViewport_3D::DrawGridInWindow(ImDrawList* draw_list, const ImVec2& canvas_origin, const ImVec2& canvas_size, const ImVec2& canvas_center, float zoom)
{
}
