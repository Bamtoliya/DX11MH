#pragma once
#include "Particle_System.h"
BEGIN(Engine)
class CParticle_Manager : public CBase
{
private:
	CParticle_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CParticle_Manager() = default;
public:
	HRESULT Add_Particle_System(CParticle_System* pParticle_System);
	HRESULT Remove_Dead_Particle_System();
	void Update(float fTimeDelta);
	void Render();
private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;
	list<CParticle_System*>		m_Particle_Systems;
public:
	static CParticle_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
	
};
END