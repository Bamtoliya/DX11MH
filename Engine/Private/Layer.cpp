#include "Layer.h"
#include "GameObject.h"
#include "GameInstance.h"

CLayer::CLayer()
{
}

CComponent* CLayer::Get_Component(const _wstring& strComponentTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	
	return (*iter)->Get_Component(strComponentTag);
}

HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

CGameObject* CLayer::Get_GameObjectById(_uint iId)
{
	CGameObject* pGameObject = nullptr;
	auto iter = find_if(m_GameObjects.begin(), m_GameObjects.end(),
		[iId](CGameObject* pGameObject) {
			// CGameObject에 ID를 가져오는 함수가 있다고 가정합니다.
			// 예를 들어 Get_ID() 또는 유사한 이름의 함수입니다.
			// 실제 프로젝트의 CGameObject 클래스에 맞게 수정해주세요.
			return pGameObject != nullptr && pGameObject->Get_ID() == iId;
		});

	if (iter != m_GameObjects.end())
	{
		pGameObject = *iter;
	}

	return pGameObject; // 해당 ID를 가진 게임 오브젝트를 찾지 못한 경우 nullptr을 반환합니다.
}

void CLayer::Priority_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Priority_Update(fTimeDelta);

	}
		
}

void CLayer::Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Update(fTimeDelta);

	}
}

void CLayer::Late_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Late_Update(fTimeDelta);

	}
}

HRESULT CLayer::RenderImGui(const wstring& strLayerTag)
{
	ImGui::PushID(strLayerTag.c_str());
	if ((ImGui::Checkbox("##Visible", &m_bVisible)))
	{
		ToggleVisible();
	}
	ImGui::SameLine();
	if (ImGui::CollapsingHeader(WideToUtf8(strLayerTag.c_str()).c_str()))
	{
		for (auto& pGameObject : m_GameObjects)
		{
			if(!pGameObject->Get_Parent())
				pGameObject->RenderImGuiObjects();
		}
	}
	ImGui::PopID();
	return S_OK;
}

HRESULT CLayer::Render_ImGui_ComboBox(_uint& iGameObjectID)
{

	ImGuiComboFlags flags = ImGuiComboFlags_HeightRegular;

	// 콤보 박스의 미리보기 값(닫혀 있을 때 표시되는 텍스트)을 결정합니다.
	std::string preview_value_utf8 = "(None)"; // 기본적으로 "선택 없음"
	if (iGameObjectID != 0) // 0이 "선택 없음"을 의미한다고 가정합니다.
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		if (pGameInstance)
		{
			CGameObject* pSelectedGameObject = pGameInstance->Get_GameObjectById(iGameObjectID);
			if (pSelectedGameObject)
			{
				// "ID | 이름" 형식으로 미리보기 텍스트를 설정합니다.
				preview_value_utf8 = std::to_string(pSelectedGameObject->Get_ID()) + " | " + WideToUtf8(pSelectedGameObject->Get_Name().c_str());
			}
			else
			{
				// ID가 설정되어 있지만 오브젝트를 찾을 수 없는 경우 (예: 삭제됨), 선택을 초기화합니다.
				iGameObjectID = 0;
				// preview_value_utf8는 "(None)"으로 유지됩니다.
			}
		}
	}

	if (ImGui::BeginCombo("Parent GameObject", preview_value_utf8.c_str(), flags))
	{
		static ImGuiTextFilter filter;

		// 콤보 박스가 처음 나타날 때 필터에 포커스를 주고 내용을 지웁니다.
		if (ImGui::IsWindowAppearing())
		{
			ImGui::SetKeyboardFocusHere();
			filter.Clear();
		}

		// 필터 입력 필드 (고유 ID 사용)
		filter.Draw("##ParentGameObjectFilterInput", ImGui::GetContentRegionAvail().x);
		ImGui::Separator();


		// "선택 없음" 옵션
		bool is_none_selected = (iGameObjectID == 0);
		if (ImGui::Selectable("(None)", is_none_selected))
		{
			iGameObjectID = 0; // 부모 ID를 "선택 없음"으로 설정
			ImGui::CloseCurrentPopup(); // 선택 후 콤보 박스 닫기
		}
		if (is_none_selected)
		{
			ImGui::SetItemDefaultFocus(); // 현재 상태가 "선택 없음"이면 기본 포커스로 설정
		}

		// 레이어 내의 게임 오브젝트들을 나열합니다.
		for (auto& pGameObject : m_GameObjects)
		{
			if (!pGameObject)
				continue;

			// 항목에 표시될 문자열: "ID | 이름"
			std::string game_object_display_str_utf8 = std::to_string(pGameObject->Get_ID()) + " | " + WideToUtf8(pGameObject->Get_Name().c_str());

			// 필터 적용
			if (filter.PassFilter(game_object_display_str_utf8.c_str()))
			{
				bool is_current_item_selected = (pGameObject->Get_ID() == iGameObjectID);
				ImGui::PushID(pGameObject->Get_ID()); // 각 선택 항목에 고유 ID 부여
				if (ImGui::Selectable(game_object_display_str_utf8.c_str(), is_current_item_selected))
				{
					iGameObjectID = pGameObject->Get_ID(); // 선택된 부모 ID 업데이트
					ImGui::CloseCurrentPopup(); // 선택 후 콤보 박스 닫기
				}
				if (is_current_item_selected)
				{
					ImGui::SetItemDefaultFocus(); // 이 항목이 선택된 경우, 목록에서 기본 포커스로 설정
				}
				ImGui::PopID();
			}
		}
		ImGui::EndCombo();
	}
	return S_OK;
	/*static _wstring selected_key = L"";
	static _wstring selected_key_with_ID = L"";
	ImGuiComboFlags flags = ImGuiComboFlags_HeightRegular;
	if (ImGui::BeginCombo("Parent GameObject", WideToUtf8(selected_key_with_ID.c_str()).c_str(), flags))
	{
		static ImGuiTextFilter filter;
		if (ImGui::IsWindowAppearing())
		{
			ImGui::SetKeyboardFocusHere();
			filter.Clear();\
		}
		ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
		filter.Draw("##Filter", -FLT_MIN);


		for (auto& pGameObject : m_GameObjects)
		{
			wstring szBuffer = pGameObject->Get_Name();
			const _bool is_selected = (selected_key == szBuffer);
			ImGui::PushID(pGameObject->Get_ID());
			selected_key_with_ID = to_wstring(pGameObject->Get_ID()) + L" | " + pGameObject->Get_Name();
			ImGui::Text("%d | ", pGameObject->Get_ID());
			ImGui::SameLine();
			if (filter.PassFilter(WideToUtf8(szBuffer.c_str()).c_str()))
				if (ImGui::Selectable(WideToUtf8(szBuffer.c_str()).c_str(), is_selected))
				{
					selected_key = szBuffer;
					iGameObjectID = pGameObject->Get_ID();
				}
			ImGui::PopID();
		}
		ImGui::EndCombo();
	}
	return S_OK;*/
}

HRESULT CLayer::ToggleVisible()
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Set_Visible(m_bVisible);

	}
	return S_OK;
}

CGameObject* CLayer::Find_GameObjectByName(const _wstring& strName)
{
	auto &iter = find_if(m_GameObjects.begin(), m_GameObjects.end(), [&strName](CGameObject* pGameObject) {
		pGameObject->Get_Name();
		return true;
		});
	return nullptr;
}

CLayer* CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	__super::Free();

	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);
	m_GameObjects.clear();
}
