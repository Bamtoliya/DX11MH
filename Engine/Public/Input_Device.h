#pragma once
#include "Base.h"

BEGIN(Engine)
class CInput_Device final : public CBase
{
#pragma region Structs
public:
	typedef struct tagInputStates {
		GameInputKeyState keyboardState = {};
		GameInputMouseState mouseState = {};
		std::map<uint64_t, GameInputGamepadState> controllerStates;
		// Device ID별 컨트롤러 상태 저장
		// 필요하다면 이전 상태도 저장하여 버튼 누름/뗌 감지
	} INPUT_STATE;
	typedef struct tagDragDesc {
		GameInputMouseButtons button;
		float startX;
		float startY;
		float currentX; // 필요하다면 현재 위치도 저장
		float currentY;
	} DRAG_DESC;
#pragma endregion
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
	IGameInput*			m_GameInput = { nullptr };
	GameInputKeyState*	m_preKeyState = { nullptr };
	GameInputKeyState*	m_curKeyState = { nullptr };
	INPUT_STATE			m_curStates = {};
	INPUT_STATE			m_preStates = {};
};
END