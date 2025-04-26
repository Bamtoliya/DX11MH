#include "Input_Device.h"

CInput_Device::CInput_Device()
    : m_GameInput { nullptr }
{
}

HRESULT CInput_Device::Initialize()
{
    if (FAILED(GameInputCreate(&m_GameInput)))
        return E_FAIL;
    Safe_AddRef(m_GameInput);
	return S_OK;
}

void CInput_Device::Update(_float fTimeDelta)
{
    m_preStates = m_curStates;
    if (!m_GameInput) return;
    IGameInputReading* reading = nullptr;
    HRESULT hr = m_GameInput->GetCurrentReading(GameInputKindKeyboard | GameInputKindMouse | GameInputKindGamepad, nullptr, &reading);
    if (FAILED(hr) || reading == nullptr) {
        // 오류 처리 또는 로깅
        if (reading) {
            reading->Release(); // 오류가 발생했더라도 얻었다면 해제
        }
        return;
    }

    // 키보드 상태 읽기
    int b = reading->GetKeyCount();
    reading->GetKeyState(b, m_curStates.keyboardState);
    //m_curStates.keyboardState++;\

    if (b == 2)
    {
        int c = 10;
    }
        

    if (m_curStates.keyboardState->virtualKey == VK_TAB)
    {
        int a = 10;
    }

    if (m_curStates.keyboardState->scanCode == 'a')
    {
        int a = 10;
    }

    // 마우스 상태 읽기
    reading->GetMouseState(m_curStates.mouseState);

    //for (uint32_t i = 0; i < actualCount; ++i) {
    //    IGameInputDevice* gamepadDevice = nullptr;
    //    // GetGamepadState만으로는 deviceId를 직접 알 수 없으므로,
    //    // GetDeviceByIndex 또는 다른 방법으로 Device를 얻어 ID를 매핑해야 할 수 있습니다.
    //    // 여기서는 GamepadState 자체에 deviceId가 있다고 가정하거나,
    //    // GameInput 콜백을 사용하여 연결된 디바이스 목록을 유지 관리해야 합니다.
    //    // 간단한 예시로, 여기서는 인덱스를 키처럼 사용하지만 실제로는 deviceId를 사용해야 합니다.
    //    // GameInput 콜백을 통해 deviceId와 상태를 매핑하는 로직이 필요합니다.
    //    // 임시 방편: 실제로는 deviceId를 얻어와야 합니다.
    //    // 예를 들어, 콜백에서 관리하는 연결된 디바이스 목록에서 인덱스 i에 해당하는 디바이스 ID를 가져옵니다.
    //    // uint64_t deviceId = GetDeviceIdForIndex(i); // 이런 함수가 있다고 가정
    //    // m_curStates.controllerStates[deviceId] = gamepadStates[i];

    //    // GameInput API 문서를 참조하여 정확한 deviceId 얻는 방법을 확인하세요.
    //    // GetDeviceByIndex 또는 콜백 등록을 통한 디바이스 관리가 일반적입니다.
    //    // 여기서는 간단하게 인덱스를 키로 사용합니다. (실제 구현 시 수정 필요)
    //    m_curStates.controllerStates[i] = gamepadStates[i]; // << 주의: 실제 deviceId를 사용해야 함
    //}

    reading->Release();
}

//_bool CInput_Device::IsKeyDown(_uint vkCode) const
//{
//    // 현재 프레임에 키가 눌려있는지 확인
//    bool currentPressed = IsKeyPressedInState(*m_curStates.keyboardState, vkCode);
//    // 이전 프레임에 키가 눌려있었는지 확인
//    bool previousPressed = IsKeyPressedInState(*m_preStates.keyboardState, vkCode);
//
//    // 현재는 눌려있고, 이전에는 눌려있지 않았다면 KeyDown
//    return currentPressed && !previousPressed;
//}
//
//_bool CInput_Device::IsKeyUp(_uint vkCode) const
//{
//    return _bool();
//}
//
//_bool CInput_Device::IsKeyPressing(_uint vkCode) const
//{
//    return _bool();
//}
//
//_bool CInput_Device::IsKeyPressedInState(const GameInputKeyState& state, _uint vkCode) const
//{
//    return (GetAsyncKeyState(vkCode) & 0x8000) != 0;
//}

CInput_Device* CInput_Device::Create()
{
    CInput_Device* pInstance = new CInput_Device();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CInput_Device");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInput_Device::Free()
{
	__super::Free();
    Safe_Release(m_GameInput);
}
