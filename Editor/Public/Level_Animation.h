#pragma once
#include "Editor_Defines.h"
#include "Level.h"
NS_BEGIN(Editor)
class CLevel_Animation : public CLevel
{
private:
	CLevel_Animation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Animation() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_Light(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Sky(const _wstring& strLayerTag);
	HRESULT Ready_Layer_ImGui();

public:
	static CLevel_Animation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};
NS_END