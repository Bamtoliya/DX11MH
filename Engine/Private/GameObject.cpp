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
	static bool align_label_with_current_x_position = false;	
	static bool test_drag_and_drop = false;
	ImGuiTreeNodeFlags node_flags = 0;
	ImGui::Indent();
	if (m_bPicked)
		node_flags |= ImGuiTreeNodeFlags_Selected;
	ImGui::Checkbox("##Visible", &m_bVisible);
	ImGui::SameLine();
	if (ImGui::TreeNodeEx(WideToUtf8(m_szName).c_str(), base_flags | node_flags))
	{
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			m_pGameInstance->PickObjectById(m_oID);
		ImGui::TreePop();
		for (auto& c : m_Childs)
		{
			c.second->RenderImGuiObjects();
		}
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