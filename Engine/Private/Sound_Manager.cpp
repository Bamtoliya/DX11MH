#include "Sound_Manager.h"
#include "Sound_Event.h"
#include "Transform.h"

inline bool EndsWith(const wstring& str, const wstring& suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

_uint CSound_Manager::sNextID = { 0 };

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Initialize(_int iNumChannels, FMOD_STUDIO_INITFLAGS studioFlags, FMOD_INITFLAGS flags, void* pArg)
{
    if (FAILED(Studio::System::create(&m_pStudioSystem)))
    {
        MSG_BOX("Failed to Created : m_pStudioSystem");
        return E_FAIL;
    }
    m_pStudioSystem->getCoreSystem(&m_pCoreSystem);
    m_pCoreSystem->setDSPBufferSize(256, 4);
    m_pStudioSystem->initialize(iNumChannels, studioFlags, flags, pArg);

    Load_Bank(L"Master");

    return S_OK;
}

void CSound_Manager::Load_Bank(const _wstring& strKeyword)
{
    wstring searchPath = BANKPATH L"\\*";
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(searchPath.c_str(), &findData);

    if (hFind != INVALID_HANDLE_VALUE) {
        // 첫 번째 파일 처리
        bool continueLoop = true;
        while (continueLoop) {

            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                wstring filename = findData.cFileName;
                if (filename.find(strKeyword) != wstring::npos && EndsWith(filename, L".bank")) {
                    wstring bankPath = BANKPATH L"" + filename;

                    // 기존 코드와 동일한 뱅크 로딩 로직
                    if (mBanks.find(bankPath) != mBanks.end())
                        continue;

                    FMOD::Studio::Bank* bank = nullptr;
                    FMOD_RESULT result = m_pStudioSystem->loadBankFile(
                        WideToUtf8(bankPath.c_str()).c_str(),
                        FMOD_STUDIO_LOAD_BANK_NORMAL,
                        &bank
                    );

                    const int maxPathLength = 512;
                    if (result == FMOD_OK) {
                        mBanks.emplace(bankPath, bank);
                        bank->loadSampleData();

                        int numEvents = 0;
                        bank->getEventCount(&numEvents);
                        if (numEvents > 0) {
                            vector<FMOD::Studio::EventDescription*> events(numEvents);
                            bank->getEventList(events.data(), numEvents, &numEvents);
                            char eventName[maxPathLength];
                            for (auto e : events) {
                                e->getPath(eventName, maxPathLength, nullptr);
                                mEvents.emplace(Utf8ToWide(eventName), e);
                            }
                        }

                        int numBuses = 0;
                        bank->getBusCount(&numBuses);
                        if (numBuses > 0) {
                            vector<FMOD::Studio::Bus*> buses(numBuses);
                            bank->getBusList(buses.data(), numBuses, &numBuses);
                            char busName[512];
                            for (auto bus : buses) {
                                bus->getPath(busName, 512, nullptr);
                                mBuses.emplace(Utf8ToWide(busName), bus);
                            }
                        }
                    }
                }
            }
            if (FindNextFile(hFind, &findData) == 0)
            {
                continueLoop = false;
            }
        }
        FindClose(hFind);
    }
}

void CSound_Manager::Unload_Bank(const _wstring& strBankPath)
{
    // 로드 되지 않았다면 무시
    auto iter = mBanks.find(strBankPath);
    if (iter != mBanks.end())
    {
        return;
    }

    // 먼저 이 뱅크의 모든 이벤트를 삭제한다.
    FMOD::Studio::Bank* bank = iter->second;
    int numEvents = 0;
    bank->getEventCount(&numEvents);
    if (numEvents > 0)
    {
        // 이 뱅크에서 event descriptions들을 가져온다.
        vector<FMOD::Studio::EventDescription*> events(numEvents);
        // 이벤트의 리스트를 가져온다.
        bank->getEventList(events.data(), numEvents, &numEvents);
        char eventName[512];
        for (auto e : events)
        {
            // 이 이벤트의 경로를 가져온다
            e->getPath(eventName, 512, nullptr);
            // 이 이벤트를 삭제한다.
            auto eventi = mEvents.find(Utf8ToWide(eventName).c_str());
            if (eventi != mEvents.end())
            {
                mEvents.erase(eventi);
            }
        }
    }

    // 이 뱅크의 모든 버스를 삭제한다.
    int numBuses = 0;
    bank->getBusCount(&numBuses);
    if (numBuses > 0)
    {
        // 이 뱅크의 모든 버스 리스트를 가져온다.
        vector<FMOD::Studio::Bus*> buses(numBuses);
        bank->getBusList(buses.data(), numBuses, &numBuses);
        char busName[512];
        for (auto bus : buses)
        {
            // 버스의 경로를 가져온다. (예를 들어 bus:/SFX 와 같은)
            bus->getPath(busName, 512, nullptr);
            // 이 버스를 삭제한다.
            auto busi = mBuses.find(Utf8ToWide(busName).c_str());
            if (busi != mBuses.end())
            {
                mBuses.erase(busi);
            }
        }
    }

    // 샘플 데이터와 뱅크를 unload
    bank->unloadSampleData();
    bank->unload();
    // banks map에서 bank 삭제
    mBanks.erase(iter);
}

void CSound_Manager::Unload_AllBank()
{
    for (auto& iter : mBanks)
    {
        iter.second->unloadSampleData();
        iter.second->unload();
    }
    mBanks.clear();
    mEvents.clear();
}

void CSound_Manager::Priority_Update(_float fTimeDelta)
{
	if (nullptr == m_pCoreSystem)
		return;
	if (nullptr == m_pStudioSystem)
		return;
}

void CSound_Manager::Update(_float fTimeDelta)
{
	if (nullptr == m_pCoreSystem)
		return;
	if (nullptr == m_pStudioSystem)
		return;
    // 정지된 이벤트를 찾는다
    vector<unsigned int> done;
    for (auto& iter : mEventInstances)
    {
        FMOD::Studio::EventInstance* e = iter.second;
        // 이 이벤트의 상태값을 얻는다.
        FMOD_STUDIO_PLAYBACK_STATE state;
        e->getPlaybackState(&state);
        if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
        {
            // 이벤트를 해제하고 id를 done 맵에 추가한다.
            e->release();
            done.emplace_back(iter.first);
        }
    }

    // 완료된 이벤트 인스턴스를 맵에서 제거
    for (auto id : done)
    {
        mEventInstances.erase(id);
    }

    // FMOD 업데이트
 	m_pStudioSystem->update();
	m_pCoreSystem->update();
}

void CSound_Manager::Late_Update(_float fTimeDelta)
{
	if (nullptr == m_pCoreSystem)
		return;
	if (nullptr == m_pStudioSystem)
		return;
}

CSound_Event CSound_Manager::Play_Background(const _wstring& strEventPath, void* pArg)
{
    unsigned int retID = 0;
    // 이벤트가 존재하는지 확인
    auto iter = mEvents.find(strEventPath);
    if (iter != mEvents.end())
    {
        // 이벤트의 인스턴스를 생성한다.
        FMOD::Studio::EventInstance* event = nullptr;
        iter->second->createInstance(&event);
        if (event)
        {
            // 이벤트 인스턴스를 시작한다.
            event->start();
            // 새 아이디를 얻어 맵에 추가한다.
            sNextID++;
            retID = sNextID;
            mEventInstances.emplace(retID, event);

            //// release는 이벤트 인스턴스가 정지할 때 이벤트 소멸자 실행을 예약한다.
            //// 반복하지 않는 이벤트는 자동으로 정지한다.
            event->release();
        }
    }
    return CSound_Event(this, retID);
}

CSound_Event* CSound_Manager::Play_Event(const _wstring& strEventPath, void* pArg)
{
    unsigned int retID = 0;
    // 이벤트가 존재하는지 확인
    auto iter = mEvents.find(strEventPath);
    if (iter != mEvents.end())
    {
        // 이벤트의 인스턴스를 생성한다.
        FMOD::Studio::EventInstance* event = nullptr;
        iter->second->createInstance(&event);
        if (event)
        {
            FMOD_STUDIO_PLAYBACK_STATE tmp = {};
            event->setUserData(m_pStudioSystem);
            // 이벤트 인스턴스를 시작한다.
            event->start();
            // 새 아이디를 얻어 맵에 추가한다.
            sNextID++;
            retID = sNextID;
            mEventInstances.emplace(retID, event);

            //// release는 이벤트 인스턴스가 정지할 때 이벤트 소멸자 실행을 예약한다.
            //// 반복하지 않는 이벤트는 자동으로 정지한다.
            event->release();
        }
    }
    CSound_Event* e = new CSound_Event(this, retID); 
    return e;
}

Studio::EventInstance* CSound_Manager::Get_EventInstance(_uint iID)
{
    FMOD::Studio::EventInstance* event = nullptr;
    auto iter = mEventInstances.find(iID);
    if (iter != mEventInstances.end())
    {
        event = iter->second;
    }
    return event;
}

void CSound_Manager::Stop_All_Event()
{
	Studio::Bus* m_pBus = nullptr;
	m_pStudioSystem->getBus("bus:/", &m_pBus);
	m_pBus->stopAllEvents(FMOD_STUDIO_STOP_IMMEDIATE);
	//Update();
}

_float CSound_Manager::Get_Global_Parameter(const string& name)
{
    _float value = {};
    m_pStudioSystem->getParameterByName(name.c_str(), &value);
    return value;
}

void CSound_Manager::Set_Global_Parameter(const string& name, _float value)
{
    m_pStudioSystem->setParameterByName(name.c_str(), value);
}

void CSound_Manager::Set_Listner(const CTransform& worldTrans)
{
    //m_pGraphic_Device->GetViewMatrix();
    //worldTrans.
    FMOD_3D_ATTRIBUTES listener = {};
    // 리스너의 위치와 전방 벡터, 상향 벡터를 설정
    listener.position = VecToFMOD(worldTrans.Get_State(CTransform::POS));
    //// 뷰 행렬의 역행렬에서 세 번째 행은 전방 벡터
    listener.forward = VecToFMOD(worldTrans.Get_State(CTransform::LOOK));
    //// 뷰 행렬의 역행렬에서 두 번째 행은 상향 벡터
    listener.up = VecToFMOD(worldTrans.Get_State(CTransform::UP));
    // 속도를 0으로 설정 (도플러 효과를 사용할 시 수정)
    listener.velocity = { 0.0f, 0.0f, 0.0f };
    // FMOD로 보낸다 (0 = 리스너는 하나)
    m_pStudioSystem->setListenerAttributes(0, &listener);
}

_float CSound_Manager::GetBusVolume(const _wstring& name) const
{
    _float retVal = 0.f;
    const auto iter = mBuses.find(name);
    if (iter != mBuses.end())
    {
        iter->second->getVolume(&retVal);
    }
    return retVal;
}

_bool CSound_Manager::GetBusPaused(const _wstring& name) const
{
    _bool retVal = false;
    const auto iter = mBuses.find(name);
    if (iter != mBuses.end())
    {
        iter->second->getPaused(&retVal);
    }
    return retVal;  
}

void CSound_Manager::SetBusVolume(const _wstring& name, float volume)
{
    auto iter = mBuses.find(name);
    if (iter != mBuses.end())
    {
        iter->second->setVolume(volume);
    }
}

void CSound_Manager::SetBusPaused(const _wstring& name, bool pause)
{
    auto iter = mBuses.find(name);
    if (iter != mBuses.end())
    {
        iter->second->setPaused(pause);
    }
}

CSound_Manager* CSound_Manager::Create(_int iNumChannels, FMOD_STUDIO_INITFLAGS studioFlags, FMOD_INITFLAGS flags, void* pArg)
{
	CSound_Manager* pInstance = new CSound_Manager();

	if (FAILED(pInstance->Initialize(iNumChannels, studioFlags, flags, pArg)))
	{
		MSG_BOX("Failed to Created : CSound_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound_Manager::Free()
{
	__super::Free();
    //if (m_iRefCnt)
    Unload_AllBank();
	//대소문자 구별로 인한 매크로 함수 호출 불가능
    
	if (nullptr != m_pStudioSystem)
	{
		m_pStudioSystem->release();
		m_pStudioSystem = nullptr;
	}		
	if (nullptr != m_pCoreSystem)
	{
		m_pCoreSystem->release();
		m_pCoreSystem = nullptr;
	}
}
