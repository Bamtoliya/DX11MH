#pragma once
#include "Level.h"
#include "Editor_Defines.h"
NS_BEGIN(Editor)
class CLevel_Effect final : public CLevel
{
private:
	CLevel_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Effect() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_ImGui();

public:
	static CLevel_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};
NS_END