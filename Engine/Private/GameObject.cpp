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
	data["Visible"] = m_bVisible;
	for(auto & iter : m_Components)
	{
		if(FAILED(iter.second->Serialize(data)))
			return E_FAIL;
	}
	return S_OK;
}


HRESULT CGameObject::Deserialize(simdjson::ondemand::object& jsonData)
{
	jsonData["Visible"].get_bool().get(m_bVisible);
	for (auto& iter : m_Components)
	{
		if (FAILED(iter.second->Deserialize(jsonData)))
			return E_FAIL;
	}
	return S_OK;
}