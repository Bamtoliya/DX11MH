#pragma once
#include "Base.h"
#include "Sound_Event.h"

BEGIN(Engine)
class CSound_Manager final : public CBase
{
public:
	enum CHANNELID { SND_BACKGROUND = 0, SND_SFX, SND_END};
#pragma region Constructor/Destructor
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;
public:
	HRESULT					Initialize(_int iNumChannels, FMOD_STUDIO_INITFLAGS studioFlags, FMOD_INITFLAGS flags, void* pArg = nullptr);
	static CSound_Manager*	Create(_int iNumChannels, FMOD_STUDIO_INITFLAGS studioFlags, FMOD_INITFLAGS flags, void* pArg = nullptr);
	virtual void			Free() override;
#pragma endregion
#pragma region Loops
public:
	void			Priority_Update(_float fTimeDelta);
	void			Update(_float fTimeDelta);
	void			Late_Update(_float fTimeDelta);
#pragma endregion
#pragma region Load
public:
	void					Load_Bank(const _wstring& strBankPath);
	void					Unload_Bank(const _wstring& strBankPath);
	void					Unload_AllBank();
#pragma endregion
#pragma region Play/Stop
public:
	class CSound_Event		Play_Background(const _wstring& strEventPath, void* pArg = nullptr);
	class CSound_Event*		Play_Event(const _wstring& strEventPath, void* pArg = nullptr);
public:
	void					Stop_All_Event();
	void					Stop();
#pragma endregion

public:
	_float					Get_Global_Parameter(const string& name);
	void					Set_Global_Parameter(const string& name, _float value);
	
public:
	void					Set_Listner(const class CTransform& worldTrans);

#pragma region BusControl
public:
	// 버스 제어를 위한 getter / setter
	_float		GetBusVolume(const _wstring& name) const;
	_bool		GetBusPaused(const _wstring& name) const;
	void		SetBusVolume(const _wstring& name, float volume);
	void		SetBusPaused(const _wstring& name, bool pause);
#pragma endregion


public:
	Studio::EventInstance* Get_EventInstance(_uint iID);

#pragma region Variables
private:
	static _uint sNextID;
private:
	FMOD::Studio::System* m_pStudioSystem = { nullptr }; //스튜디오 객체
	FMOD::System* m_pCoreSystem = { nullptr };	//코어 객체(전에 사용 했던 방식)
	// 로드된 뱅크를 관리하는 맵
	// key: 뱅크 파일 이름
	// value: 뱅크 포인터
	unordered_map<_wstring, FMOD::Studio::Bank*> mBanks;

	// 이벤트 이름과 EventDescription 맵
	// key: FMOD가 할당한 이벤트 이름
	// value: EventDescription 포인터
	unordered_map<_wstring, FMOD::Studio::EventDescription*> mEvents;

	// EventInstance를 관리하기 위한 이벤트 id 맵
	// key: 부호없는 정수형 id
	// value: EventInstance
	unordered_map<_uint, FMOD::Studio::EventInstance*> mEventInstances;

	// Bus를 관리하기 위한 맵
	// key: Bus의 이름
	// value: Bus의 포인터
	unordered_map<_wstring, FMOD::Studio::Bus*> mBuses;
#pragma endregion
};

END