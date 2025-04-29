#pragma once
#include "Base.h"
BEGIN(Engine)
class CGameInstance;
class CMyImGui;
class ENGINE_DLL CMyImGui_Manager final : public CBase
{
public:

#pragma region Constructor/Destructor
private:
	CMyImGui_Manager(ID3D11Device*, ID3D11DeviceContext*);
	virtual ~CMyImGui_Manager() = default;
public:
	static CMyImGui_Manager* Create(HWND, ID3D11Device*, ID3D11DeviceContext*);
	void Free();
#pragma endregion

public:
	virtual HRESULT Initialize(HWND);
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render();
private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
private:
	void DrawGridWindow(bool* p_open);
	void DrawAssetBrowser(bool* p_open);
public:

	HRESULT Add_ImGui(const _wstring& key, CMyImGui* pMyImGui);
private:
	map<const _wstring, CMyImGui*>		m_vecMyImGui = {};
};
END
