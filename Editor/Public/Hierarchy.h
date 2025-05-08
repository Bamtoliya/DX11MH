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

};
NS_END