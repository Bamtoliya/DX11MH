#include "Camera_Manager.h"
#include "Camera.h"
CCamera_Manager::CCamera_Manager()
{
}

CCamera_Manager* CCamera_Manager::Create()
{
	return new CCamera_Manager();
}

void CCamera_Manager::Free()
{
	__super::Free();
	for (auto& pCamera : m_Cameras)
	{
		Safe_Release(pCamera);
	}
	m_Cameras.clear();
}

void CCamera_Manager::Priority_Update(_float fTimeDelta)
{
	if (m_Cameras.empty() || m_iCurIndex >= m_Cameras.size())
	{
		return;
	}

	auto iter = m_Cameras.begin();
	std::advance(iter, m_iCurIndex);

	if (iter != m_Cameras.end() && (*iter) != nullptr)
	{
		(*iter)->Priority_Update(fTimeDelta);
	}
}

void CCamera_Manager::Update(_float fTimeDelta)
{
	if (m_Cameras.empty() || m_iCurIndex >= m_Cameras.size())
	{
		return;
	}

	auto iter = m_Cameras.begin();
	std::advance(iter, m_iCurIndex);

	if (iter != m_Cameras.end() && (*iter) != nullptr)
	{
		(*iter)->Update(fTimeDelta);
	}
}

void CCamera_Manager::Late_Update(_float fTimeDelta)
{
	if (m_Cameras.empty() || m_iCurIndex >= m_Cameras.size())
	{
		return;
	}

	auto iter = m_Cameras.begin();
	std::advance(iter, m_iCurIndex);

	if (iter != m_Cameras.end() && (*iter) != nullptr)
	{
		(*iter)->Late_Update(fTimeDelta);
	}
}

HRESULT CCamera_Manager::Render()
{
	if (m_Cameras.empty() || m_iCurIndex >= m_Cameras.size())
	{
		return S_OK;
	}

	auto iter = m_Cameras.begin();
	std::advance(iter, m_iCurIndex);

	if (iter != m_Cameras.end() && (*iter) != nullptr)
	{
		(*iter)->Render();
	}
	return S_OK;
}

void CCamera_Manager::Add_Camera(CCamera* pCamera)
{
	auto iter = find_if(m_Cameras.begin(), m_Cameras.end(), [&pCamera](CCamera* camera)-> bool {
		if (camera == pCamera)
			return true;
		return false;
		});

	if (iter == m_Cameras.end())
	{
		m_Cameras.emplace_back(pCamera);
		Safe_AddRef(pCamera);
	}
}

void CCamera_Manager::Remove_Camera(CCamera* pCamera)
{
	if (nullptr == pCamera || m_Cameras.empty())
		return;

	_uint numeric_idx_to_remove = 0;
	auto iter_to_remove = m_Cameras.begin();
	bool bFound = false;

	for (; iter_to_remove != m_Cameras.end(); ++iter_to_remove, ++numeric_idx_to_remove)
	{
		if (*iter_to_remove == pCamera)
		{
			bFound = true;
			break;
		}
	}

	if (!bFound)
		return;

	Safe_Release(*iter_to_remove);
	m_Cameras.erase(iter_to_remove);

	if (m_Cameras.empty())
	{
		m_iCurIndex = 0;
	}
	else
	{
		if (numeric_idx_to_remove < m_iCurIndex)
		{
			m_iCurIndex--;
		}
		// m_iCurIndex가 제거 후 리스트의 유효한 범위를 벗어나지 않도록 조정
		m_iCurIndex = std::min(m_iCurIndex, static_cast<_uint>(m_Cameras.size() - 1));
	}

	// 활성 카메라 상태 업데이트
	InActiveCameras(); // 모든 카메라 비활성화
	if (!m_Cameras.empty())
	{
		auto current_iter = m_Cameras.begin();
		std::advance(current_iter, m_iCurIndex);
		if (current_iter != m_Cameras.end() && (*current_iter) != nullptr)
		{
			(*current_iter)->Set_Active(true);
		}
	}
}

void CCamera_Manager::Remove_Camera(_uint iIndex)
{
	if (iIndex >= m_Cameras.size() || m_Cameras.empty())
		return;

	auto iter_to_remove = m_Cameras.begin();
	std::advance(iter_to_remove, iIndex);

	Safe_Release(*iter_to_remove);
	m_Cameras.erase(iter_to_remove);

	if (m_Cameras.empty())
	{
		m_iCurIndex = 0;
	}
	else
	{
		if (iIndex < m_iCurIndex)
		{
			m_iCurIndex--;
		}
		m_iCurIndex = std::min(m_iCurIndex, static_cast<_uint>(m_Cameras.size() - 1));
	}

	// 활성 카메라 상태 업데이트
	InActiveCameras(); // 모든 카메라 비활성화
	if (!m_Cameras.empty())
	{
		auto current_iter = m_Cameras.begin();
		std::advance(current_iter, m_iCurIndex);
		if (current_iter != m_Cameras.end() && (*current_iter) != nullptr)
		{
			(*current_iter)->Set_Active(true);
		}
	}
}

void CCamera_Manager::Set_Camera(CCamera* pCamera)
{
	auto iter = std::find_if(m_Cameras.begin(), m_Cameras.end(),
		[&pCamera](const CCamera* camera) {
			return camera == pCamera;
		});

	if (iter != m_Cameras.end())
	{
		m_iCurIndex = static_cast<_uint>(std::distance(m_Cameras.begin(), iter));

		InActiveCameras();

		if ((*iter) != nullptr)
		{
			(*iter)->Set_Active(true);
		}
	}
}

void CCamera_Manager::Set_Camera(_uint iIndex)
{
	// 인덱스가 유효한 범위 내에 있는지 확인합니다.
	if (iIndex >= m_Cameras.size() || m_Cameras.empty())
	{
		return; // 유효하지 않으면 함수 종료
	}

	// 현재 카메라 인덱스를 설정합니다.
	m_iCurIndex = iIndex;

	// 모든 카메라를 비활성화합니다.
	InActiveCameras();

	// 새 현재 카메라를 활성화합니다.
	// m_Cameras는 std::list이므로 반복자를 사용해야 합니다.
	auto iter = m_Cameras.begin();
	std::advance(iter, m_iCurIndex); // m_iCurIndex만큼 반복자 이동

	if (iter != m_Cameras.end() && (*iter) != nullptr)
	{
		(*iter)->Set_Active(true);
	}
}

void CCamera_Manager::Change_Camera(CCamera* pCamera)
{
	auto iter = std::find_if(m_Cameras.begin(), m_Cameras.end(),
		[&pCamera](const CCamera* camera) {
			return camera == pCamera;
		});

	if (iter != m_Cameras.end())
	{
		m_iCurIndex = static_cast<_uint>(std::distance(m_Cameras.begin(), iter));

		InActiveCameras();

		if ((*iter) != nullptr)
		{
			(*iter)->Set_Active(true);
		}
	}
}

void CCamera_Manager::Change_Camera(_uint iIndex)
{
	// 인덱스가 유효한 범위 내에 있는지 확인합니다.
	if (iIndex >= m_Cameras.size() || m_Cameras.empty())
	{
		return; // 유효하지 않으면 함수 종료
	}

	// 현재 카메라 인덱스를 설정합니다.
	m_iCurIndex = iIndex;

	// 모든 카메라를 비활성화합니다.
	InActiveCameras();

	// 새 현재 카메라를 활성화합니다.
	// m_Cameras는 std::list이므로 반복자를 사용해야 합니다.
	auto iter = m_Cameras.begin();
	std::advance(iter, m_iCurIndex); // m_iCurIndex만큼 반복자 이동

	if (iter != m_Cameras.end() && (*iter) != nullptr)
	{
		(*iter)->Set_Active(true);
	}
}


void CCamera_Manager::InActiveCameras()
{
	// 모든 카메라를 우선 비활성화합니다.
	for (auto& pCam : m_Cameras)
	{
		if (pCam != nullptr)
		{
			pCam->Set_Active(false);
		}
	}
}

void CCamera_Manager::Clear()
{
	// 모든 카메라를 우선 비활성화합니다.
	for (auto& pCam : m_Cameras)
	{
		if (pCam != nullptr)
		{
			Safe_Release(pCam);
		}
	}
	m_Cameras.clear();
}

