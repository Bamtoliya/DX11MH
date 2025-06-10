#pragma once
#include "Base.h"
NS_BEGIN(Engine)
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
	_byte Get_DIKState(_ubyte eKeyID) {
		return m_curKeyState[eKeyID];
	}
	_byte Get_DIKeyState(_ubyte eKeyID) { return m_curKeyState[eKeyID]; }
	_bool Key_Down(_ubyte eKeyID);
	_bool Key_Up(_ubyte eKeyID);
	_bool Key_Pressing(_ubyte eKeyID);
	_bool Any_KeyDown();
	
#pragma endregion

#pragma region Mouse
	_byte Get_DIMouseState(DIM eMouseKeyID) {
		return m_curMouseState.rgbButtons[ENUM_CLASS(eMouseKeyID)];
	}

	_long Get_DIMMoveState(DIMM eMouseMoveID) {
		return ((_long*)&m_curMouseState)[ENUM_CLASS(eMouseMoveID)];
	}
	_long	Get_DIMouseMove(DIMM eMouseState)
	{
		return *((reinterpret_cast<_int*>(&m_curMouseState)) + ENUM_CLASS(eMouseState));
	}
	_bool Mouse_Down(DIM eMouseKeyID);
	_bool Mouse_Up(DIM eMouseKeyID);
	_bool Mouse_Pressing(DIM eMouseKeyID);
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

NS_END;
