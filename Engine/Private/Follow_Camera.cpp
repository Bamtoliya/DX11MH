
#include "Follow_Camera.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Transform.h"

CFollow_Camera::CFollow_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CFollow_Camera::CFollow_Camera(const CFollow_Camera& Prototype)
	: CCamera{ Prototype }
	, m_pTarget { Prototype.m_pTarget }
	, m_fDistance { Prototype.m_fDistance }
	, m_fSmoothFactor { Prototype.m_fSmoothFactor }
{
	if (nullptr != m_pTarget)
		Safe_AddRef(m_pTarget);
}

HRESULT CFollow_Camera::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CFollow_Camera::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	return S_OK;
}
HRESULT CFollow_Camera::Late_Initialize()
{
	return S_OK;
}

CFollow_Camera* CFollow_Camera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFollow_Camera* pInstance = new CFollow_Camera(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFollow_Camera");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CFollow_Camera* CFollow_Camera::Clone(void* pArg)
{
	CFollow_Camera* pInstance = new CFollow_Camera(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CFollow_Camera");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CFollow_Camera::Free()
{
	__super::Free();
	Safe_Release(m_pTarget);
}

HRESULT CFollow_Camera::Serialize(json& data)
{
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	data["Distance"] = m_fDistance;
	data["SmoothFactor"] = m_fSmoothFactor;
	return S_OK;
}
HRESULT CFollow_Camera::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	m_fDistance = static_cast<_float>(jsonData["Distance"].get_double().value_unsafe());
	m_fSmoothFactor = static_cast<_float>(jsonData["SmoothFactor"].get_double().value_unsafe());
	return S_OK;
}

#pragma region Loops
void CFollow_Camera::Priority_Update(_float fTimeDelta)
{
}
void CFollow_Camera::Update(_float fTimeDelta)
{
}
void CFollow_Camera::Late_Update(_float fTimeDelta)
{
	if (nullptr == m_pTarget)
		return;

	CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(g_strTransformTag));
	if (nullptr == pTargetTransform)
		return;

	_vector vTargetPosition = pTargetTransform->Get_State(STATE::POSITION);
	_vector vTargetLook = pTargetTransform->Get_State(STATE::LOOK); // 타겟의 바라보는 방향

	// 카메라의 목표 위치 계산 (타겟 뒤쪽, m_fDistance 만큼 떨어진 위치)
	_vector vDesiredPosition = vTargetPosition - XMVector3Normalize(vTargetLook) * m_fDistance;

	// 현재 카메라 위치 가져오기
	CTransform* pCameraTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	if (nullptr == pCameraTransform)
		return;
	_vector vCurrentPosition = pCameraTransform->Get_State(STATE::POSITION);

	// 선형 보간을 사용하여 카메라 위치를 부드럽게 이동
	_vector vNewPosition = XMVectorLerp(vCurrentPosition, vDesiredPosition, m_fSmoothFactor);
	pCameraTransform->Set_State(STATE::POSITION, vNewPosition);

	__super::LookAt(vTargetPosition);
}
HRESULT CFollow_Camera::Render()
{
	return S_OK;
}
#pragma endregion
HRESULT CFollow_Camera::RenderImGui()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Follow_Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		//ImGui::InputInt("Target Object ID")
		ImGui::InputFloat("Distance", &m_fDistance, 1.0f, 10.0f, "%.2f");
		ImGui::InputFloat("SmoothFactor", &m_fSmoothFactor, 0.01f, 0.1f, "%.3f");
		if (m_fSmoothFactor < 0.01f) m_fSmoothFactor = 0.01f;
		if (m_fSmoothFactor > 1.f) m_fSmoothFactor = 1.f;
	}
	ImGui::PopID();
	return S_OK;
}