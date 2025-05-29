#include "Sonic_Controller.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Animator.h"
#include "Sonic.h"

CSonic_Controller::CSonic_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CSonic_Controller::CSonic_Controller(const CSonic_Controller& Prototype)
	: CComponent{ Prototype }
{
}

HRESULT CSonic_Controller::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CSonic_Controller::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	return S_OK;
}
HRESULT CSonic_Controller::Late_Initialize()
{
	return S_OK;
}

CSonic_Controller* CSonic_Controller::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSonic_Controller* pInstance = new CSonic_Controller(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSonic_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CComponent* CSonic_Controller::Clone(void* pArg)
{
	CSonic_Controller* pInstance = new CSonic_Controller(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CSonic_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CSonic_Controller::Free()
{
	__super::Free();
}

HRESULT CSonic_Controller::Serialize(json& data)
{
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	return S_OK;
}
HRESULT CSonic_Controller::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

#pragma region Loops
void CSonic_Controller::Priority_Update(_float fTimeDelta)
{
}
void CSonic_Controller::Update(_float fTimeDelta)
{
	if (!m_bActive)
		return;

	CAnimator* pAnimator = static_cast<CAnimator*>(m_pOwner->Get_Component(L"Com_Animator"));
	if (nullptr == pAnimator)
		return;
	CTransform* pTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	if (nullptr == pTransform)
		return;

	// 1. 카메라의 뷰 행렬 가져오기
	_matrix camWorldMatrix = m_pGameInstance->Get_Transform_MatrixInverse(D3DTS::VIEW);

	// 3. 카메라의 월드 공간 기준 Forward 및 Right 벡터 추출
	_vector camLookWorld = camWorldMatrix.r[2]; // 카메라의 Z축 (바라보는 방향)
	_vector camRightWorld = camWorldMatrix.r[0]; // 카메라의 X축 (오른쪽 방향)

	// 4. 카메라 방향 벡터를 XZ 평면에 투영하고 정규화 (수평 이동을 위해 Y성분 제거)
	_vector camLookXZ = XMVectorSet(XMVectorGetX(camLookWorld), 0.f, XMVectorGetZ(camLookWorld), 0.f);
	camLookXZ = XMVector3Normalize(camLookXZ);

	_vector camRightXZ = XMVectorSet(XMVectorGetX(camRightWorld), 0.f, XMVectorGetZ(camRightWorld), 0.f);
	camRightXZ = XMVector3Normalize(camRightXZ);
	// 또는 camRightXZ = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), camLookXZ); 로 계산 가능

	_vector vMovementDirection = XMVectorZero();
	_bool bIsMoving = false;

	if (KEY_PRESSING(DIK_W))
	{
		vMovementDirection = XMVectorAdd(vMovementDirection, camLookXZ);
		bIsMoving = true;
	}
	if (KEY_PRESSING(DIK_S))
	{
		vMovementDirection = XMVectorAdd(vMovementDirection, -camLookXZ);
		bIsMoving = true;
	}
	if (KEY_PRESSING(DIK_A))
	{
		vMovementDirection = XMVectorAdd(vMovementDirection, -camRightXZ);
		bIsMoving = true;
	}
	if (KEY_PRESSING(DIK_D))
	{
		vMovementDirection = XMVectorAdd(vMovementDirection, camRightXZ);
		bIsMoving = true;
	}

	if (bIsMoving)
	{
		vMovementDirection = XMVector3Normalize(vMovementDirection); // 최종 이동 방향 정규화

		// 캐릭터가 이동할 방향으로 회전
		_float fTargetAngleRadians = atan2f(XMVectorGetX(vMovementDirection), XMVectorGetZ(vMovementDirection));
		pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTargetAngleRadians);

		// 캐릭터 이동
		pTransform->Go_Straight(fTimeDelta); // Go_Straight는 현재 Transform의 Look 방향으로 이동
		pAnimator->Set_NextState(ENUM_CLASS(CSonic::STATE::RUN_LOOP), true);
	}
	else
	{
		pAnimator->Set_NextState(ENUM_CLASS(CSonic::STATE::IDLE_ACT00), true);
	}
}
void CSonic_Controller::Late_Update(_float fTimeDelta)
{
}
HRESULT CSonic_Controller::Render()
{
	return S_OK;
}
#pragma endregion
HRESULT CSonic_Controller::RenderImGui()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Sonic Controller", ImGuiTreeNodeFlags_DefaultOpen))
	{

	}
	ImGui::PopID();
	return S_OK;
}