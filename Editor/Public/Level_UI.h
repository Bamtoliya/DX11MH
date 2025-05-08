#pragma once
#include "Editor_Defines.h"
#include "Level.h"
NS_BEGIN(Editor)
class CLevel_UI final : public CLevel
{
private:
	CLevel_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_UI() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_Background(const wstring strLayerTag);
	HRESULT Ready_Layer_Camera(const wstring strLayerTag);
	HRESULT Ready_Layer_ImGui();

public:
	static CLevel_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END