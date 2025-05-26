#include "Controller.h"
#include "GameInstance.h"
#include "Transform.h"

CController::CController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent { pDevice, pContext }
{
}

CController::CController(const CController& Prototype)
	: CComponent{ Prototype }
{
}

HRESULT CController::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CController::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	return S_OK;
}
HRESULT CController::Late_Initialize()
{ 
	return S_OK;
}

CController* CController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CController* pInstance = new CController(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CController");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CComponent* CController::Clone(void* pArg)
{
	CController* pInstance = new CController(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CController");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CController::Free() 
{
	__super::Free();
}

HRESULT CController::Serialize(json& data)
{
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	return S_OK;
}
HRESULT CController::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

#pragma region Loops
void CController::Priority_Update(_float fTimeDelta)
{
}
void CController::Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;
	CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	if (KEY_DOWN(DIK_W))
	{
		pTransform->Go_Straight(fTimeDelta);
	}
	if (KEY_DOWN(DIK_A))
	{
		pTransform->Go_Left(fTimeDelta);
	}
	if (KEY_DOWN(DIK_S))
	{
		pTransform->Go_Backward(fTimeDelta);
	}
	if (KEY_DOWN(DIK_D))
	{
		pTransform->Go_Right(fTimeDelta);
	}
}
void CController::Late_Update(_float fTimeDelta)
{
}
HRESULT CController::Render()
{
	return S_OK;
}
#pragma endregion
HRESULT CController::RenderImGui()
{
	ImGui::PushID(this);
	if(ImGui::CollapsingHeader("Controller", ImGuiTreeNodeFlags_DefaultOpen))
	{

	}
	ImGui::PopID();
	return S_OK;
}