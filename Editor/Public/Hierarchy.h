#pragma once
#include "Editor_Defines.h"
#include "MyImGui.h"

NS_BEGIN(Editor)
class CHierarchy : public CMyImGui
{
#pragma region Constructor/Destructor
private:
	CHierarchy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CHierarchy() = default;
public:
	static CHierarchy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
#pragma endregion

public:
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_GameObject_Popup();
	HRESULT Add_Prototype_Popup();
	HRESULT Delete_GameObject_Popup();
private:
	LEVEL			m_eCurSelLevel = { LEVEL::STATIC };
};
NS_END