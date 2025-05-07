#pragma once
#include "Editor_Defines.h"
#include "MyImGui.h"
NS_BEGIN(Editor)
class CToolBar : public CMyImGui
{
#pragma region Constructor/Destructor
private:
	CToolBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CToolBar() = default;
public:
	static CToolBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
#pragma endregion

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
};
NS_END