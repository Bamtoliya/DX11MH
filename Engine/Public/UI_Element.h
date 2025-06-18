#pragma once
#include "GameObject.h"
NS_BEGIN(Engine)
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

#pragma region Variables
protected:
	UI_DESC	m_tDesc		= {};
protected:
	_bool m_bFocus		= { false };
protected:
#pragma endregion
};
NS_END