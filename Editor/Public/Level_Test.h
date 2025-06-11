#pragma once
#include "Level.h"
NS_BEGIN(Editor)
class CLevel_Test : public CLevel
{
private:
	CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Test() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_GameObjects(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Terrain(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Light(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Sky(const _wstring& strLayerTag);
	HRESULT Ready_Layer_ImGui();

public:
	static CLevel_Test* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END