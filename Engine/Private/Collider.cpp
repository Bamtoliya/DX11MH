
#include "Collider.h"
#include "GameInstance.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CCollider::CCollider(const CCollider& Prototype)
	: CComponent{ Prototype }
{
}

HRESULT CCollider::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CCollider::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	return S_OK;
}
HRESULT CCollider::Late_Initialize()
{
	return S_OK;
}

CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCollider* pInstance = new CCollider(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CCollider* CCollider::Clone(void* pArg)
{
	CCollider* pInstance = new CCollider(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CCollider::Free()
{
	__super::Free();
}

HRESULT CCollider::Serialize(json& data)
{
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	return S_OK;
}
HRESULT CCollider::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

#pragma region Loops
void CCollider::Priority_Update(_float fTimeDelta)
{
}
void CCollider::Update(_float fTimeDelta)
{
}
void CCollider::Late_Update(_float fTimeDelta)
{
}
HRESULT CCollider::Render()
{
	return S_OK;
}
#pragma endregion
HRESULT CCollider::RenderImGui()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Collider", ImGuiTreeNodeFlags_DefaultOpen))
	{

	}
	ImGui::PopID();
	return S_OK;
}
