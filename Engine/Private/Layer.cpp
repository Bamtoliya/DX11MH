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

HRESULT CLayer::Remove_GameObjectById(_uint iId)
{
	m_GameObjects.remove_if([&iId](CGameObject* pGameObject) {
		if (pGameObject->Get_ID() == iId)
			return true;
		return false;
		});

	return S_OK;
}

HRESULT CLayer::Remove_GameObjectByObject(CGameObject* pGameObject)
{
	m_GameObjects.remove_if([&pGameObject](CGameObject* pObject) {
		if (pObject == pGameObject)
			return true;
		return false;
		});

	return S_OK;
}

HRESULT CLayer::Remove_GameObjectByName(const _wstring& strObjectTag)
{
	m_GameObjects.remove_if([&strObjectTag](CGameObject* pGameObject) {
		if (pGameObject->Get_Name() == strObjectTag)
			return true;
		return false;
		});

	return S_OK;
}

void CLayer::Priority_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Priority_Update(fTimeDelta);
	}

	m_GameObjects.remove_if([](CGameObject* pGameObject) {
		if (pGameObject && pGameObject->Get_IsDestroy()) // Get_IsDestroy()가 있다고 가정
		{
			Safe_Release(pGameObject); // 객체 해제
			return true; // 리스트에서 제거 대상으로 표시
		}
		return false;
		});
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

	bool bHeaderOpen = ImGui::CollapsingHeader(WideToUtf8(strLayerTag.c_str()).c_str());

	// CollapsingHeader 아이템을 드롭 타겟으로 설정
	if (ImGui::BeginDragDropTarget()) // 바로 이전에 그려진 아이템(CollapsingHeader)을 타겟으로 함
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_PAYLOAD"))
		{
			IM_ASSERT(payload->DataSize == sizeof(_uint));
			_uint draggedObjectID = *(const _uint*)payload->Data;

			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			if (pGameInstance)
			{
				// CObject_Manager를 통해 오브젝트를 현재 레이어로 이동시킵니다.
				// 현재 CGameInstance에 CObject_Manager의 MoveGameObjectToLayer를 호출하는 인터페이스가 없다면,
				// CGameInstance에 해당 함수를 추가하거나, CObject_Manager의 포인터를 직접 가져와야 합니다.
				// 여기서는 CGameInstance에 해당 기능이 래핑되어 있다고 가정합니다.
				// 예: HRESULT hr = pGameInstance->MoveGameObjectToLayer(draggedObjectID, pGameInstance->Get_CurrentLevelIndex(), strLayerTag);

				// 임시로 CObject_Manager의 포인터를 CGameInstance에서 가져오는 방법 (GetObjectManager()가 있다고 가정)
				// CObject_Manager* pObjectManager = pGameInstance->GetObjectManager();
				// if(pObjectManager) {
				//    pObjectManager->MoveGameObjectToLayer(draggedObjectID, pGameInstance->Get_CurrentLevelIndex(), strLayerTag);
				// }
				// else { // 직접 CObject_Manager에 접근할 수 없다면, CGameInstance에 인터페이스 함수를 만들어야 합니다.
				//    MSG_BOX("Object Manager not accessible through GameInstance for moving GameObject.");
				// }

				// 가장 직접적인 방법은 CGameInstance에 Wrapper 함수를 만드는 것입니다.
				// GameInstance.h / .cpp 에 다음 함수를 추가했다고 가정:
				// HRESULT CGameInstance::MoveGameObjectToLayer(_uint iObjectID, _uint iTargetLevelIndex, const _wstring& strTargetLayerTag) {
				//     return m_pObject_Manager->MoveGameObjectToLayer(iObjectID, iTargetLevelIndex, strTargetLayerTag);
				// }
				HRESULT hr = pGameInstance->Move_GameObjectToLayer(draggedObjectID, pGameInstance->Get_CurrentLevelIndex(), strLayerTag);
				if (FAILED(hr))
				{
					MSG_BOX("Failed to move GameObject to new layer.");
				}
				// Set_Parent(nullptr)은 MoveGameObjectToLayer 내부에서 처리되거나,
				// MoveGameObjectToLayer 호출 후 pGameInstance->Get_GameObjectById(draggedObjectID)->Set_Parent(nullptr)를 호출할 수 있습니다.
				// 위 CObject_Manager::MoveGameObjectToLayer 구현에서는 Set_Parent(nullptr)가 이미 포함되어 있습니다.
			}
		}
		ImGui::EndDragDropTarget();
	}

	// ... (컨텍스트 메뉴 로직 및 기존 코드) ...

	if (bHeaderOpen) // CollapsingHeader가 열려 있을 때만 내부 게임 오브젝트들을 그립니다.
	{
		for (auto& pGameObject : m_GameObjects)
		{
			if (pGameObject && !pGameObject->Get_Parent()) // 부모가 없는 최상위 객체만 직접 그림
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

CGameObject* CLayer::Find_GameObjectById(_uint oId)
{
	auto& iter = find_if(m_GameObjects.begin(), m_GameObjects.end(), [&oId](CGameObject* pGameObject) {
		if(pGameObject->Get_ID() == oId)
			return true;
		return false;
		});
	if (iter != m_GameObjects.end())
		return *iter;
	return nullptr;
}

CGameObject* CLayer::Find_GameObjectByName(const _wstring& strName)
{
	auto &iter = find_if(m_GameObjects.begin(), m_GameObjects.end(), [&strName](CGameObject* pGameObject) {
		if (pGameObject->Get_Name() == strName)
			return true;
		return false;
		});
	if (iter != m_GameObjects.end())
		return *iter;
	return nullptr;
}

HRESULT CLayer::Serialize(json& outJson)
{
	for(auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
		{
			json gameObjectJson;
			if (pGameObject->Get_Parent() == nullptr)
			{
				pGameObject->Serialize(gameObjectJson);
				outJson["GameObjects"].push_back(gameObjectJson);
			}
		}
	}
	return S_OK;
}

HRESULT CLayer::Deserialize(simdjson::ondemand::object& jsonData)
{
	/*for (auto& gameObjectJson : jsonData["GameObjects"].get_array().value_or({}))
	{
		CGameObject* pGameObject = CGameObject::Clone(gameObjectJson);
		if (nullptr == pGameObject)
			return E_FAIL;
		m_GameObjects.push_back(pGameObject);
	}*/
	return S_OK;
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
