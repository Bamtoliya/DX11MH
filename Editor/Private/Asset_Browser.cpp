#include "Asset_Browser.h"

namespace fs = std::filesystem;

CAsset_Browser::CAsset_Browser(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMyImGui{ pDevice, pContext }
{
}

CAsset_Browser* CAsset_Browser::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CAsset_Browser* pInstance = new CAsset_Browser(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Created : CAsset_Browser");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAsset_Browser::Free()
{
    __super::Free();
}

HRESULT CAsset_Browser::Initialize(void* pArg)
{
    m_bShow = true;
	return S_OK;
}

void CAsset_Browser::Update(_float fTimeDelta)
{
}

HRESULT CAsset_Browser::Render()
{
    if (m_bShow)
    {
        if (!ImGui::Begin("Asset Browser", &m_bShow)) {
            ImGui::End();
            return E_FAIL;
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
    return S_OK;
}
