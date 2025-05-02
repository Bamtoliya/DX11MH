#pragma once
#include "Base.h"
BEGIN(Engine)
class CRenderTarget final : public CBase
{
#pragma region Constructor/Destructor
private:
	CRenderTarget(ID3D11Device pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

public:
	static CRenderTarget* Create(ID3D11Device pDevice, ID3D11DeviceContext* pContext);
	virtual void Free();
public:
	virtual HRESULT Initialize();
#pragma endregion
};
END