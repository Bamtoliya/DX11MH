#pragma once
#include "Editor_Defines.h"
#include "MyImGui.h"
BEGIN(Engine)
class CGameObject;
END
BEGIN(Editor)
class CInspector final : public CMyImGui
{
#pragma region Constructor/Destructor
private:
	CInspector();
	virtual ~CInspector() = default;
public:
	static CInspector* Create();
	virtual void Free() override;
#pragma endregion

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void SetGameObject(CGameObject* pGameObject) { m_pGameObject = pGameObject; }

private:
	CGameObject* m_pGameObject = { nullptr };
};
END