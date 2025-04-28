#pragma once
#include "Base.h"
BEGIN(Engine)
class CInput_Device : public CBase
{
#pragma region Constructor/Destructor
private:
	CInput_Device();
	virtual ~CInput_Device() = default;
public:
	static CInput_Device* Create(HINSTANCE hInstance, HWND hWnd);
	void Free();
#pragma endregion
#pragma region Intialize
public:
	virtual HRESULT Initialize(HINSTANCE hInstance, HWND hWnd);
#pragma endregion
public:
	virtual void Update(_float fTimeDelta);

#pragma region Keyboard
	_char Get_DIKState(_uchar eKeyID) {
		return m_curKeyState[eKeyID];
	}
	_bool Key_Down(_uchar eKeyID);
	_bool Key_Up(_uchar eKeyID);
	_bool Key_Pressing(_uchar eKeyID);

	
#pragma endregion

#pragma region Mouse
	_char Get_DIMKeyState(DIMK eMouseKeyID) {
		return m_curMouseState.rgbButtons[eMouseKeyID];
	}

	_long Get_DIMMoveState(DIMM eMouseMoveID) {
		return ((_long*)&m_curMouseState)[eMouseMoveID];
	}
	_bool Mouse_Down(DIMK eMouseKeyID);
	_bool Mouse_Up(DIMK eMouseKeyID);
	_bool Mouse_Pressing(DIMK eMouseKeyID);
#pragma endregion


private:
	LPDIRECTINPUT8				m_pInputSDK = nullptr;
	LPDIRECTINPUTDEVICE8		m_pKeyboard = nullptr;
	LPDIRECTINPUTDEVICE8		m_pMouse = nullptr;
private:
	_char						m_preKeyState[256]	= { 0 };
	_char						m_curKeyState[256]	= { 0 };
	DIMOUSESTATE				m_preMouseState;
	DIMOUSESTATE				m_curMouseState;
};

END;