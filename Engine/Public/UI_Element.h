#pragma once
#include "GameObject.h"
BEGIN(Engine)
class ENGINE_DLL CUI_Element abstract : public CGameObject
{
#pragma region Struct
public:
typedef struct tagUiDesc {
	CUI_Element* parent;
	_float2 pos;
	_float2 size;
}UI_DESC;
#pragma endregion
#pragma region Constructor/Destructor

protected:
	CUI_Element(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Element(const CUI_Element& Prototype);
	virtual ~CUI_Element() = default;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Ready_Components();
public:
	virtual CGameObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr) PURE;
	virtual CGameObject* Clone(void* pArg = nullptr) PURE;
	void Free() override;
#pragma endregion
#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
public:
	virtual HRESULT Pre_Render() override;
	virtual HRESULT Render() override;
	virtual HRESULT Post_Render() override;
#pragma endregion

public:
	CUI_Element*	GetParent() { return m_pParent ? m_pParent : nullptr; }
	void			Add_UI(_wstring strKey, CUI_Element* pUIElement) { m_Children.emplace(strKey, pUIElement); }
	void			Remove_UI(_wstring strKey) { m_Children.erase(strKey); }
public:
	void			Show() { m_bShow = true; }
	void			Hide() { m_bShow = false; }
#pragma region Variables
protected:
	CUI_Element* m_pParent = { nullptr };
protected:
	map<const _wstring, CUI_Element*> m_Children;
protected:
	UI_DESC	m_tDesc		= {};
protected:
	bool m_bShow		= { false };
	bool m_bFocus		= { false };
	bool m_bIsActive	= { false };
	bool m_bLocal		= { false };
	bool m_bWorld		= { false };
protected:
#pragma endregion
};
END