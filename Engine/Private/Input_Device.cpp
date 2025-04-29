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
	/* m_pInputSDK */
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
		return E_FAIL;

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
		return E_FAIL;

	if (FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		return E_FAIL;
	if (FAILED(m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE /*| DISCL_NOWINKEY*/)))
		return E_FAIL;
	if (FAILED(m_pKeyboard->Acquire()))
		return E_FAIL;

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	if (FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
		return E_FAIL;
	if (FAILED(m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE /*| DISCL_NOWINKEY*/)))
		return E_FAIL;
	if (FAILED(m_pMouse->Acquire()))
		return E_FAIL;

	return S_OK;
}

void CInput_Device::Update(_float fTimeDelta)
{
	memcpy(&m_preKeyState, &m_curKeyState, sizeof(_char) * 256);
	m_pKeyboard->GetDeviceState(256, m_curKeyState);
	memcpy(&m_preMouseState, &m_curMouseState, sizeof(DIMOUSESTATE));
	m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_curMouseState);
}

_bool CInput_Device::Key_Down(_uchar eKeyID)
{
	if (m_preKeyState[eKeyID] != m_curKeyState[eKeyID])
	{
		if (m_curKeyState[eKeyID] & 0x80)
			return true;
	}
	return false;
}

_bool CInput_Device::Key_Up(_uchar eKeyID)
{
	if (m_preKeyState[eKeyID] != m_curKeyState[eKeyID])
	{
		if (m_preKeyState[eKeyID] & 0x80)
			return true;
	}
	return false;
}

_bool CInput_Device::Key_Pressing(_uchar eKeyID)
{
	if (m_preKeyState[eKeyID] == m_curKeyState[eKeyID])
	{
		if (m_preKeyState[eKeyID] & 0x80)
			return true;
	}
	return false;
}

_bool CInput_Device::Mouse_Down(DIMK eMouseKeyID)
{
	if (m_preMouseState.rgbButtons[eMouseKeyID] != m_curMouseState.rgbButtons[eMouseKeyID])
	{
		if (m_curMouseState.rgbButtons[eMouseKeyID] & 0x80)
		{
			return true;
		}

	}
	return false;
}

_bool CInput_Device::Mouse_Up(DIMK eMouseKeyID)
{
	if (m_preMouseState.rgbButtons[eMouseKeyID] != m_curMouseState.rgbButtons[eMouseKeyID])
	{
		if (m_preMouseState.rgbButtons[eMouseKeyID] & 0x80)
		{
			return true;
		}

	}
	return false;
}

_bool CInput_Device::Mouse_Pressing(DIMK eMouseKeyID)
{
	if (m_preMouseState.rgbButtons[eMouseKeyID] == m_curMouseState.rgbButtons[eMouseKeyID])
	{
		if (m_curMouseState.rgbButtons[eMouseKeyID] & 0x80)
			return true;
	}
	return false;
}
