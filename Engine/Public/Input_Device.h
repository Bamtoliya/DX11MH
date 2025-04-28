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
	static CInput_Device* Create();
	void Free();
#pragma endregion
#pragma region Intialize
public:
	virtual HRESULT Initialize();
#pragma endregion
public:
	virtual void Update(_float fTimeDelta);

#pragma region Keyboard
	/*_bool IsKeyDown(_uint vkCode) const;
	_bool IsKeyUp(_uint vkCode) const;
	_bool IsKeyPressing(_uint vkCode) const;
	_bool IsKeyPressedInState(const GameInputKeyState& state, _uint vkCode) const;*/
#pragma endregion

#pragma region Mouse

#pragma endregion


private:
	LPDIRECTINPUT8				m_pInputSDK = nullptr;
	LPDIRECTINPUTDEVICE8		m_pKeyboard = nullptr;
	LPDIRECTINPUTDEVICE8		m_pMouse = nullptr;
};

END;