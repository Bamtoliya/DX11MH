#pragma once
#include "Editor_Defines.h"
#include "MyImGui.h"
NS_BEGIN(Engine)
class CGameObject;
NS_END
NS_BEGIN(Editor)
class CInspector final : public CMyImGui
{
#pragma region Constructor/Destructor
private:
	CInspector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CInspector() = default;
public:
	static CInspector* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
#pragma endregion

public:
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void SetGameObject(CGameObject* pGameObject) { m_pGameObject = pGameObject; }

private:
	HRESULT PopUpComponents();

private:
	CGameObject* m_pGameObject = { nullptr };
};
NS_END