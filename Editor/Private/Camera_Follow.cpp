#include "Camera_Follow.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Follow_Camera.h"

#pragma region Constructor/Destructor
CCamera_Follow::CCamera_Follow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject_Camera{ pDevice, pContext }
{

}

CCamera_Follow::CCamera_Follow(const CCamera_Follow& Prototype)
	: CGameObject_Camera{ Prototype }
	, m_fSensor{ Prototype.m_fSensor } // m_fSensor 복사
{

}

CCamera_Follow* CCamera_Follow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Follow* pInstance = new CCamera_Follow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Follow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Follow::Clone(void* pArg)
{
	CCamera_Follow* pInstance = new CCamera_Follow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Follow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Follow::Clone(simdjson::ondemand::object& jsonData)
{
	CCamera_Follow* pInstance = new CCamera_Follow(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Follow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CCamera_Follow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Follow::Initialize(void* pArg)
{
	CGameObject_Camera::CAMERA_DESC Desc{};
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = 1.f;
	lstrcpy(Desc.szName, TEXT("Camera"));
	m_fSensor = 0.1f;
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (m_pCameraCom) // m_pCameraCom은 CGameObject_Camera의 멤버일 것으로 가정
	{
		// 카메라 매니저에서 기존 카메라 제거 (필요한 경우)
		m_pGameInstance->Remove_Camera(m_pCameraCom); 

		// 컴포넌트 맵에서 제거
		auto iter = m_Components.find(g_strCameraTag);
		if (iter != m_Components.end())
		{
			Safe_Release(iter->second); // 기존 컴포넌트 참조 해제
			m_Components.erase(iter);
		}
		Safe_Release(m_pCameraCom); // CGameObject_Camera의 m_pCameraCom 멤버 해제
		m_pCameraCom = nullptr;
	}

	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(&Desc);

	CCamera::CAMERA_COM_DESC* cameraDesc = static_cast<CCamera::CAMERA_COM_DESC*>(pDesc);

	// 2. 새로운 CFollow_Camera 컴포넌트 생성 및 초기화
	CFollow_Camera* pFollowCamera = CFollow_Camera::Create(m_pDevice, m_pContext /*, 필요한 인자 */);
	if (nullptr == pFollowCamera)
	{
		MSG_BOX("Failed to Create CFollow_Camera Component");
		return E_FAIL;
	}
	pFollowCamera->Set_OwnerGameObject(this);
	if (FAILED(pFollowCamera->Initialize(cameraDesc)))
	{
		MSG_BOX("Failed to Initialize CFollow_Camera Component");
		Safe_Release(pFollowCamera);
		return E_FAIL;
	}

	// 3. 새 컴포넌트를 게임 오브젝트에 등록
	m_pCameraCom = pFollowCamera; // CGameObject_Camera의 m_pCameraCom을 새 카메라로 설정
	m_Components.emplace(g_strCameraTag, m_pCameraCom);
	Safe_AddRef(m_pCameraCom); // 새 컴포넌트 참조 추가

	// 4. 카메라 매니저에 새 카메라 등록 (필요한 경우)
	m_pGameInstance->Add_Camera(m_pCameraCom);
	if (m_pCameraCom)
	{
	     m_pCameraCom->Late_Initialize(); // CCamera의 Late_Initialize 호출 (필요시)
	}


	// 중요: 위 CFollow_Camera 관련 주석 처리된 부분은 실제 CFollow_Camera 클래스의
	//      구현에 맞춰 수정해야 합니다. 현재는 개념적인 코드입니다.
	//      만약 CCamera_Follow 자체가 새로운 카메라 로직을 직접 포함하고,
	//      CCamera 컴포넌트를 완전히 대체하는 것이 아니라 CCamera의 기능을 확장/변경하는 것이라면
	//      접근 방식이 달라집니다.
	//      현재 CCamera_Follow는 CGameObject_Camera를 상속하고 있으므로,
	//      CGameObject_Camera가 생성하는 CCamera 컴포넌트(m_pCameraCom)를
	//      새로운 타입의 카메라 컴포넌트로 교체하는 로직이 필요합니다.
	//      만약 CFollow_Camera라는 별도의 컴포넌트 클래스가 없다면,
	//      CGameObject_Camera::Initialize에서 생성되는 CCamera 객체의 동작을
	//      CCamera_Follow 클래스 내에서 오버라이드하거나 수정해야 합니다.

	// 현재 코드에서는 CGameObject_Camera::Initialize가 CCamera를 생성하고 m_pCameraCom에 할당합니다.
	// CCamera_Follow의 목적이 이 CCamera의 동작을 변경하는 것이라면,
	// m_pCameraCom을 다른 타입으로 교체하거나, m_pCameraCom의 설정을 변경해야 합니다.
	// 만약 "CFollow_Camera"가 CCamera를 상속한 새로운 클래스라면,
	// CGameObject_Camera::Initialize 내부에서 CCamera 대신 CFollow_Camera를 생성하도록 수정하거나,
	// 여기서 교체하는 로직을 구현해야 합니다.

	// 임시로, CGameObject_Camera가 생성한 CCamera를 그대로 사용한다고 가정하고,
	// 해당 카메라의 설정을 CCamera_Follow에 맞게 조정할 수 있습니다.
	// 예를 들어, m_pCameraCom->Set_FollowTarget(...) 같은 메서드가 있다면 호출합니다.

	return S_OK;
}

HRESULT CCamera_Follow::Initialize(simdjson::ondemand::object& jsonData)
{
	CGameObject_Camera::CAMERA_DESC Desc{};
	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = 1.f;
	lstrcpy(Desc.szName, TEXT("Camera"));
	m_fSensor = 0.1f;
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if (m_pCameraCom) // m_pCameraCom은 CGameObject_Camera의 멤버일 것으로 가정
	{
		// 카메라 매니저에서 기존 카메라 제거 (필요한 경우)
		m_pGameInstance->Remove_Camera(m_pCameraCom);

		// 컴포넌트 맵에서 제거
		auto iter = m_Components.find(g_strCameraTag);
		if (iter != m_Components.end())
		{
			Safe_Release(iter->second); // 기존 컴포넌트 참조 해제
			m_Components.erase(iter);
		}
		Safe_Release(m_pCameraCom); // CGameObject_Camera의 m_pCameraCom 멤버 해제
		m_pCameraCom = nullptr;
	}

	CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(&Desc);

	CCamera::CAMERA_COM_DESC* cameraDesc = static_cast<CCamera::CAMERA_COM_DESC*>(pDesc);

	// 2. 새로운 CFollow_Camera 컴포넌트 생성 및 초기화
	CFollow_Camera* pFollowCamera = CFollow_Camera::Create(m_pDevice, m_pContext /*, 필요한 인자 */);
	if (nullptr == pFollowCamera)
	{
		MSG_BOX("Failed to Create CFollow_Camera Component");
		return E_FAIL;
	}
	pFollowCamera->Set_OwnerGameObject(this);
	if (FAILED(pFollowCamera->Initialize(cameraDesc)))
	{
		MSG_BOX("Failed to Initialize CFollow_Camera Component");
		Safe_Release(pFollowCamera);
		return E_FAIL;
	}

	// 3. 새 컴포넌트를 게임 오브젝트에 등록
	m_pCameraCom = pFollowCamera; // CGameObject_Camera의 m_pCameraCom을 새 카메라로 설정
	m_Components.emplace(g_strCameraTag, m_pCameraCom);
	Safe_AddRef(m_pCameraCom); // 새 컴포넌트 참조 추가

	// 4. 카메라 매니저에 새 카메라 등록 (필요한 경우)
	m_pGameInstance->Add_Camera(m_pCameraCom);
	if (m_pCameraCom)
	{
		m_pCameraCom->Late_Initialize(); // CCamera의 Late_Initialize 호출 (필요시)
	}

	if (FAILED(Deserialize(jsonData)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Follow::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region Loops
void CCamera_Follow::Priority_Update(_float fTimeDelta)
{
	if (KEY_PRESSING(DIK_RIGHTARROW))
		static_cast<CFollow_Camera*>(m_pCameraCom)->AddYaw(-fTimeDelta * m_fSensor);

	if (KEY_PRESSING(DIK_LEFTARROW))
		static_cast<CFollow_Camera*>(m_pCameraCom)->AddYaw(fTimeDelta * m_fSensor);

	if (KEY_PRESSING(DIK_UPARROW))
		static_cast<CFollow_Camera*>(m_pCameraCom)->AddPitch(-fTimeDelta * m_fSensor);

	if (KEY_PRESSING(DIK_DOWNARROW))
		static_cast<CFollow_Camera*>(m_pCameraCom)->AddPitch(fTimeDelta * m_fSensor);


	m_pCameraCom->Priority_Update(fTimeDelta);
}

void CCamera_Follow::Update(_float fTimeDelta)
{
	m_pCameraCom->Update(fTimeDelta);
}

void CCamera_Follow::Late_Update(_float fTimeDelta)
{
	m_pCameraCom->Late_Update(fTimeDelta);
}

HRESULT CCamera_Follow::Render()
{
	return S_OK;
}
#pragma endregion

#pragma region Save&Load

HRESULT CCamera_Follow::Serialize(json& outJson)
{
	json data;
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;
	outJson["CCamera_Follow"] = data;
	outJson["CCamera_Follow"]["PrototypeTag"] = "Prototype_GameObject_Camera_Follow";
	return S_OK;
}

HRESULT CCamera_Follow::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;
	return S_OK;
}

HRESULT CCamera_Follow::Prototype_Serialize(json& outJson)
{
	outJson["Class"] = "CCamera_Follow";
	return S_OK;
}

HRESULT CCamera_Follow::Prototype_Deserialize(simdjson::ondemand::object& jsonData)
{
	return S_OK;
}

#pragma endregion