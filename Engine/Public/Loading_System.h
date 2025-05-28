#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CLoading_System final : public CBase
{
#pragma region Constructor/Desturctor
private:
	CLoading_System(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoading_System() = default;

public:
	static CLoading_System* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) { return new CLoading_System(pDevice, pContext); }
public:
	virtual HRESULT Initialize();
public:
	virtual void Free() override;
#pragma endregion

#pragma region Loops
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
#pragma endregion

};

NS_END