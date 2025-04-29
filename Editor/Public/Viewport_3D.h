#pragma once
#include "Editor_Defines.h"
#include "MyImGui.h"
BEGIN(Editor)
class CViewport_3D final : public CMyImGui
{
#pragma region Constructor/Destructor
private:
	CViewport_3D();
	virtual ~CViewport_3D() = default;
public:
	static CViewport_3D* Create();
	virtual void Free() override;
#pragma endregion

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
};
END

