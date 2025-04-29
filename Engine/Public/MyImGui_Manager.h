#pragma once
#include "Base.h"
BEGIN(Engine)
class CGameInstance;
#pragma region CallBack Settings
using ImGuiDrawCallBack = function<void()>;
using ImGuiDrawCallbackHandle = size_t;
constexpr ImGuiDrawCallbackHandle InvalidCallbackHandle = 0;
#pragma endregion

class ENGINE_DLL CMyImGui_Manager final : public CBase
{
public:
	typedef struct tagRegisteredCallbackInfo {
		ImGuiDrawCallbackHandle handle = InvalidCallbackHandle;
		ImGuiDrawCallBack       callback;
		std::string             name; // 디버깅용 이름
		// 필요시 우선순위(priority) 등의 메타데이터 추가 가능
	} CALLBACK_DESC;
private:
	CMyImGui_Manager(ID3D11Device*, ID3D11DeviceContext*);
	virtual ~CMyImGui_Manager() = default;
public:
	virtual HRESULT Initialize(HWND);
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render();
public:
	static CMyImGui_Manager* Create(HWND, ID3D11Device*, ID3D11DeviceContext*);
	void Free();
private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
public:
	ImGuiDrawCallbackHandle RegisterDrawCallback(ImGuiDrawCallBack callback, const std::string& debugName = "");
	bool UnregisterDrawCallback(ImGuiDrawCallbackHandle handle);
	void ClearAllCallbacks();
private:
	vector<CALLBACK_DESC> m_registeredCallbacks;
	ImGuiDrawCallbackHandle m_nextHandle = 1;
};
END
