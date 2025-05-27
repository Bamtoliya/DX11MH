
#include "Follow_Camera.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Transform.h"

#pragma region Constructor/Destrurctor
CFollow_Camera::CFollow_Camera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CFollow_Camera::CFollow_Camera(const CFollow_Camera& Prototype)
	: CCamera{ Prototype }
	, m_pTarget{ Prototype.m_pTarget }
	, m_fDistance{ Prototype.m_fDistance }
	, m_fSmoothFactor{ Prototype.m_fSmoothFactor }
	, m_fCameraYaw{ Prototype.m_fCameraYaw }
	, m_fCameraPitch{ Prototype.m_fCameraPitch }
{
	if (nullptr != m_pTarget)
		Safe_AddRef(m_pTarget);
}

HRESULT CFollow_Camera::Initialize_Prototype()
{
	// 기본값 설정은 헤더에서 처리하거나 여기서 명시적으로 할 수 있습니다.
	m_fDistance = 10.f;
	m_fSmoothFactor = 0.2f; // 좀 더 부드러운 값을 기본으로 할 수도 있습니다.
	m_fCameraYaw = 0.f;
	m_fCameraPitch = XM_PIDIV4; // 약 45도 위에서 시작
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
	if (FAILED(__super::Late_Initialize()))
		return E_FAIL;
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
#pragma endregion

HRESULT CFollow_Camera::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	data["Distance"] = m_fDistance;
	data["SmoothFactor"] = m_fSmoothFactor;
	data["CameraYaw"] = m_fCameraYaw;
	data["CameraPitch"] = m_fCameraPitch;
	outJson["CFollow_Camera"] = data;
	return S_OK;
}
HRESULT CFollow_Camera::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	m_fDistance = static_cast<_float>(jsonData["Distance"].get_double().value_unsafe());
	m_fSmoothFactor = static_cast<_float>(jsonData["SmoothFactor"].get_double().value_unsafe());
	m_fCameraYaw = static_cast<_float>(jsonData["CameraYaw"].get_double().value_unsafe());
	m_fCameraPitch = static_cast<_float>(jsonData["CameraPitch"].get_double().value_unsafe());
	return S_OK;
}

#pragma region Loops
void CFollow_Camera::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}
void CFollow_Camera::Update(_float fTimeDelta)
{

}
void CFollow_Camera::Late_Update(_float fTimeDelta)
{
	if (nullptr == m_pOwner) // CComponent의 멤버, 이 카메라 컴포넌트를 소유한 게임 오브젝트
		return;

	if (nullptr == m_pTarget) // 따라갈 대상이 없으면 업데이트 안 함
	{
		// 대상이 없을 때 기본 동작 (예: 현재 위치 유지 또는 다른 로직)
		// CCamera의 Late_Update를 호출하여 뷰/투영 행렬만 업데이트 할 수 있습니다.
		__super::Late_Update(fTimeDelta);
		return;
	}

	CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(g_strTransformTag));
	if (nullptr == pTargetTransform)
		return;

	_vector vTargetPosition = pTargetTransform->Get_State(STATE::POSITION);

	// 1. 카메라 회전 행렬 생성 (Yaw, Pitch 기준)
	XMMATRIX matRotation = XMMatrixRotationRollPitchYaw(m_fCameraPitch, m_fCameraYaw, 0.f);

	// 2. 타겟으로부터의 기본 오프셋 벡터 (타겟 뒤쪽 Z축 방향)
	_vector vBaseOffset = XMVectorSet(0.f, 0.f, -m_fDistance, 0.f); // -Z 방향으로 m_fDistance 만큼

	// 3. 회전된 오프셋 벡터 계산
	// XMVector3TransformCoord는 w=1로 간주하므로 위치 변환에 적합합니다.
	// 방향 벡터의 회전에는 XMVector3TransformNormal이 더 적합할 수 있습니다.
	// 여기서는 오프셋 위치를 계산하므로 Coord도 괜찮습니다.
	_vector vRotatedOffset = XMVector3TransformCoord(vBaseOffset, matRotation);

	// 4. 최종 카메라 목표 위치 계산
	_vector vDesiredPosition = vTargetPosition + vRotatedOffset;

	// 5. 현재 카메라(소유자 게임 오브젝트)의 위치 가져오기
	CTransform* pCameraOwnerTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	if (nullptr == pCameraOwnerTransform)
		return;
	_vector vCurrentPosition = pCameraOwnerTransform->Get_State(STATE::POSITION);

	// 6. 선형 보간(Lerp)을 사용하여 카메라 위치를 부드럽게 이동
	// m_fSmoothFactor가 1에 가까우면 즉시 이동, 0에 가까우면 느리게 이동.
	// 프레임률에 독립적인 스무딩을 원하면: Lerp(current, desired, 1.f - exp(-Rate * fTimeDelta))
	// 현재는 단순 Lerp: factor가 클수록 빠르게 따라감.
	_vector vNewPosition = XMVectorLerp(vCurrentPosition, vDesiredPosition, m_fSmoothFactor /* * fTimeDelta - 사용 방식에 따라 결정 */);
	pCameraOwnerTransform->Set_State(STATE::POSITION, vNewPosition);

	// 7. CCamera의 LookAt 함수를 호출하여 카메라가 항상 타겟을 바라보도록 설정
	// CCamera::LookAt은 m_pOwner의 Transform에서 눈 위치(Eye)를 가져오고,
	// 제공된 타겟 위치(At)를 사용하여 뷰 행렬을 설정합니다.
	__super::LookAt(vTargetPosition);

	// 8. CCamera의 나머지 Late_Update 로직 (뷰/투영 행렬 업데이트 등) 호출
	// LookAt을 호출했으므로, __super::Late_Update(fTimeDelta)를 다시 호출할 필요는 없을 수 있습니다.
	// CCamera::LookAt 내부에서 필요한 뷰 행렬 업데이트가 수행되고,
	// CCamera::Late_Update에서 투영 행렬 및 기타 업데이트가 수행될 것입니다.
	// 만약 LookAt이 뷰 행렬만 설정하고, Late_Update에서 최종 행렬 계산을 한다면 호출 필요.
	// 현재 CCamera 구조에 따라 __super::Late_Update(fTimeDelta)를 호출하거나,
	// LookAt 이후 필요한 __super::Update_Matrix() 같은 함수를 호출할 수 있습니다.
	// 안전하게는 CCamera::Late_Update를 호출하여 모든 기본 업데이트가 수행되도록 합니다.
	// 단, LookAt을 호출한 후에는 Eye, At, Up이 이미 설정된 상태이므로,
	// CCamera::Late_Update가 이를 덮어쓰지 않도록 주의해야 합니다.
	// 가장 일반적인 방법은 CCamera::Set_ViewMatrix, CCamera::Set_ProjMatrix 등을 직접 호출하거나,
	// __super::Late_Update()가 내부적으로 m_matView, m_matProj를 업데이트하도록 하는 것입니다.
	// 여기서는 LookAt이 뷰 행렬을 설정하고, __super::Late_Update가 나머지를 처리한다고 가정합니다.
	// CCamera::Late_Update가 m_vEye, m_vAt 등을 기반으로 LookAt을 다시 호출한다면 중복이므로,
	// CCamera의 구현을 확인해야 합니다.
	// 여기서는 LookAt으로 뷰를 설정했으므로, __super::Late_Update()는 호출하지 않거나,
	// 뷰 행렬을 제외한 부분만 업데이트하도록 수정된 CCamera::Late_Update_Partially() 같은 함수를 가정합니다.
	// 가장 간단하게는, CCamera::Late_Update가 내부적으로 m_matView와 m_matProj를 업데이트한다고 가정하고 호출합니다.
	// 만약 CCamera::Late_Update가 Transform을 기반으로 Eye, At을 다시 계산한다면 문제가 됩니다.
	// 여기서는 __super::LookAt()이 뷰 행렬을 설정하고,
	// CCamera의 __super::Late_Update()는 주로 투영 행렬 업데이트 및 최종 파이프라인 상태 설정을 한다고 가정합니다.
	// CCamera::Late_Update()를 호출하여 파이프라인에 뷰/투영 행렬을 설정하도록 합니다.
	__super::Late_Update(fTimeDelta); // CCamera의 최종 업데이트 (행렬 파이프라인에 적용 등)
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

		// 카메라 회전 값 표시 및 수정 (라디안을 각도로 변환하여 표시)
		float fYawDeg = XMConvertToDegrees(m_fCameraYaw);
		float fPitchDeg = XMConvertToDegrees(m_fCameraPitch);
		if (ImGui::DragFloat("Yaw (Degrees)", &fYawDeg, 1.0f, -360.0f, 360.0f)) {
			m_fCameraYaw = XMConvertToRadians(fYawDeg);
		}
		if (ImGui::DragFloat("Pitch (Degrees)", &fPitchDeg, 1.0f, -89.0f, 89.0f)) { // Pitch 제한 예시
			m_fCameraPitch = XMConvertToRadians(fPitchDeg);
		}

		// 타겟 오브젝트 표시 및 드래그 앤 드롭 타겟 설정
		if (m_pTarget)
		{
			// CGameObject에 Get_Name() 또는 유사한 이름 반환 함수가 있다고 가정합니다.
			// 실제 프로젝트의 함수명으로 변경해야 할 수 있습니다.
			ImGui::Text("Target: %s", WideToUtf8(m_pTarget->Get_Name().c_str()).c_str());
		}
		else
		{
			ImGui::Text("Target: None");
		}

		if (ImGui::BeginDragDropTarget())
		{
			// "GAMEOBJECT_PAYLOAD"는 하이라키 뷰에서 드래그 소스를 설정할 때 사용한 페이로드 타입과 일치해야 합니다.
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_PAYLOAD"))
			{
				IM_ASSERT(payload->DataSize == sizeof(_uint));
				_uint payload_oID = *(const _uint*)payload->Data;
				CGameObject* pPayloadObject = m_pGameInstance->Get_GameObjectById(payload_oID);

				// 기존 타겟이 있다면 Release
				if (nullptr != m_pTarget)
				{
					Safe_Release(m_pTarget);
				}

				// 새로운 타겟 설정 및 AddRef
				m_pTarget = pPayloadObject;
				if (nullptr != m_pTarget)
				{
					Safe_AddRef(m_pTarget);
				}
			}
			ImGui::EndDragDropTarget();
		}

		if(ImGui::Button("Clear Target"))
		{
			if (nullptr != m_pTarget)
			{
				Safe_Release(m_pTarget);
				m_pTarget = nullptr;
			}
		}

		
	}
	ImGui::PopID();
	return S_OK;
}

void CFollow_Camera::AddYaw(_float fDelta)
{
	m_fCameraYaw += fDelta;
	// Yaw는 일반적으로 랩어라운드 됩니다 (예: -PI ~ PI 또는 0 ~ 2PI)
	// 여기서는 특별한 처리를 하지 않지만, 필요시 추가 가능
}

void CFollow_Camera::AddPitch(_float fDelta)
{
	m_fCameraPitch += fDelta;
	if (m_fCameraPitch < m_fMinPitch)
		m_fCameraPitch = m_fMinPitch;
	if (m_fCameraPitch > m_fMaxPitch)
		m_fCameraPitch = m_fMaxPitch;
}

void CFollow_Camera::AdjustDistance(_float fDeltaDistance)
{
	m_fDistance += fDeltaDistance;
	if (m_fDistance < 1.0f) m_fDistance = 1.0f; // 최소 거리 제한 예시
	if (m_fDistance > 50.f) m_fDistance = 50.f; // 최대 거리 제한 예시
}