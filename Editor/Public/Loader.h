#pragma once

#include "Editor_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Editor)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	_bool isFinished() const {
		return m_isFinished;
	}

public:
	HRESULT Initialize(LEVEL eNextLevelID);
	HRESULT Loading();

	void Output_LoadingText() {
		SetWindowText(g_hWnd, m_szLoadingText);
	}
	

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	LEVEL						m_eNextLevelID = { LEVEL::END };
	_bool						m_isFinished = { false };
	CGameInstance*				m_pGameInstance = { nullptr };

private:
	HANDLE						m_hThread = {};
	CRITICAL_SECTION			m_CriticalSection = {};
	_tchar						m_szLoadingText[MAX_PATH] = {};

private:
	_uint m_iTotalLoadingItems = 0;
	_uint m_iLoadedItems = 0;
	_uint m_iTotalJsonItems = 0; // JSON 파일 내의 총 아이템 수를 저장하기 위한 변수 (선택적)
	_uint m_iLoadedJsonItems = 0; // JSON 파일에서 로드된 아이템 수를 저장하기 위한 변수

public:
    // ...
    void Set_TotalLoadingItems(_uint iTotal) {
        m_iTotalLoadingItems = iTotal;
        m_iLoadedItems = 0;
        m_iTotalJsonItems = 0; // 초기화
        m_iLoadedJsonItems = 0; // 초기화
    }
    void Increment_LoadedItems(UINT iNumItems = 1) { // 여러 개 증가 옵션
        m_iLoadedItems += iNumItems;
        if (m_iLoadedItems > m_iTotalLoadingItems) m_iLoadedItems = m_iTotalLoadingItems; // 최대값 초과 방지
    }
    _float Get_LoadingProgress() const {
        //if (m_iTotalLoadingItems == 0) return m_isFinished ? 100.0f : 0.0f;
        return static_cast<_float>(m_iLoadedJsonItems) / m_iTotalJsonItems * 100.0f;
    }

    // JSON 로딩 진행 콜백을 위한 정적 멤버 함수 또는 전역 함수
    static void JsonLoadCallback(void* pUserData, _uint iLoadedCount, _uint iTotalInJson) {
        if (pUserData) {
            CLoader* pLoader = static_cast<CLoader*>(pUserData);
            pLoader->m_iTotalJsonItems = iTotalInJson;
            pLoader->m_iLoadedJsonItems = iLoadedCount;
            //pLoader->UpdateJsonLoadProgress(iLoadedCount, iTotalInJson);
        }
    }

    void UpdateJsonLoadProgress(_uint iLoadedCount, _uint iTotalInJson) {
        // 이전에 JSON에서 로드된 항목 수를 빼고 새로 계산된 항목 수를 더함
        m_iLoadedItems -= m_iLoadedJsonItems;
        m_iLoadedJsonItems = iLoadedCount;
        m_iLoadedItems += m_iLoadedJsonItems;

        // m_iTotalJsonItems는 처음 콜백 시 설정하거나, Load_Prototypes 전에 미리 계산하여 설정
        if (m_iTotalJsonItems == 0 && iTotalInJson > 0) {
            // 만약 CJson_Loader가 총량을 전달해준다면, 전체 로딩 아이템 수에 반영할 수 있음
            // 이 부분은 전체 아이템 수 계산 로직과 연동 필요
        }
        if (m_iLoadedItems > m_iTotalLoadingItems) m_iLoadedItems = m_iTotalLoadingItems;
    }

public:
	HRESULT Loading_For_Logo();
	HRESULT Loading_For_UI();
	HRESULT Loading_For_Map();


public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;
};

NS_END