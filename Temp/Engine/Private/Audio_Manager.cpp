#include "Audio_Manager.h"

HRESULT CAudio_Manager::Initialize(_int iNumChannels, FMOD_STUDIO_INITFLAGS studioFlags, FMOD_INITFLAGS flags, void* pArg)
{
    // 1. 메모리 설정 (필요시 커스텀 설정)
    AkMemSettings memSettings{};
    AK::MemoryMgr::GetDefaultSettings(memSettings);
    // memSettings 값 수정...

    // 2. 스트리밍 장치 및 관리자 설정 (기본값 사용 또는 커스텀)
    AkDeviceSettings deviceSettings;
    AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);
    // deviceSettings 값 수정...

    AkStreamMgrSettings stmSettings;
    AK::StreamMgr::GetDefaultSettings(stmSettings);
    // stmSettings 값 수정...

    // 3. 사운드 엔진 초기화 설정
    AkInitSettings initSettings;
    AK::SoundEngine::GetDefaultInitSettings(initSettings);
    // initSettings 값 수정...

    AkPlatformInitSettings platformInitSettings;
    AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);
    // platformInitSettings 값 수정...

    // --- 이제 설정들을 사용하여 초기화 ---

    // 메모리 관리자 초기화 (내부적으로 처리되거나, 명시적 호출 방식이 다를 수 있음 - 문서 확인 필요)
    // 예: if (AK::MemoryMgr::Init(&memSettings) != AK_Success) return false; // 이전 방식, 현재는 다를 수 있음!
    AKRESULT
        // 스트리밍 관리자/장치 생성 (여전히 명시적 생성이 필요할 수 있음 - 문서 확인 필요)
        if (!AK::StreamMgr::Create(stmSettings)) return E_FAIL;
    if (AK::StreamMgr::CreateDevice(deviceSettings) == AK_FAIL) return E_FAIL; // 저수준 IO 구현 필요할 수 있음

    // 코어 사운드 엔진 초기화 (이 함수가 많은 것을 처리)
    if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success) {
        return false;
    }

    // 음악 엔진, 공간 음향 등 다른 모듈 초기화 (필요시)
    // AkMusicSettings musicSettings; ... AK::MusicEngine::Init(&musicSettings);
    // AkSpatialAudioInitSettings spatialSettings; ... AK::SpatialAudio::Init(spatialSettings);

    // 통신 초기화 (프로파일링 시)
    // AkCommSettings commSettings; ... AK::Comm::Init(commSettings);
    return S_OK;
}
