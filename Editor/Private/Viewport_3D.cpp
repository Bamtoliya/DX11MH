#include "Viewport_3D.h"

#include "GameObject.h"
#include "GameInstance.h"


#pragma region Constructor/Destructor
CViewport_3D::CViewport_3D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMyImGui{ pDevice, pContext }
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
    m_strRenderTargetKey = L"RT_Skybox";
    m_vImageGridPosition = { -(g_iWinSizeX / 2.f), -(g_iWinSizeY / 2.f) }; // 그리드 (0,0) 기준 좌상단 좌표
    m_vImageGridSize = { g_iWinSizeX, g_iWinSizeY };   // 그리드 단위 크기
    return S_OK;
}
#pragma endregion

void CViewport_3D::Update(_float fTimeDelta)
{
    if (KEY_DOWN(DIK_L))
    {
        m_bLockPicked = !m_bLockPicked;
    }
}

HRESULT CViewport_3D::Render()
{
    // ImGui 창 시작 - 이 창이 뷰포트 영역이 됩니다.
    // 창 플래그를 조정하여 제목 표시줄, 배경 등을 없앨 수 있습니다.
    // 예: ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse; // 필요에 따라 플래그 추가
    if (m_bShow)
    {
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
        static char mouse_cursor[128] = "Click on the grid area"; // 클릭 정보 표시용 문자열 추가
        static char click_info[128] = "Grid Ready";
        static char click_color[128] = "Click on the grid area"; // 클릭 정보 표시용 문자열 추가

        // --- 입력 처리 (줌, 패닝) ---
        const float zoom_speed = 0.1f;
        const float min_zoom = 0.01f;
        const float max_zoom = 10.0f;
        ImGuiIO& io = ImGui::GetIO();
        bool is_window_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
        if (g_bCameraControl)
        {
            //if (is_window_hovered) {
            //    // 줌 (마우스 휠)
            //    float wheel = io.MouseWheel;
            //    if (wheel != 0) {
            //        float scale_delta = wheel * zoom_speed * zoom;
            //        zoom += scale_delta;
            //        zoom = fmaxf(min_zoom, fminf(zoom, max_zoom));
            //    }
            //}
        }
        // --- 좌표 및 그리기 준비 ---
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        // 현재 ImGui 창의 내용 영역 계산 (ImGuizmo SetRect 및 2D 그리드 표시에 사용 가능)
        ImVec2 windowPos = ImGui::GetWindowPos();
        ImVec2 contentMinRel = ImGui::GetWindowContentRegionMin();
        ImVec2 contentMaxRel = ImGui::GetWindowContentRegionMax();
        float contentWidth = contentMaxRel.x - contentMinRel.x;
        float contentHeight = contentMaxRel.y - contentMinRel.y;

        ImVec2 canvas_p0 = ImVec2(windowPos.x + contentMinRel.x, windowPos.y + contentMinRel.y); // 내용 영역의 화면 절대 좌표 (좌상단)
        ImVec2 canvas_p1 = ImVec2(windowPos.x + contentMaxRel.x, windowPos.y + contentMaxRel.y);       // 내용 영역의 화면 절대 좌표 (우하단) 
        ImVec2 canvas_size = ImVec2(canvas_p1.x - canvas_p0.x, canvas_p1.y - canvas_p0.y);

        ImVec2 initial_center = ImVec2(canvas_p0.x + canvas_size.x * 0.5f, canvas_p0.y + canvas_size.y * 0.5f);
        ImVec2 canvas_center = ImVec2(initial_center.x + panning.x, initial_center.y + panning.y); // 패닝 적용된 중심
        if (g_bCameraControl)
            SetCursorPos(initial_center.x, initial_center.y);

        DrawTexture(draw_list, L"RT_Skybox", canvas_p0, canvas_size, canvas_center, zoom); 
        DrawGrid(canvas_p0, canvas_size);
        
        // --- 클릭 처리 및 좌표 변환 ---
        if (is_window_hovered) {
            ImVec2 mouse_pos_in_screen = io.MousePos;
            // 화면 좌표 -> 그리드 좌표 변환 (패닝된 중앙점과 줌 고려)
            ImVec2 mouse_pos_relative_to_center = ImVec2(mouse_pos_in_screen.x - canvas_center.x, mouse_pos_in_screen.y - canvas_center.y);
            ImVec2 mouse_pos_in_grid = ImVec2(mouse_pos_relative_to_center.x, mouse_pos_relative_to_center.y);
            
            if (!m_bLockPicked)
            {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    ImVec2 mouse_pos_in_image_area = ImVec2(mouse_pos_in_screen.x - m_vActualImageP0.x, mouse_pos_in_screen.y - m_vActualImageP0.y);
                    float normalized_x = mouse_pos_in_image_area.x / (m_vActualImageSize.x - m_vActualImageP0.x);
                    float normalized_y = mouse_pos_in_image_area.y / (m_vActualImageSize.y - m_vActualImageP0.y);

                    // 4. 원본 이미지 텍스처 좌표로 변환
                    // m_vImageGridSize는 원본 렌더 타겟의 크기 (예: g_iWinSizeX, g_iWinSizeY)
                    _uint texture_coord_x = static_cast<_uint>(normalized_x * m_vImageGridSize.x);
                    _uint texture_coord_y = static_cast<_uint>(normalized_y * m_vImageGridSize.y);
                    snprintf(click_info, sizeof(click_info), "Left Clicked at Grid(%.1f, %.1f)", mouse_pos_in_grid.x, mouse_pos_in_grid.y);
                    // 여기서 last_left_click_grid_pos 같은 변수에 저장하거나 다른 작업 수행 가능
                    
                    _float2 ratio = { io.DisplaySize.x / g_iWinSizeX, io.DisplaySize.y / g_iWinSizeY };
                    ImVec2 TexturePos = ImVec2(mouse_pos_in_grid.x + io.DisplaySize.x * 0.5f, mouse_pos_in_grid.y + io.DisplaySize.y * 0.5f);
                    snprintf(mouse_cursor, sizeof(mouse_cursor), "TexturePos(%d, %d)", texture_coord_x, texture_coord_y);
                    _float4 testColor = { 1.0f, 1.0f, 1.0f, 1.0f };
                    if (FAILED(m_pGameInstance->ReadPickingTargetPixel(texture_coord_x, texture_coord_y, &testColor)))
                        return E_FAIL;
                    snprintf(click_color, sizeof(click_color), "Left Clicked Color(%.1f, %.1f, %.1f, %.1f)", testColor.x, testColor.y, testColor.z, testColor.w);
                    m_pGameInstance->PickObjectByColor(&testColor);
                }
            } 
            else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                snprintf(click_info, sizeof(click_info), "Right Clicked at Grid(%.1f, %.1f)", mouse_pos_in_grid.x, mouse_pos_in_grid.y);
                // 여기서 last_right_click_grid_pos 같은 변수에 저장하거나 다른 작업 수행 가능
            }

            if (m_bLockPicked)
                ImGui::Text("Picked Object Locked");
        }
        //렌더링 결과물
#pragma region RectForTargetRatio
        // --- 그리드 및 경계선 그리기 ---
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
#pragma endregion

        

#pragma region Widgets
        ImGuiWindowFlags widget_window_flags = ImGuiWindowFlags_AlwaysAutoResize;
        if (ImGui::Begin("Widget Window", nullptr, widget_window_flags)) // p_open 인자를 nullptr로 전달하면 닫기 버튼이 생기지 않습니다. 필요하다면 bool 변수를 전달하세요.
        {
            ImGui::Text("Pan Offset: (%.1f, %.1f) (Right Mouse Click & Drag)", panning.x, panning.y);
            ImGui::TextUnformatted(mouse_cursor);
            ImGui::TextUnformatted(click_color);
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
        DrawTexture(draw_list, L"RT_ObjectsOnly", canvas_p0, canvas_size, canvas_center, zoom);
        DrawImGuizmo3D(zoom, panning);
        

        ImGui::End(); // 뷰포트 창 끝
    }
    return S_OK;
}


ImVec2 CViewport_3D::GridToScreen(const ImVec2& gridPos, const ImVec2& canvas_center, _float zoom)
{
    return ImVec2(canvas_center.x + gridPos.x * zoom, canvas_center.y +  gridPos.y * zoom); 
}

HRESULT CViewport_3D::DrawGrid(ImVec2& p0, ImVec2& size)
{
    if (size.x <= 1.0f || size.y <= 1.0f)
    {
        return S_OK;
    }
    ImGuizmo::SetRect(p0.x, p0.y, size.x, size.y);

    // 실제 3D 카메라의 뷰 및 투영 행렬 가져오기
    _matrix dxViewMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
    _matrix dxProjMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);

    float viewPtr[16], projPtr[16];
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(viewPtr), dxViewMatrix);
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(projPtr), dxProjMatrix);
    
    // 6. 그리드 그리기 (월드 원점에 고정)
    float identityMatrixFloat[16];  
    _matrix identityMatrix = XMMatrixIdentity(); // 단위 행렬 (월드 원점)
    XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(identityMatrixFloat), identityMatrix);
    float gridSize = 100.0f; // 원하는 그리드 크기
    ImGuizmo::SetOrthographic(false); // 원근 투영이므로 false
    ImGuizmo::SetDrawlist(); // 명시적 DrawList 설정
    ImGuizmo::DrawGrid(viewPtr, projPtr, identityMatrixFloat, gridSize);
    
    return S_OK;
}

HRESULT CViewport_3D::DrawTexture(ImDrawList* pList, const _wstring& strRTKey, const ImVec2& p0, const ImVec2& canvas_size, const ImVec2& center, _float fZoom)
{
    m_pSRV = m_pGameInstance->Get_SRV(strRTKey);
    if (!m_bShowImage || !m_pSRV)
        return S_OK;

    if (canvas_size.x <= 0.f || canvas_size.y <= 0.f) // 그릴 영역이 없으면 반환
        return S_OK;

    // 1. 원본 렌더 타겟의 크기 가져오기
    // m_vImageGridSize는 Initialize에서 설정된 원본 렌더 타겟의 크기 (예: 1920x1080)
    _float rtWidth = m_vImageGridSize.x;
    _float rtHeight = m_vImageGridSize.y;

    if (rtWidth <= 0.f || rtHeight <= 0.f) // 유효하지 않은 렌더 타겟 크기면 전체를 사용하지 않음
        return S_OK;

    // 2. 화면에 그릴 최종 영역 계산 (종횡비 유지를 위한 레터박스/필러박스 적용)
    ImVec2 imageScreenMin = p0;
    ImVec2 imageScreenMax = ImVec2(p0.x + canvas_size.x, p0.y + canvas_size.y);

    _float rtAspect = rtWidth / rtHeight;
    _float vpAspect = canvas_size.x / canvas_size.y;

    if (vpAspect > rtAspect) // 뷰포트가 렌더타겟보다 가로로 넓음 (렌더타겟은 상대적으로 세로가 김) -> 필러박스 (좌우 검은 바)
    {
        // 높이는 뷰포트에 꽉 채움, 너비는 렌더타겟 종횡비에 맞춰 계산
        _float scaledWidth = canvas_size.y * rtAspect;
        _float offsetX = (canvas_size.x - scaledWidth) * 0.5f;
        imageScreenMin.x = p0.x + offsetX;
        imageScreenMax.x = p0.x + offsetX + scaledWidth;
        // imageScreenMin.y 와 imageScreenMax.y 는 canvas_p0 와 canvas_size 에 의해 이미 설정됨
    }
    else if (vpAspect < rtAspect) // 뷰포트가 렌더타겟보다 세로로 넓음 (렌더타겟은 상대적으로 가로가 김) -> 레터박스 (상하 검은 바)
    {
        // 너비는 뷰포트에 꽉 채움, 높이는 렌더타겟 종횡비에 맞춰 계산
        _float scaledHeight = canvas_size.x / rtAspect;
        _float offsetY = (canvas_size.y - scaledHeight) * 0.5f;
        imageScreenMin.y = p0.y + offsetY;
        imageScreenMax.y = p0.y + offsetY + scaledHeight;
        // imageScreenMin.x 와 imageScreenMax.x 는 canvas_p0 와 canvas_size 에 의해 이미 설정됨
    }
    // else (vpAspect == rtAspect) -> imageScreenMin/Max 는 canvas_p0 와 canvas_size 전체를 사용 (변경 없음)


    // 3. UV 좌표는 렌더 타겟 전체를 사용
    ImVec2 uv0 = ImVec2(0.f, 0.f);
    ImVec2 uv1 = ImVec2(1.f, 1.f);
    m_vActualImageP0 = { imageScreenMin.x, imageScreenMin.y };
    m_vActualImageSize = { imageScreenMax.x, imageScreenMax.y };

    // 4. ImDrawList::AddImage() 호출
    pList->AddImage(
        (ImTextureID)m_pSRV,
        imageScreenMin, // 계산된 화면 좌상단 (레터박스/필러박스 적용)
        imageScreenMax, // 계산된 화면 우하단 (레터박스/필러박스 적용)
        uv0,            // UV는 전체 사용
        uv1,            // UV는 전체 사용
        IM_COL32_WHITE
    );

    return S_OK;
}

HRESULT CViewport_3D::DrawImGuizmo3D(_float fZoom, ImVec2& vPanning)
{
    CGameObject* pGameObject = m_pGameInstance->Get_PickedObject();
    if (pGameObject)
    {
        CTransform* pTransform = static_cast<CTransform*>(pGameObject->Get_Component(g_strTransformTag));
        if (pTransform)
        {
            // 2. 실제 카메라 행렬 가져오기 (GameInstance 사용)
            _matrix viewMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
            _matrix projMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);
            // View: 줌과 패닝을 적용 (카메라가 줌 아웃하고 패닝하는 효과)
            // 스케일 중심은 (0,0), 패닝은 화면 좌표 기준
            _matrix scaleMatrix = XMMatrixScaling(fZoom, fZoom, 1.f);
            // 패닝: 카메라가 -panning 만큼 이동하는 것과 동일
            _matrix panMatrix = XMMatrixTranslation(vPanning.x / fZoom, -vPanning.y / fZoom, 0.f);
            // 최종 View 행렬: 이동 -> 스케일 순서 적용
            //_matrix viewMatrix = panMatrix * scaleMatrix;

            // 3. ImGuizmo 설정
            ImGuizmo::SetOrthographic(false); // 원근 투영이므로 false
            ImGuizmo::SetDrawlist();

            // 현재 "3D Viewport" 창의 내용 영역을 가져옵니다.
            // DrawTexture 함수 이후에 호출되므로, GetItemRectMin/Max 대신
            // 현재 창의 ContentRegion을 사용합니다.
            ImVec2 windowPos = ImGui::GetWindowPos();
            ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
            ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
            float viewManipulateLeft = windowPos.x + contentMin.x;
            float viewManipulateTop = windowPos.y + contentMin.y;
            float contentWidth = contentMax.x - contentMin.x;
            float contentHeight = contentMax.y - contentMin.y;

            // ImGuizmo의 렌더링 영역을 현재 창의 내용 영역으로 설정합니다.
            ImGuizmo::SetRect(viewManipulateLeft, viewManipulateTop, contentWidth, contentHeight);
            // 유효한 영역인지 확인 후 설정
            if (contentWidth > 1.0f && contentHeight > 1.0f)
            {
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
            }
            else
            {
                int a = 10;
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