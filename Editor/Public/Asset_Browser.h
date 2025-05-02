#pragma once
#include "Editor_Defines.h"
#include "MyImGui.h"
BEGIN(Editor)
class CAsset_Browser final : public CMyImGui
{
#pragma region Constructor/Destructor
private:
	
	CAsset_Browser(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CAsset_Browser() = default;
public:
	static CAsset_Browser* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
#pragma endregion

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
};
END