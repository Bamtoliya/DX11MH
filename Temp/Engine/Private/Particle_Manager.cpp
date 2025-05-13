#include "Particle_Manager.h"

CParticle_Manager::CParticle_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CParticle_Manager::Add_Particle_System(CParticle_System* pParticle_System)
{
	m_Particle_Systems.push_back(pParticle_System);
	return S_OK;
}

HRESULT CParticle_Manager::Remove_Dead_Particle_System()
{
	if (!m_Particle_Systems.empty())
	{
		m_Particle_Systems.erase(
			std::remove_if(m_Particle_Systems.begin(), m_Particle_Systems.end(),
				[](CParticle_System* ps) { return !ps->IsDead(); }),
			m_Particle_Systems.end());
	}
	return S_OK;
}

void CParticle_Manager::Update(float fTimeDelta)
{
	for (auto& ps : m_Particle_Systems)
	{
		ps->Update(fTimeDelta);
	}
	Remove_Dead_Particle_System();
}

void CParticle_Manager::Render()
{
	for (auto& ps : m_Particle_Systems)
	{
		ps->Render();
	}
}

CParticle_Manager* CParticle_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return new CParticle_Manager(pDevice, pContext);
}

void CParticle_Manager::Free()
{
	__super::Free();

	for (auto& ps : m_Particle_Systems)
	{
		Safe_Release(ps);
	}

	m_Particle_Systems.clear();
}
