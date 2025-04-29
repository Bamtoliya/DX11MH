#pragma once
#include "Editor_Defines.h"
#include "MyImGui.h"
BEGIN(Editor)
class CViewport_2D final : public CMyImGui
{
#pragma region Constructor/Destructor
private:
	CViewport_2D();
	virtual ~CViewport_2D() = default;
public:
	static CViewport_2D* Create();
	virtual void Free() override;
#pragma endregion

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
};
END
