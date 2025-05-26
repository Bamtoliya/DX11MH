#include "Camera_Manager.h"
#include "Camera.h"
CCamera_Manager::CCamera_Manager()
{
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
	}
}

