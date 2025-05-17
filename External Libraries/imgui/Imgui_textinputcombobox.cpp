#pragma once    
#include <imgui.h>
#include <imgui_internal.h> // ImGui::InputTextEx 와 같은 내부 함수 접근이 필요할 수 있으나, 현재 코드는 직접 사용 X
#include <string>
#include <vector>   // std::pair 사용 시 필요할 수 있음 (현재 코드에서는 UserData로 const char** 와 size_t를 직접 캐스팅)
#include <cstring>  // strlen, strcpy_s 등
#include <utility>  // std::pair

namespace ImGui { // 또는 원하는 다른 네임스페이스 사용 (예: namespace MyCustomWidgets {)
// SFML 의존성 제거: 아래 줄 삭제
// extern sf::Keyboard::Key key;

    bool identical(const char* buf, const char* item) {
        if (!buf || !item) return false; // 널 포인터 체크
        size_t buf_size = strlen(buf);
        size_t item_size = strlen(item);
        if (buf_size == 0) return false; // 입력 버퍼가 비어있으면 제안하지 않음
        if (buf_size > item_size) return false; // 아이템 길이가 더 짧으면 부분 일치가 아님

        // strncmp를 사용하면 더 간결하게 표현 가능
        return strncmp(buf, item, buf_size) == 0;
        /*
        // 기존 로직:
        for (size_t i = 0; i < buf_size; ++i) {
            if (buf[i] != item[i]) return false;
        }
        return true;
        */
    }

    int propose(ImGuiInputTextCallbackData* data) {
        // 입력 버퍼가 비어있으면 "미리 선택"하지 않음
        if (data->BufTextLen == 0) return 0;

        // UserData에서 아이템 목록과 길이를 가져옴
        auto* M_userData = static_cast<std::pair<const char**, size_t>*>(data->UserData);
        const char** items = M_userData->first;
        size_t length = M_userData->second;

        // ImGuiInputTextFlags_CallbackEdit 플래그는 ImGui::InputTextEx 내부에서 사용자가 버퍼를 수정했을 때 설정됨
        // Backspace나 Delete 키로 인한 수정도 여기에 해당될 수 있음
        // 만약 data->EventKey를 사용하고 싶다면, InputText 호출 시 ImGuiInputTextFlags_CallbackCompletion,
        // ImGuiInputTextFlags_CallbackHistory, ImGuiInputTextFlags_CallbackCharFilter 중 하나 이상이 필요함.
        // ImGuiInputTextFlags_CallbackAlways는 모든 편집 시 콜백을 호출하지만 EventKey를 항상 채우지는 않음.

        // 여기서는 사용자가 Backspace/Delete를 눌러 텍스트를 수정하면,
        // 일단 ImGui가 해당 작업을 수행하도록 두고, 수정된 Buf를 기반으로 다시 제안을 시도함.
        // 공격적인 자동 완성을 피하기 위해, 사용자가 명시적으로 제안을 선택하도록 유도할 수도 있음.

        for (size_t i = 0; i < length; i++) {
            if (items[i] && data->Buf && identical(data->Buf, items[i])) { // 널 포인터 체크 추가
                // 현재 입력된 내용과 아이템의 앞부분이 일치하는 경우
                if (strcmp(data->Buf, items[i]) == 0) { // 이미 완전히 일치하면 추가 작업 불필요
                    data->SelectionStart = data->BufTextLen; // 선택 영역 제거
                    data->SelectionEnd = data->BufTextLen;
                    return 0;
                }

                const int cursor_pos = data->CursorPos; // 현재 커서 위치 저장
                const int old_len = data->BufTextLen;   // 이전 버퍼 길이 (현재 입력된 부분)

                // 기존 버퍼 내용을 삭제하고 매칭된 아이템으로 채움
                data->DeleteChars(0, old_len);
                data->InsertChars(0, items[i]);

                // 커서 위치를 사용자가 입력했던 부분의 끝으로 설정
                data->CursorPos = old_len; // 또는 cursor_pos (이전 커서 위치)

                // 나머지 제안된 부분을 선택 상태로 만듦
                data->SelectionStart = old_len; // 또는 cursor_pos
                data->SelectionEnd = strlen(items[i]);
                break; // 첫 번째 매칭되는 아이템으로 제안하고 종료
            }
        }
        return 0;
    }

    // std::string을 사용하는 오버로드
    bool TextInputComboBox(const char* id, std::string& str, size_t maxInputSize, const char* items[], size_t item_len, short showMaxItems) {
        if (str.length() > maxInputSize) { // 너무 길면 편집 불가 (표시만)
            ImGui::Text("%s", str.c_str()); // ImGui::Text는 포맷 문자열을 사용
            return false;
        }

        // std::string은 내부적으로 null을 포함할 수 있지만, ImGui InputText는 C-style 문자열로 다룸
        // 따라서 std::string의 내용을 char 배열로 복사해서 사용
        std::vector<char> buffer(maxInputSize + 1, 0); // null 종료 문자 공간 포함
        strncpy_s(buffer.data(), buffer.size(), str.c_str(), str.length());

        // char* 사용하는 메인 함수 호출
        // ImGui::TextInputComboBox는 현재 네임스페이스에 정의되어 있다고 가정
        bool changed = TextInputComboBox(id, buffer.data(), maxInputSize, items, item_len, showMaxItems);

        if (changed) {
            // 변경된 char 배열 내용을 다시 std::string으로 업데이트
            // buffer.data()는 null로 끝나는 것이 보장됨 (InputText 또는 strcpy_s에 의해)
            str = buffer.data();
        }
        return changed;
    }


    // char* 버퍼를 사용하는 메인 위젯 함수
    bool TextInputComboBox(const char* id, char* buffer, size_t maxInputSize, const char* items[], size_t item_len, short showMaxItems) {
        if (showMaxItems == 0 && item_len > 0) { // item_len > 0 조건 추가
            showMaxItems = static_cast<short>(item_len); // 모든 아이템 표시
        }
        else if (item_len == 0 && showMaxItems != 0) {
            showMaxItems = 0; // 아이템이 없으면 표시할 것도 없음
        }


        ImGui::PushID(id);
        std::pair<const char**, size_t> pass_data(items, item_len);

        // InputText 위젯. ImGuiInputTextFlags_CallbackAlways 플래그로 항상 propose 콜백 호출
        bool value_changed = ImGui::InputText("##in", buffer, maxInputSize, ImGuiInputTextFlags_CallbackAlways, propose, static_cast<void*>(&pass_data));

        // 팝업 관련 로직은 대부분 유지
        ImGui::OpenPopupOnItemClick("combobox", ImGuiPopupFlags_MouseButtonRight); // 오른쪽 클릭 시 팝업 (ImGuiPopupFlags 추가 가능)
        ImVec2 input_rect_min = ImGui::GetItemRectMin();
        ImVec2 input_rect_size = ImGui::GetItemRectSize();

        ImGui::SameLine(0, 0);
        if (ImGui::ArrowButton("##openCombo", ImGuiDir_Down)) {
            ImGui::OpenPopup("combobox");
        }
        // 중복된 OpenPopupOnItemClick 제거 가능 (만약 ArrowButton으로만 열고 싶다면)
        // 또는 InputText 영역 클릭 시에도 열리게 하려면 유지 (위의 오른쪽 클릭과 기능이 다름)
        // 만약 일반 클릭으로도 열고 싶다면:
        // if (ImGui::IsItemClicked()) ImGui::OpenPopup("combobox");

        ImVec2 popup_pos = input_rect_min;
        popup_pos.y += input_rect_size.y; // InputText 바로 아래에 위치

        // 팝업 크기 동적 계산 개선
        float popup_width = input_rect_size.x + ImGui::GetItemRectSize().x; // InputText 너비 + 화살표 버튼 너비
        if (ImGui::GetStyle().WindowPadding.x * 2 < popup_width) { // 최소 너비 보장
            popup_width += ImGui::GetStyle().WindowPadding.x * 2;
        }

        ImGui::SetNextWindowPos(popup_pos);
        ImGui::SetNextWindowSizeConstraints(ImVec2(popup_width, 0), ImVec2(popup_width, ImGui::GetTextLineHeightWithSpacing() * (showMaxItems + 2))); // +2 for header/separator

        if (ImGui::BeginPopup("combobox", ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) { // ImGuiWindowFlags_NoResize 추가 고려
            ImGui::Text("항목을 선택하거나 입력하세요.");
            ImGui::Separator();
            for (size_t i = 0; i < item_len; i++) {
                if (items[i]) { // 널 포인터 아이템 방지
                    if (ImGui::Selectable(items[i], false)) { // 'selected' 상태는 InputText와 동기화하기 어려움
                        // 아이템 선택 시 버퍼 내용 변경
                        strncpy_s(buffer, maxInputSize, items[i], _TRUNCATE); // maxInputSize 대신 실제 복사 길이 제한 가능
                        // buffer[maxInputSize -1] = '\0'; // strncpy_s는 알아서 null 종료하나, _TRUNCATE시 확인 필요
                        value_changed = true; // 선택으로 인해 값이 변경되었음을 알림
                        ImGui::CloseCurrentPopup(); // 선택 후 팝업 닫기
                    }
                }
            }
            ImGui::EndPopup();
        }
        ImGui::PopID();

        ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::TextUnformatted(id); // 레이블 표시 (ImGui::Text는 포맷 문자열로 해석하므로 TextUnformatted가 더 안전)

        return value_changed;
    }
} // 네임스페이스 끝 (사용했다면)