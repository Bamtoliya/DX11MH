#include "Layer.h"
#include "GameObject.h"

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
	static _wstring selected_key = L"";
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
