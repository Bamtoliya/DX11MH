#pragma once
#include "Base.h"
BEGIN(Engine)
class CGameInstance;
class CPicking_System final : public CBase
{
#pragma region Constructor/Destructor
private:
	CPicking_System(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPicking_System() = default;

public:
	static CPicking_System* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
#pragma endregion


public:
	virtual HRESULT Initialize();
public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice		= { nullptr };
	ID3D11DeviceContext*	m_pContext		= { nullptr };
	
};
END