#pragma once
#include "Editor_Defines.h"
#include "Level.h"
NS_BEGIN(Editor)
class CGame_Manager;
class CLevel_Stage01 : public CLevel
{
private:
	CLevel_Stage01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Stage01() = default;

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
	static CLevel_Stage01* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

private:
	CGame_Manager* m_pGame_Manager = { nullptr };
	_bool			m_bChangeToLogo = { false };
	_bool			m_bChangeToOpen = { false };
};
NS_END