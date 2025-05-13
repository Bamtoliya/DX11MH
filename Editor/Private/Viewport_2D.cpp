#include "Viewport_2D.h"
#include "GameInstance.h"
#include "GameObject.h"
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
}

HRESULT CViewport_2D::Initialize(void* pArg)
{
    m_bShow = true;
    m_bShowImage = true;
    m_vImageGridPosition = { -(g_iWinSizeX/2.f), -(g_iWinSizeY / 2.f)}; // 그리드 (0,0) 기준 좌상단 좌표
    m_vImageGridSize = { g_iWinSizeX, g_iWinSizeY };   // 그리드 단위 크기
    return S_OK;
}

void CViewport_2D::Priority_Update(_float fTimeDelta)
{
}

void CViewport_2D::Update(_float fTimeDelta)
{
    m_pGameObject = m_pGameInstance->Get_PickedObject();
    //m_vImageGridPosition = { -(io.DisplaySize.x / 2.f), -(io.DisplaySize.y / 2.f) }; // 그리드 (0,0) 기준 좌상단 좌표
    //m_vImageGridSize = { io.DisplaySize.x, io.DisplaySize.y };   // 그리드 단위 크기
}

void CViewport_2D::Late_Update(_float fTimeDelta)
{
    
    //// DPI 스케일링을 사용하지 않는 경우 보통 1.0f
    //io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
}

// 그리드 좌표 -> 화면 좌표 변환 헬퍼 함수 구현
ImVec2 CViewport_2D::GridToScreen(const ImVec2& gridPos, const ImVec2& canvas_center, float zoom) {
    // 그리드 좌표계 원점 = canvas_center
    // 스케일 = zoom
    return ImVec2(canvas_center.x + gridPos.x * zoom, canvas_center.y + gridPos.y * zoom);
}

HRESULT CViewport_2D::Render()
{
    if (m_bShow)
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
                snprintf(click_info, sizeof(click_info), "Left Clicked at Grid(%.1f, %.1f)", mouse_pos_in_grid.x, mouse_pos_in_grid.y);
                // 여기서 last_left_click_grid_pos 같은 변수에 저장하거나 다른 작업 수행 가능

                _float2 ratio = { io.DisplaySize.x / g_iWinSizeX, io.DisplaySize.y / g_iWinSizeY };
                ImVec2 TexturePos = ImVec2(mouse_pos_in_grid.x * ratio.x + io.DisplaySize.x * 0.5f, mouse_pos_in_grid.y * ratio.y + io.DisplaySize.y * 0.5f);
                snprintf(mouse_cursor, sizeof(mouse_cursor), "TexturePos(%.1f, %.1f)", TexturePos.x, TexturePos.y);
                _float4 testColor = { 1.0f, 1.0f, 1.0f, 1.0f };
                if (FAILED(m_pGameInstance->ReadPickingTargetPixel((_uint)TexturePos.x, (_uint)TexturePos.y, &testColor)))
                    return E_FAIL;
                snprintf(click_color, sizeof(click_color), "Left Clicked Color(%.1f, %.1f, %.1f, %.1f)", testColor.x, testColor.y, testColor.z, testColor.w);
                m_pGameInstance->PickObjectByColor(&testColor);

            }
            else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                snprintf(click_info, sizeof(click_info), "Right Clicked at Grid(%.1f, %.1f)", mouse_pos_in_grid.x, mouse_pos_in_grid.y);
                // 여기서 last_right_click_grid_pos 같은 변수에 저장하거나 다른 작업 수행 가능
            }
        }

        //그리드 그리기
        DrawGrid(draw_list, canvas_p0, canvas_p1, canvas_center, zoom);

        // 해상도 영역 그리기 (canvas_center가 패닝을 반영하므로 로직 자체는 동일)
        DrawResolutionRect(draw_list, canvas_center, zoom);

        //렌더링 결과물
        DrawTexture(draw_list, canvas_center, zoom);

#pragma region Widgets
        ImGuiWindowFlags widget_window_flags = ImGuiWindowFlags_AlwaysAutoResize;
    if (ImGui::Begin("Widget Window", nullptr, widget_window_flags)) // p_open 인자를 nullptr로 전달하면 닫기 버튼이 생기지 않습니다. 필요하다면 bool 변수를 전달하세요.
    {
        ImGui::Text("Pan Offset: (%.1f, %.1f) (Right Mouse Click & Drag)", panning.x, panning.y);
        /*ImGui::TextUnformatted(mouse_cursor);
        ImGui::TextUnformatted(click_color);*/
        ImGui::TextUnformatted(click_info);
        if (ImGui::Button("Reset View")) {
            zoom = 1.0f; // 실제 zoom 변수 사용 시 주석 해제
            panning = ImVec2(0.0f, 0.0f); // 실제 panning 변수 사용 시 주석 해제
            snprintf(click_info, sizeof(click_info), "View Reset"); // 실제 click_info 변수 사용 시 주석 해제
        }
        ImGui::SameLine();
        ImGui::SliderFloat("Zoom Slider", &zoom, min_zoom, max_zoom);

    }
    ImGui::End(); // "Widget Window" 창 끝
#pragma endregion

        DrawImGuizmo2D(zoom, panning);

        ImGui::End(); // 창 끝
    }
    return S_OK;
}

HRESULT CViewport_2D::DrawGrid(ImDrawList* pList, ImVec2& p0, ImVec2& p1, ImVec2& center, _float fZoom)
{
    // --- 그리기 ---
    const float grid_base_spacing = 25.0f;
    const float apparent_spacing = grid_base_spacing * fZoom;
    const ImU32 grid_color = IM_COL32(200, 200, 200, 40);
    const ImU32 grid_thick_color = IM_COL32(200, 200, 200, 80);
    const int thick_line_interval = 4;

    // 그리드 그리기 (canvas_center가 패닝을 반영하므로 로직 자체는 동일)
    if (apparent_spacing > 1.0f) {
        // 수직선
        for (float x = center.x; x <= p1.x; x += apparent_spacing) {
            int line_index = static_cast<int>(roundf(fabsf(x - center.x) / apparent_spacing));
            bool is_thick_line = (line_index > 0 && line_index % thick_line_interval == 0);
            pList->AddLine(ImVec2(x, p0.y), ImVec2(x, p1.y), is_thick_line ? grid_thick_color : grid_color);
        }
        for (float x = center.x - apparent_spacing; x >= p0.x; x -= apparent_spacing) {
            int line_index = static_cast<int>(roundf(fabsf(x - center.x) / apparent_spacing));
            bool is_thick_line = (line_index > 0 && line_index % thick_line_interval == 0);
            pList->AddLine(ImVec2(x, p0.y), ImVec2(x, p1.y), is_thick_line ? grid_thick_color : grid_color);
        }
        // 수평선
        for (float y = center.y; y <= p1.y; y += apparent_spacing) {
            int line_index = static_cast<int>(roundf(fabsf(y - center.y) / apparent_spacing));
            bool is_thick_line = (line_index > 0 && line_index % thick_line_interval == 0);
            pList->AddLine(ImVec2(p0.x, y), ImVec2(p1.x, y), is_thick_line ? grid_thick_color : grid_color);
        }
        for (float y = center.y - apparent_spacing; y >= p0.y; y -= apparent_spacing) {
            int line_index = static_cast<int>(roundf(fabsf(y - center.y) / apparent_spacing));
            bool is_thick_line = (line_index > 0 && line_index % thick_line_interval == 0);
            pList->AddLine(ImVec2(p0.x, y), ImVec2(p1.x, y), is_thick_line ? grid_thick_color : grid_color);
        }
        // 중앙선
        pList->AddLine(ImVec2(center.x, p0.y), ImVec2(center.x, p1.y), grid_thick_color);
        pList->AddLine(ImVec2(p0.x, center.y), ImVec2(p1.x, center.y), grid_thick_color);
    }
    return S_OK;
}

HRESULT CViewport_2D::DrawResolutionRect(ImDrawList* pList, ImVec2& center, _float fZoom)
{
    // 해상도 영역 그리기 (canvas_center가 패닝을 반영하므로 로직 자체는 동일)
    const float targetResolutionWidth = 1920.0f;
    const float targetResolutionHeight = 1080.0f;
    const float scaledWidth = targetResolutionWidth * fZoom;
    const float scaledHeight = targetResolutionHeight * fZoom;
    ImVec2 rectP0 = ImVec2(center.x - scaledWidth * 0.5f, center.y - scaledHeight * 0.5f);
    ImVec2 rectP1 = ImVec2(center.x + scaledWidth * 0.5f, center.y + scaledHeight * 0.5f);
    const ImU32 rectColor = IM_COL32(0, 255, 0, 255);
    const float rectThickness = 2.0f;
    pList->AddRect(rectP0, rectP1, rectColor, 0.0f, ImDrawFlags_None, rectThickness);
    return S_OK;
}

HRESULT CViewport_2D::DrawTexture(ImDrawList* pList, ImVec2& center, _float fZoom)
{
    m_pSRV = m_pGameInstance->Get_SRV(L"RT_FinalScene");
    if (m_bShowImage && m_pSRV) // 이미지를 표시하고 SRV가 유효할 때만
    {
        // 1. 이미지의 그리드 좌표 계산 (m_vImageGridPosition이 좌상단이라고 가정)
        ImVec2 imageGridMin = { m_vImageGridPosition.x, m_vImageGridPosition.y };
        ImVec2 imageGridMax = { m_vImageGridPosition.x + m_vImageGridSize.x, m_vImageGridPosition.y + m_vImageGridSize.y };

        // 2. 그리드 좌표를 화면 좌표로 변환
        ImVec2 imageScreenMin = GridToScreen(imageGridMin, center, fZoom);
        ImVec2 imageScreenMax = GridToScreen(imageGridMax, center, fZoom);

        // 3. ImDrawList::AddImage() 호출하여 그리기
        pList->AddImage(
            (ImTextureID)m_pSRV,      // 텍스처의 Shader Resource View
            imageScreenMin,     // 화면 좌상단 좌표
            imageScreenMax,     // 화면 우하단 좌표
            ImVec2(0, 0),       // UV 시작 좌표 (텍스처 전체 사용)
            ImVec2(1, 1),       // UV 끝 좌표 (텍스처 전체 사용)
            IM_COL32_WHITE      // 틴트 색상 (흰색, 불투명). IM_COL32(R,G,B,A) 형태로 지정 가능.
        );

        // (선택 사항) 이미지 주변에 테두리 그리기 (디버깅용)
        //pList->AddRect(imageScreenMin, imageScreenMax, IM_COL32(255, 255, 0, 255));
    }
    return S_OK;
}

HRESULT CViewport_2D::DrawImGuizmo2D(_float fZoom, ImVec2& vPanning)
{
    if (m_pGameObject)
    {
        CTransform* pTransform = static_cast<CTransform*>(m_pGameObject->Get_Component(g_strTransformTag));
        if (pTransform)
        {
            // 2. 실제 카메라 행렬 가져오기 (GameInstance 사용)

            // View: 줌과 패닝을 적용 (카메라가 줌 아웃하고 패닝하는 효과)
            // 스케일 중심은 (0,0), 패닝은 화면 좌표 기준
            _matrix scaleMatrix = XMMatrixScaling(fZoom, fZoom, 1.f);
            // 패닝: 카메라가 -panning 만큼 이동하는 것과 동일
            _matrix panMatrix = XMMatrixTranslation(vPanning.x / fZoom, -vPanning.y / fZoom, 0.f);
            // 최종 View 행렬: 이동 -> 스케일 순서 적용
            _matrix viewMatrix = panMatrix * scaleMatrix;

            //_matrix viewMatrix = XMMatrixIdentity(); //m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
            

            // 3. ImGuizmo 설정
            // ImGuizmo::BeginFrame(); // Pre_Render에서 호출됨
            ImGuizmo::SetOrthographic(true); // 원근 투영이므로 false
            ImGuizmo::SetDrawlist();

            // --- 중요: 현재 ImGui 윈도우("Viewport_2D")의 내용 영역으로 SetRect 설정 ---
            ImVec2 windowPos = ImGui::GetWindowPos(); // Viewport_2D 윈도우의 화면 좌표
            ImVec2 contentMin = ImGui::GetWindowContentRegionMin(); // 내용 영역의 상대 좌표 (좌상단)
            ImVec2 contentMax = ImGui::GetWindowContentRegionMax(); // 내용 영역의 상대 좌표 (우하단)
            
            float contentWidth = contentMax.x - contentMin.x;
            float contentHeight = contentMax.y - contentMin.y;

            _matrix projMatrix = XMMatrixOrthographicLH(contentWidth, contentHeight, 0.0f, 1.0f);//m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ); // 원근 투영 행렬

            // 내용 영역의 화면 절대 좌표 및 크기 계산
            float viewManipulateLeft = windowPos.x + contentMin.x;
            float viewManipulateTop = windowPos.y + contentMin.y;

            // 유효한 영역인지 확인 후 설정
            if (contentWidth > 1.0f && contentHeight > 1.0f)
            {
                ImGuizmo::SetRect(viewManipulateLeft, viewManipulateTop, contentWidth, contentHeight);

                // 4. 행렬 준비 (float[16] 배열)
                _matrix objectMatrix = pTransform->Get_WorldMatrix();
                float matrixPtr[16], viewPtr[16], projPtr[16];
                XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(matrixPtr), objectMatrix);
                XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(viewPtr), viewMatrix);
                XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(projPtr), projMatrix);

                // 5. ImGuizmo 조작 함수 호출
                float* pSnap = m_bUseSnap ? m_fSnapValue : nullptr;
                if (ImGuizmo::Manipulate(viewPtr, projPtr, m_currentGizmoOperation, m_currentGizmoMode, matrixPtr, nullptr, pSnap))
                {
                    // 6. Transform 컴포넌트 업데이트
                    _matrix updatedMatrix = XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(matrixPtr));
                    pTransform->Set_WorldMatrix(updatedMatrix);
                }

                // (선택 사항) 그리드 및 큐브 그리기
                float identityMatrixPtr[16];
                XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(identityMatrixPtr), XMMatrixIdentity());
                //ImGuizmo::DrawGrid(viewPtr, projPtr, identityMatrixPtr, 10.0f); // 필요 시 활성화
                // ImGuizmo::DrawCubes(viewPtr, projPtr, matrixPtr, 1); // 필요 시 활성화
            }
            else
            {
                // 내용 영역이 너무 작거나 유효하지 않으면 기즈모를 그리지 않음
                // ImGui::Text("Viewport too small for Gizmo"); // 디버깅 메시지
            }
#pragma region Widgets
            // --- ImGui 윈도우 내에 조작 모드 변경 버튼 추가 ---
            ImGui::Begin("Gizmo Controls"); // 별도 윈도우 또는 인스펙터 내부에 추가
            if (ImGui::RadioButton("Translate", m_currentGizmoOperation == ImGuizmo::TRANSLATE)) m_currentGizmoOperation = ImGuizmo::TRANSLATE; ImGui::SameLine();
            if (ImGui::RadioButton("Rotate", m_currentGizmoOperation == ImGuizmo::ROTATE)) m_currentGizmoOperation = ImGuizmo::ROTATE; ImGui::SameLine();
            if (ImGui::RadioButton("Scale", m_currentGizmoOperation == ImGuizmo::SCALE)) m_currentGizmoOperation = ImGuizmo::SCALE;

            if (m_currentGizmoOperation != ImGuizmo::SCALE)
            {
                if (ImGui::RadioButton("World", m_currentGizmoMode == ImGuizmo::WORLD)) m_currentGizmoMode = ImGuizmo::WORLD; ImGui::SameLine();
                if (ImGui::RadioButton("Local", m_currentGizmoMode == ImGuizmo::LOCAL)) m_currentGizmoMode = ImGuizmo::LOCAL;
            }
            else {
                m_currentGizmoMode = ImGuizmo::LOCAL; // Scale은 보통 Local 모드 사용
            }
            // -- 스냅 기능 추가 (선택 사항) --
            ImGui::Checkbox("Snap", &m_bUseSnap); ImGui::SameLine();
            switch (m_currentGizmoOperation)
            {
            case ImGuizmo::TRANSLATE: ImGui::InputFloat3("Snap T", &m_fSnapValue[0]); break;
            case ImGuizmo::ROTATE:    ImGui::InputFloat("Snap R", &m_fSnapValue[0]); break;
            case ImGuizmo::SCALE:     ImGui::InputFloat("Snap S", &m_fSnapValue[0]); break;

            }
            ImGui::End(); // Gizmo Controls 윈도우 끝
#pragma endregion
        }
    }
    return S_OK;
}
