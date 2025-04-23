#pragma once
#include "UI_Element.h"
BEGIN(Engine)
class ENGINE_DLL CPanel : public CUI_Element
{
#pragma region Constructor/Destructor
private:
	CPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPanel(const CUI_Element& Prototype);
	virtual ~CPanel() = default;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	//UI에 컴포넌트를 담아야하는가? 
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
};
END