#include "Input_Device.h"
#pragma region Constructor/Destructor

CInput_Device::CInput_Device()
{
}

CInput_Device* CInput_Device::Create(HINSTANCE hInstance, HWND hWnd)
{
    CInput_Device* pInstance = new CInput_Device();

    if (FAILED(pInstance->Initialize(hInstance, hWnd)))
    {
        MSG_BOX("Failed to Created : CInput_Device");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInput_Device::Free()
{
	Safe_Release(m_pKeyboard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}
#pragma endregion

HRESULT CInput_Device::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
		return E_FAIL;
	// 키보드 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
		return E_FAIL;
	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	if (FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		return E_FAIL;
	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	if (FAILED(m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE /*| DISCL_NOWINKEY*/)))
		return E_FAIL;
	// 장치에 대한 access 버전을 받아오는 함수
	if (FAILED(m_pKeyboard->Acquire()))
		return E_FAIL;
	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;
	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	if (FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
		return E_FAIL;
	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	if (FAILED(m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE /*| DISCL_NOWINKEY*/)))
		return E_FAIL;
	// 장치에 대한 access 버전을 받아오는 함수
	if (FAILED(m_pMouse->Acquire()))
		return E_FAIL;

	return S_OK;
}

void CInput_Device::Update(_float fTimeDelta)
{
	memcpy(&m_preKeyState, &m_curKeyState, sizeof(_char) * 256);
	m_pKeyboard->GetDeviceState(256, m_curKeyState);
	memcpy(&m_preMouseState, &m_curMouseState, sizeof(DIMOUSESTATE));
	HRESULT hr = m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_curMouseState);
	if (hr == E_FAIL)
	{
		MSG_BOX("WHELL NOT RESET");
	}
	int a = 10;
}

_bool CInput_Device::Key_Down(_ubyte eKeyID)
{
	if (m_preKeyState[eKeyID] != m_curKeyState[eKeyID])
	{
		if (m_curKeyState[eKeyID] & 0x80)
			return true;
	}
	return false;
}

_bool CInput_Device::Key_Up(_ubyte eKeyID)
{
	if (m_preKeyState[eKeyID] != m_curKeyState[eKeyID])
	{
		if (m_preKeyState[eKeyID] & 0x80)
			return true;
	}
	return false;
}

_bool CInput_Device::Key_Pressing(_ubyte eKeyID)
{
	if (m_preKeyState[eKeyID] == m_curKeyState[eKeyID])
	{
		if (m_preKeyState[eKeyID] & 0x80)
			return true;
	}
	return false;
}

_bool CInput_Device::Mouse_Down(DIM eMouseKeyID)
{
	if (m_preMouseState.rgbButtons[ENUM_CLASS(eMouseKeyID)] != m_curMouseState.rgbButtons[ENUM_CLASS(eMouseKeyID)])
	{
		if (m_curMouseState.rgbButtons[ENUM_CLASS(eMouseKeyID)] & 0x80)
		{
			return true;
		}

	}
	return false;
}

_bool CInput_Device::Mouse_Up(DIM eMouseKeyID)
{
	if (m_preMouseState.rgbButtons[ENUM_CLASS(eMouseKeyID)] != m_curMouseState.rgbButtons[ENUM_CLASS(eMouseKeyID)])
	{
		if (m_preMouseState.rgbButtons[ENUM_CLASS(eMouseKeyID)] & 0x80)
		{
			return true;
		}

	}
	return false;
}

_bool CInput_Device::Mouse_Pressing(DIM eMouseKeyID)
{
	if (m_preMouseState.rgbButtons[ENUM_CLASS(eMouseKeyID)] == m_curMouseState.rgbButtons[ENUM_CLASS(eMouseKeyID)])
	{
		if (m_curMouseState.rgbButtons[ENUM_CLASS(eMouseKeyID)] & 0x80)
			return true;
	}
	return false;
}
