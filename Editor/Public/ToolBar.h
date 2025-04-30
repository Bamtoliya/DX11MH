#pragma once
#include "Editor_Defines.h"
#include "MyImGui.h"
BEGIN(Editor)
class CToolBar : public CMyImGui
{
#pragma region Constructor/Destructor
private:
	CToolBar();
	virtual ~CToolBar() = default;
public:
	static CToolBar* Create();
	virtual void Free() override;
#pragma endregion

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
};
END