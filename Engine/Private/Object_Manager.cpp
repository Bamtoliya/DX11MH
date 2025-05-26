#include "Object_Manager.h"
#include "Layer.h"

#include "GameInstance.h"
#include "GameObject.h"

_uint CObject_Manager::m_oID = { 1 };

CObject_Manager::CObject_Manager()
	: m_pGameInstance { CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

CComponent* CObject_Manager::Get_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	CLayer*		pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_Component(strComponentTag, iIndex);
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	m_pLayers = new map<const _wstring, CLayer*>[iNumLevels];

	m_iNumLevels = iNumLevels;

    return S_OK;
}

HRESULT CObject_Manager::Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	/* 사본객체를 오브젝트 매니져에 추가한다. */
	/* 원형 매니져에게 복제하여 내놔!! */
	CGameObject*		pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pGameObject)
		return E_FAIL;

	//pGameObject->Set_ID(m_oID++);
	pGameObject->Set_LayerTag(strLayerTag);
	CLayer*		pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();

		pLayer->Add_GameObject(pGameObject);

		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, simdjson::ondemand::object& jsonData)
{
	/* 사본객체를 오브젝트 매니져에 추가한다. */
	/* 원형 매니져에게 복제하여 내놔!! */
	CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, jsonData));
	if (nullptr == pGameObject)
		return E_FAIL;

	//pGameObject->Set_ID(m_oID++);
	pGameObject->Set_LayerTag(strLayerTag);
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();

		pLayer->Add_GameObject(pGameObject);

		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, _uint iGameObjectID, void* pArg)
{
	/* 사본객체를 오브젝트 매니져에 추가한다. */
	/* 원형 매니져에게 복제하여 내놔!! */

	CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, m_pGameInstance->Get_GameObjectById(iGameObjectID), pArg));
	if (nullptr == pGameObject)
		return E_FAIL;

	//pGameObject->Set_ID(m_oID++);
	pGameObject->Set_LayerTag(strLayerTag);
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();

		pLayer->Add_GameObject(pGameObject);

		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLevelIndex, const _wstring& strLayerTag, CGameObject* parentGameObject, simdjson::ondemand::object& jsonData)
{
	CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, parentGameObject, jsonData));
	if (nullptr == pGameObject)
		return E_FAIL;

	//pGameObject->Set_ID(m_oID++);
	pGameObject->Set_LayerTag(strLayerTag);
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();

		pLayer->Add_GameObject(pGameObject);

		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

void CObject_Manager::Priority_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Priority_Update(fTimeDelta);
	}
}

void CObject_Manager::Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Update(fTimeDelta);
	}
}

void CObject_Manager::Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Late_Update(fTimeDelta);
	}
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return;

	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();
}

CGameObject* CObject_Manager::Get_GameObjectById(_uint iId)
{
	CGameObject* pGameObject = { nullptr };
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			pGameObject = Pair.second->Get_GameObjectById(iId);
			if (pGameObject != nullptr)
				return pGameObject;
		}
	}
	return nullptr;
}

CGameObject* CObject_Manager::Find_GameObjectByName(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strGameObjectName)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;
	CGameObject* pGameObject = pLayer->Find_GameObjectByName(strGameObjectName);
	if (nullptr == pGameObject)
		return nullptr;
	
	return pGameObject;
}

HRESULT CObject_Manager::Move_GameObjectToLayer(_uint iObjectID, _uint iTargetLevelIndex, const _wstring& strTargetLayerTag)
{
	if (iTargetLevelIndex >= m_iNumLevels)
		return E_FAIL;

	CGameObject* pGameObject = Get_GameObjectById(iObjectID);
	if (nullptr == pGameObject)
		return E_FAIL;

	// 1. 현재 오브젝트가 속한 레벨과 레이어에서 오브젝트 제거
	//    (GameObject가 자신의 레벨/레이어 정보를 가지고 있다면 더 효율적으로 찾을 수 있습니다.)
	//    여기서는 모든 레벨과 레이어를 순회하여 오브젝트를 찾는다고 가정합니다.
	//    또는, GameObject가 자신의 현재 레벨 인덱스를 안다면 해당 레벨만 검색합니다.
	//    CGameInstance를 통해 현재 레벨 인덱스를 가져올 수 있습니다.
	_uint currentLevelOfObject = m_pGameInstance->Get_CurrentLevelIndex(); // 예시: 오브젝트가 현재 레벨에 있다고 가정
	// 또는 오브젝트가 자신의 레벨 정보를 가지고 있어야 함.

// 기존 레이어에서 오브젝트 제거 (Safe_Release 없이 포인터만 리스트에서 제거)
// 이 작업은 RemoveGameObjectFromLayer 헬퍼 함수를 통해 수행
	if (FAILED(Remove_GameObjectFromLayer(currentLevelOfObject, pGameObject)))
	{
		// 만약 오브젝트가 어떤 레이어에도 속하지 않은 상태라면 (예: 방금 프로토타입에서 복제만 된 상태)
		// 이 부분은 실패할 수 있지만, 그래도 새 레이어에 추가하는 것은 계속 진행할 수 있습니다.
		// MSG_BOX("Failed to remove object from its original layer, or it was not in any layer.");
	}


	// 2. 대상 레이어 찾기 또는 생성
	CLayer* pTargetLayer = Find_Layer(iTargetLevelIndex, strTargetLayerTag);
	if (nullptr == pTargetLayer)
	{
		pTargetLayer = CLayer::Create();
		if (nullptr == pTargetLayer)
			return E_FAIL;
		m_pLayers[iTargetLevelIndex].emplace(strTargetLayerTag, pTargetLayer);
	}

	// 3. 대상 레이어에 게임 오브젝트 추가
	if (FAILED(pTargetLayer->Add_GameObject(pGameObject)))
		return E_FAIL;

	// 4. 게임 오브젝트의 부모를 nullptr로 설정 (최상위 오브젝트로 만듦)
	pGameObject->Set_Parent(nullptr);

	// (선택 사항) GameObject가 자신의 레이어 태그를 저장한다면 업데이트
	// pGameObject->SetLayerTag(strTargetLayerTag); 

	return S_OK;
}

HRESULT CObject_Manager::Remove_GameObjectFromLayer(_uint iLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject)
{
	if (iLevelIndex >= m_iNumLevels || nullptr == pGameObject)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return E_FAIL;

	if (pLayer->Remove_GameObjectByObject(pGameObject))
		return S_OK;
	return E_FAIL; // 레이어에 해당 오브젝트가 없음
}

HRESULT CObject_Manager::Remove_GameObjectFromLayer(_uint iLevelIndex, CGameObject* pGameObject)
{
	if (iLevelIndex >= m_iNumLevels || nullptr == pGameObject)
		return E_FAIL;

	// 모든 레이어를 순회하며 pGameObject를 찾아서 제거
	for (auto& layerPair : m_pLayers[iLevelIndex])
	{
		CLayer* pLayer = layerPair.second;
		if (pLayer)
		{
			pLayer->Remove_GameObjectByObject(pGameObject);
		}
	}
	return S_OK;
}

HRESULT CObject_Manager::RenderImGui()
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->RenderImGui(Pair.first);
		}
	}
	return S_OK;
}

HRESULT CObject_Manager::ToggleLayerVisible(_uint iLevelIndex, const _wstring& strLayerTag)
{
	auto	iter = m_pLayers[iLevelIndex].find(strLayerTag);
	if (iter == m_pLayers[iLevelIndex].end())
		return (*iter).second->ToggleVisible();
	return S_OK;
}

static int MyCallback(ImGuiInputTextCallbackData* data)
{
	if (data->EventFlag == ImGuiInputTextFlags_CallbackCompletion)
	{
		data->InsertChars(data->CursorPos, "Layer_");
	}
	return 0;
}

HRESULT CObject_Manager::Render_ImGuiLayerList(_uint iLevelIndex, _char* szLayerBuffer, _uint& iGameObjectID, _uint iBufferSize)
{
	static _wstring selected_key = L""; // 레이어 선택용
	ImGuiComboFlags flags = ImGuiComboFlags_HeightRegular;
	if (ImGui::BeginCombo("Layer", WideToUtf8(selected_key.c_str()).c_str(), flags))
	{
		static ImGuiTextFilter filter;
		if (ImGui::IsWindowAppearing())
		{
			ImGui::SetKeyboardFocusHere();
			filter.Clear();
		}
		ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
		filter.Draw("##LayerFilter", -FLT_MIN);


		for (auto& Pair : m_pLayers[iLevelIndex])
		{
			const _bool is_selected = (selected_key == Pair.first);
			if (filter.PassFilter(WideToUtf8(Pair.first.c_str()).c_str()))
				if (ImGui::Selectable(WideToUtf8(Pair.first.c_str()).c_str(), is_selected))
				{
					selected_key = Pair.first;
					strncpy_s(szLayerBuffer, iBufferSize, WideToUtf8(selected_key.c_str()).c_str(), _TRUNCATE);
				}
		}
		ImGui::EndCombo();
	}

	if (ImGui::InputTextWithHint("##Layer_Text_Input: ", "Layer_", szLayerBuffer, iBufferSize, ImGuiInputTextFlags_CallbackCompletion, MyCallback))
	{

	}

	auto iter = m_pLayers[iLevelIndex].find(Utf8ToWide(szLayerBuffer));
	if (iter != m_pLayers[iLevelIndex].end())
	{
		static char temp[MAX_PATH] = {}; // 선택된 부모 게임 오브젝트의 이름을 표시하는 버퍼
		(*iter).second->Render_ImGui_ComboBox(iGameObjectID); // 이 함수는 iGameObjectID를 업데이트합니다.

		CGameObject* pGameObject = nullptr;
		if (iGameObjectID != 0) // 유효한 부모 ID가 선택되었는지 확인 (0은 "선택 없음"으로 가정)
		{
			pGameObject = m_pGameInstance->Get_GameObjectById(iGameObjectID);
		}

		if (pGameObject) // 부모가 성공적으로 찾아졌다면
		{
			strncpy_s(temp, iBufferSize, WideToUtf8(pGameObject->Get_Name().c_str()).c_str(), _TRUNCATE);
		}
		else // 부모가 선택되지 않았거나 찾을 수 없다면
		{
			temp[0] = '\0'; // 표시되는 이름을 지웁니다.
		}

		// 선택된 부모 게임 오브젝트의 이름을 표시 (읽기 전용)
		if (ImGui::InputTextWithHint("##GameObject_Text_Input: ", "Parent GameObject", temp, iBufferSize, ImGuiInputTextFlags_ReadOnly))
		{
			// 읽기 전용이므로 이 블록은 일반적으로 실행되지 않음
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear Parent##ParentClearButton")) // 버튼 ID에 접미사 추가
		{
			iGameObjectID = 0; // 부모 ID 선택 해제
			temp[0] = '\0';    // 표시 버퍼 지우기
		}
	}

	return S_OK;
}

HRESULT CObject_Manager::Save_GameObjects(json& outJson)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			json layer = json::object();
			Pair.second->Serialize(layer);
			outJson[WideToUtf8(Pair.first.c_str()).c_str()] = layer;
		}
	}
	
	return S_OK;
}

HRESULT CObject_Manager::Save_GameObjects(_uint iLevelIndex, json& outJson)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
	{
		json layer = json::object();
		Pair.second->Serialize(layer);
		outJson[WideToUtf8(Pair.first.c_str()).c_str()] = layer;
	}
	return S_OK;
}

CLayer* CObject_Manager::Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	auto	iter = m_pLayers[iLevelIndex].find(strLayerTag);
	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

CObject_Manager* CObject_Manager::Create(_uint iNumLevels)
{
	CObject_Manager* pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CObject_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CObject_Manager::Free()
{
    __super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);
		m_pLayers[i].clear();
	}

	Safe_Delete_Array(m_pLayers);

	Safe_Release(m_pGameInstance);
}
