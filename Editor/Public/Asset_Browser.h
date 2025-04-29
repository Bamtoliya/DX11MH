#pragma once
#include "Editor_Defines.h"
#include "MyImGui.h"
BEGIN(Editor)
class CAsset_Browser final : public CMyImGui
{
#pragma region Constructor/Destructor
private:
	CAsset_Browser();
	virtual ~CAsset_Browser() = default;
public:
	static CAsset_Browser* Create();
	virtual void Free() override;
#pragma endregion

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
};
END