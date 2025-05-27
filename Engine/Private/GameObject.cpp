#include "GameObject.h"
#include "GameInstance.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice 	{ pDevice }
	, m_pContext	{ pContext }
	, m_pGameInstance { CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

CGameObject::CGameObject(const CGameObject& Prototype)
	: m_pDevice 	{ Prototype.m_pDevice }
	, m_pContext 	{ Prototype.m_pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
	, m_oID { objID++ }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}

CComponent* CGameObject::Get_Component(const _wstring& strComponentTag)
{
	auto	iter = m_Components.find(strComponentTag);
	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void* pArg)
{

	if (nullptr == pArg)
		return S_OK;

	GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);
	lstrcpy(m_szName, pDesc->szName);

	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;	

	m_pTransformCom->Set_OwnerGameObject(this);

	if (FAILED(m_pTransformCom->Initialize(pArg)))
		return E_FAIL;

	m_Components.emplace(g_strTransformTag, m_pTransformCom);

	Safe_AddRef(m_pTransformCom);


	return S_OK;
}

HRESULT CGameObject::Late_Initialize(void* pArg)
{
	for (auto& pair : m_Components)
		pair.second->Late_Initialize();
	return S_OK;
}

HRESULT CGameObject::Initialize(simdjson::ondemand::object& jsonData)
{
	string temp;
	jsonData["Name"].get_string(temp);
	lstrcpy(m_szName, Utf8ToWide(temp).c_str());

	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	m_pTransformCom->Set_OwnerGameObject(this);

	if (FAILED(m_pTransformCom->Deserialize(jsonData)))
		return E_FAIL;

	m_Components.emplace(g_strTransformTag, m_pTransformCom);

	Safe_AddRef(m_pTransformCom);


	return S_OK;
}

void CGameObject::Free()
{
	__super::Free();

	Safe_Release(m_pParent);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pTransformCom);

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	for(auto& Pair:m_Childs)
		Safe_Release(Pair.second);
	m_Childs.clear();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

CGameObject* CGameObject::Clone(CGameObject* pGameObject, void* pArg)
{
	CGameObject* pInstance = Clone(pArg);
	if (pGameObject)
	{
		pInstance->m_pParent = pGameObject;
		Safe_AddRef(m_pParent);
		pInstance->m_pParent->Add_Child(pInstance->m_szName, pInstance);
	}
	return pInstance;
}

CGameObject* CGameObject::Clone(CGameObject* pGameObject, simdjson::ondemand::object& jsonData)
{
	CGameObject* pInstance = Clone(jsonData);
	if (pGameObject)
	{
		pInstance->m_pParent = pGameObject;
		Safe_AddRef(m_pParent);
		pInstance->m_pParent->Add_Child(pInstance->m_szName, pInstance);
	}
	return pInstance;
}

void CGameObject::Priority_Update(_float fTimeDelta)
{
	for (auto& iter : m_Components)
	{
		iter.second->Priority_Update(fTimeDelta);
	}
}

void CGameObject::Update(_float fTimeDelta)
{
	for (auto& iter : m_Components)
	{
		iter.second->Update(fTimeDelta);
	}
}

void CGameObject::Late_Update(_float fTimeDelta)
{
	for (auto& iter : m_Components)
	{
		iter.second->Late_Update(fTimeDelta);
	}
}

HRESULT CGameObject::Pre_Render()
{
	return S_OK;
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

HRESULT CGameObject::Post_Render()
{
	return S_OK;
}

HRESULT CGameObject::RenderImGuiComponents()
{
	for (auto& c : m_Components)
	{
		c.second->RenderImGui();
	}
	return S_OK;
}

HRESULT CGameObject::RenderImGuiObjects()
{
	ImGui::PushID(m_oID);
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	// static bool align_label_with_current_x_position = false; // 원본 코드에 있었으나, 현재 사용되지 않는 것으로 보임
	// static bool test_drag_and_drop = false; // 원본 코드에 있었으나, 현재 사용되지 않는 것으로 보임
	ImGuiTreeNodeFlags node_flags = base_flags; // base_flags로 초기화
	ImGui::Indent();
	if (m_bPicked)
		node_flags |= ImGuiTreeNodeFlags_Selected;

	// 체크박스와 트리 노드를 같은 줄에 배치하기 위한 처리
	// ImGui::AlignTextToFramePadding(); // 필요에 따라 사용

	ImGui::Checkbox("##Visible", &m_bVisible);
	ImGui::SameLine();

	bool node_open = ImGui::TreeNodeEx(WideToUtf8(m_szName).c_str(), node_flags);

	char popup_id[64];
	sprintf_s(popup_id, sizeof(popup_id), "GameObjectContextMenu_%u", m_oID);

	if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
	{
		ImGui::OpenPopup(popup_id);
	}

	if (ImGui::BeginPopup(popup_id))
	{
		if (ImGui::MenuItem("Delete"))
		{
			m_bToDestroy = true;
			ImGui::CloseCurrentPopup(); // 메뉴 아이템 클릭 후 팝업 닫기
		}
		
		ImGui::EndPopup();
	}

	// 드래그 소스 설정 (이 노드를 드래그할 수 있도록)
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		// 페이로드 설정 (여기서는 GameObject의 ID를 전달)
		ImGui::SetDragDropPayload("GAMEOBJECT_PAYLOAD", &m_oID, sizeof(_uint));
		ImGui::Text("%s", WideToUtf8(m_szName).c_str()); // 드래그 중 표시될 텍스트
		ImGui::EndDragDropSource();
	}

	// 드롭 대상 설정 (다른 노드를 이 노드 위나 아래, 또는 자식으로 드롭할 수 있도록)
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_PAYLOAD"))
		{
			IM_ASSERT(payload->DataSize == sizeof(_uint));
			_uint payload_oID = *(const _uint*)payload->Data;
			m_pGameInstance->Get_GameObjectById(payload_oID)->Set_Parent(this);
			// 여기에 드롭 로직 구현:
			// 1. payload_oID를 가진 GameObject를 찾습니다.
			// 2. 현재 노드(this)와 드롭된 노드(payload_oID)의 관계를 설정합니다.
			//    - 예: 드롭된 노드를 현재 노드의 자식으로 만들기
			//    - 예: 드롭된 노드를 현재 노드의 형제로 만들기 (순서 변경)
			// CGameInstance 또는 ObjectManager를 통해 GameObject를 가져오고 관계를 변경하는 함수 호출
			// 예시: m_pGameInstance->SetGameObjectParent(payload_oID, m_oID);
			//       m_pGameInstance->MoveGameObject(payload_oID, m_oID, targetRelation); // targetRelation은 자식, 이전 형제, 다음 형제 등
		}
		ImGui::EndDragDropTarget();
	}


	if (node_open)
	{
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			m_pGameInstance->PickObjectById(m_oID);

		for (auto& pair : m_Childs) // map 순회 시 pair 사용
		{
			if (pair.second) // 유효한 자식인지 확인
				pair.second->RenderImGuiObjects();
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
	ImGui::Unindent();

	return S_OK;
}

HRESULT CGameObject::Add_Child(const _wstring& strChildTag, CGameObject* pChild)
{
	if (nullptr == pChild)
		return E_FAIL;
	m_Childs.emplace(strChildTag, pChild);
	Safe_AddRef(pChild);
	return S_OK;
}

HRESULT CGameObject::Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg)
{
	CComponent*	pComponent = static_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_COMPONENT, iPrototypeLevelIndex, strPrototypeTag, this, pArg));
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(strComponentTag, pComponent);
	
	if(ppOut)
		*ppOut = pComponent;

	Safe_AddRef(pComponent);

	return S_OK;
}

HRESULT CGameObject::Serialize(json& data)
{
	data["Name"] = WideToUtf8(m_szName).c_str();
	data["Visible"] = m_bVisible;
	data["Active"] = m_bActive;
	data["LayerTag"] = WideToUtf8(m_strLayerTag.c_str());
	for(auto & iter : m_Components)
	{
		if(FAILED(iter.second->Serialize(data)))
			return E_FAIL;
	}
	for (auto& pChild : m_Childs)
	{
		json child;
		if(FAILED(pChild.second->Serialize(child)))
			return E_FAIL;
		data["Childs"].push_back(child);
	}
	
	return S_OK;
}

HRESULT CGameObject::Deserialize(simdjson::ondemand::object& jsonData)
{
	string_view name_sv;
	jsonData["Name"].get_string().get(name_sv);
	lstrcpy(m_szName, Utf8ToWide(string(name_sv)).c_str());
	jsonData["Visible"].get_bool().get(m_bVisible);
	for (auto& iter : m_Components)
	{
		if (FAILED(iter.second->Deserialize(jsonData)))
			return E_FAIL;
	}
	// 자식 객체들 역직렬화 (Childs가 배열이라고 가정)
	simdjson::ondemand::value childs_node_value;
	auto error = jsonData["Childs"].get(childs_node_value);

	if (error == simdjson::SUCCESS) {
		if (childs_node_value.type() == simdjson::ondemand::json_type::array) {
			simdjson::ondemand::array childs_array = childs_node_value.get_array();
			for (simdjson::ondemand::value child_item_value : childs_array) {
				if (child_item_value.type() == simdjson::ondemand::json_type::object) {
					simdjson::ondemand::object child_data_obj = child_item_value.get_object();

					// m_pGameInstance->Load_GameObject는 다음을 수행해야 합니다:
					// 1. child_data_obj에서 "PrototypeTag"를 읽어 프로토타입 복제
					// 2. 복제된 자식 객체의 Initialize(child_data_obj) 또는 Deserialize(child_data_obj) 호출
					// 3. 자식 객체의 부모를 현재 객체(m_oID)로 설정
					// 4. 자식 객체를 적절한 레벨/레이어에 추가
					if (FAILED(m_pGameInstance->Load_GameObject(
						m_pGameInstance->Get_CurrentLevelIndex(), // 또는 부모로부터 레벨 인덱스 전달
						this,                                  // 현재 객체의 ID (부모 ID로 사용)
						child_data_obj))) {
						return E_FAIL;
					}
				}
			}
		}
		else if (childs_node_value.type() == simdjson::ondemand::json_type::null) {
			// "Childs": null 이면, 자식 없음으로 처리
		}
		else {
			// "Childs" 필드가 있지만 배열도 아니고 null도 아닌 경우 (예상치 못한 형식)
			// MSG_BOX("Warning: Childs field is present but not an array or null.");
		}
	}
	else if (error == simdjson::NO_SUCH_FIELD) {
		// "Childs" 필드가 없는 경우, 자식 없음으로 처리
	}
	else {
		// "Childs" 필드 접근 중 다른 오류 발생
		return E_FAIL;
	}

	return S_OK;
}