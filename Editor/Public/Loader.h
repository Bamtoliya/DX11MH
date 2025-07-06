#pragma once

#include "Editor_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Editor)

class CLoader final : public CBase
{
#pragma region Constructor/Destructor
private:
    CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CLoader() = default;
public:
    static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
public:
    HRESULT Initialize(LEVEL eNextLevelID);
public:
    virtual void Free() override;
#pragma endregion

public:
	_bool isFinished() const { return m_isFinished; }
	HRESULT Loading();
	void Output_LoadingText() { SetWindowText(g_hWnd, m_szLoadingText); }
public:
    HRESULT Execute_CommandLists(ID3D11DeviceContext* pMainContext);


//Deferred Context
private:
    ID3D11DeviceContext* m_pDeferredContext = nullptr;
    queue<ID3D11CommandList*> m_CommandListQueue;
    mutex m_QueueMutex;

#pragma region Variables
private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    CGameInstance* m_pGameInstance = { nullptr };
    LEVEL						m_eNextLevelID = { LEVEL::END };
    _bool						m_isFinished = { false };

    HANDLE						m_hThread = {};
    CRITICAL_SECTION			m_CriticalSection = {};
    _tchar						m_szLoadingText[MAX_PATH] = {};
#pragma endregion

private:
	_uint m_iTotalLoadingItems = 0;
	_uint m_iLoadedItems = 0;
	_uint m_iTotalJsonItems = 0; // JSON 파일 내의 총 아이템 수를 저장하기 위한 변수 (선택적)
	_uint m_iLoadedJsonItems = 0; // JSON 파일에서 로드된 아이템 수를 저장하기 위한 변수

#pragma region Loading Count

#pragma endregion

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
            if (nullptr == pLoader)
                return;
            pLoader->m_iTotalJsonItems = iTotalInJson;
            pLoader->m_iLoadedJsonItems = iLoadedCount;

            if (pLoader->m_pDeferredContext && iLoadedCount % 5 == 0)  // 5개 항목마다 커맨드 리스트 생성 (성능 조정 가능)
            {
                ID3D11CommandList* pCommandList = nullptr;
                if (SUCCEEDED(pLoader->m_pDeferredContext->FinishCommandList(FALSE, &pCommandList)))
                {
                    std::lock_guard<std::mutex> lock(pLoader->m_QueueMutex);
                    pLoader->m_CommandListQueue.push(pCommandList);
                }
            }
        }
    }

    // Deferred Context 접근 함수 추가
    ID3D11DeviceContext* Get_DeferredContext() const { return m_pDeferredContext; }

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
#pragma region Loading For Level
public:
    HRESULT Loading_For_Logo();
    HRESULT Loading_For_UI();
    HRESULT Loading_For_Map();
    HRESULT Loading_For_Effect();
    HRESULT Loading_For_Gameplay();
    HRESULT Loading_For_Animation();
    HRESULT Loading_For_Test();
    HRESULT Loading_For_Open();
    HRESULT Loading_For_Stage01();
    HRESULT Loading_For_Stage02();
#pragma endregion





};

NS_END