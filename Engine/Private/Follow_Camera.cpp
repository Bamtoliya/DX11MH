
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

	// 타겟 오브젝트 이름 저장
	if (m_pTarget != nullptr)
	{
		data["TargetName"] = WideToUtf8(m_pTarget->Get_Name().c_str());
	}

	outJson["CFollow_Camera"] = data;
	return S_OK;
}
HRESULT CFollow_Camera::Deserialize(simdjson::ondemand::object& jsonData)
{
	// CFollow_Camera 전용 데이터 추출
	simdjson::ondemand::object cameraData;
	simdjson::error_code error = jsonData["CFollow_Camera"].get_object().get(cameraData);
	if (error != simdjson::SUCCESS) {
		MSG_BOX("Failed to get CFollow_Camera data from JSON");
		return E_FAIL;
	}

	// 부모 클래스의 Deserialize 호출 (CCamera 데이터 전달)
	if (FAILED(__super::Deserialize(cameraData)))
		return E_FAIL;

	// CFollow_Camera 자체 필드 파싱
	double distance, smoothFactor, cameraYaw, cameraPitch;

	if (cameraData["Distance"].get_double().get(distance) == simdjson::SUCCESS)
		m_fDistance = static_cast<_float>(distance);

	if (cameraData["SmoothFactor"].get_double().get(smoothFactor) == simdjson::SUCCESS)
		m_fSmoothFactor = static_cast<_float>(smoothFactor);

	if (cameraData["CameraYaw"].get_double().get(cameraYaw) == simdjson::SUCCESS)
		m_fCameraYaw = static_cast<_float>(cameraYaw);

	if (cameraData["CameraPitch"].get_double().get(cameraPitch) == simdjson::SUCCESS)
		m_fCameraPitch = static_cast<_float>(cameraPitch);

	// 타겟 이름 파싱 및 저장
	std::string_view targetNameView;
	if (cameraData["TargetName"].get_string().get(targetNameView) == simdjson::SUCCESS)
	{
		m_strTargetName = Utf8ToWide(std::string(targetNameView));
		m_bFindTarget = true;
	}

	return S_OK;
}

#pragma region Loops
void CFollow_Camera::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}
void CFollow_Camera::Update(_float fTimeDelta)
{
	if (m_bFindTarget)
		Find_Target();
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

	CTransform* pCameraTransform = static_cast<CTransform*>(m_pOwner->Get_Component(g_strTransformTag));
	
	if (nullptr == pCameraTransform)
		return;
	CTransform* pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(g_strTransformTag));
	if (nullptr == pTargetTransform)
		return;
	_vector vTargetPosition = pTargetTransform->Get_State(STATE::POSITION);

	Follow(pCameraTransform, vTargetPosition, fTimeDelta);

	__super::Late_Update(fTimeDelta);
}
HRESULT CFollow_Camera::Render()
{
	return S_OK;
}

HRESULT CFollow_Camera::Find_Target()
{
	if (!m_bFindTarget || m_strTargetName.empty())
		return S_OK;

	if (m_pTarget != nullptr)
	{
		Safe_Release(m_pTarget);
		m_pTarget = nullptr;
	}

	CGameObject* pGameObject = m_pGameInstance->Get_GameObjectByName(m_strTargetName);
	if (nullptr != pGameObject)
	{
		Set_Target(pGameObject);
		m_bFindTarget = false;
	}
	return S_OK;
}

void CFollow_Camera::Follow(CTransform* pTransformCom, _vector vTargetPosition, _float fTimeDelta)
{

	// 1. 구면 좌표계로 카메라 방향 벡터 계산
	// Yaw는 수평 회전 (Y축 기준), Pitch는 수직 회전 (X축 기준)
	_float fSinPitch = sinf(m_fCameraPitch);
	_float fCosPitch = cosf(m_fCameraPitch);
	_float fSinYaw = sinf(m_fCameraYaw);
	_float fCosYaw = cosf(m_fCameraYaw);

	_vector vDirection = XMVectorSet(
		fCosPitch * fSinYaw,
		fSinPitch,
		fCosPitch * fCosYaw,
		0.0f
	);

	// 2. 방향 벡터에 거리를 곱하여 오프셋 계산
	_vector vOffset = XMVectorScale(vDirection, -m_fDistance); // 음수는 카메라가 타겟 뒤에 위치하게 함

	// 3. 타겟 위치에 오프셋을 더하여 카메라의 이상적 위치 계산
	_vector vIdealPosition = XMVectorAdd(vTargetPosition, vOffset);

	// 추가 높이 오프셋 (선택적, 타겟의 머리 위를 보게 하는 용도)
	// 이미 Pitch로 높이가 조절되므로 작은 값이나 0으로 설정 가능
	_float fExtraHeightOffset = 1.0f;
	vIdealPosition = XMVectorAdd(vIdealPosition, XMVectorSet(0.0f, fExtraHeightOffset, 0.0f, 0.0f));

	_vector vCurrentPosition = pTransformCom->Get_State(STATE::POSITION);
	_vector vNewPosition = XMVectorLerp(vCurrentPosition, vIdealPosition, m_fSmoothFactor * fTimeDelta);

	// 5. 카메라 위치 및 시선 방향 업데이트
	pTransformCom->Set_State(STATE::POSITION, vNewPosition);

	// 시선은 항상 타겟을 향하도록 (약간 위쪽을 보도록 오프셋 적용 가능)
	_vector vLookAt = XMVectorAdd(vTargetPosition, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	pTransformCom->LookAt(vLookAt);

	return;
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