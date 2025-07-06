#pragma once


namespace Editor
{
	enum class LEVEL { STATIC, LOADING, LOGO, UI, MAP, EFFECT, ANIMATION, GAMEPLAY, TEST, OPEN, STAGE01, STAGE02, END };
	inline const char* ToString(LEVEL e)
	{
		switch (e)
		{
		case Editor::LEVEL::STATIC:
			return "Static";
		case Editor::LEVEL::LOADING:
			return "Loading";
		case Editor::LEVEL::LOGO:
			return "Logo";
		case Editor::LEVEL::UI:
			return "UI";
		case Editor::LEVEL::MAP:
			return "Map";
		case Editor::LEVEL::EFFECT:
			return "Effect";
		case Editor::LEVEL::ANIMATION:
			return "Animation";
		case Editor::LEVEL::GAMEPLAY:
			return "Gameplay";
		case Editor::LEVEL::TEST:
			return "Test";
		case Editor::LEVEL::OPEN:
			return "Open";
		case Editor::LEVEL::STAGE01:
			return "Stage01";
		case Editor::LEVEL::STAGE02:
			return "Stage02";
		case Editor::LEVEL::END:
			return "End";
		default:
			return "Unknown";
		}
	}
	inline const char* ToString(int i)
	{
		switch (i)
		{
		case static_cast<unsigned int>(Editor::LEVEL::STATIC):
			return "Static";
		case static_cast<unsigned int>(Editor::LEVEL::LOADING):
			return "Loading";
		case static_cast<unsigned int>(Editor::LEVEL::LOGO):
			return "Logo";
		case static_cast<unsigned int>(Editor::LEVEL::UI):
			return "UI";
		case static_cast<unsigned int>(Editor::LEVEL::MAP):
			return "Map";
		case static_cast<unsigned int>(Editor::LEVEL::EFFECT):
			return "Effect";
		case static_cast<unsigned int>(Editor::LEVEL::ANIMATION):
			return "Animation";
		case static_cast<unsigned int>(Editor::LEVEL::GAMEPLAY):
			return "Gameplay";
		case static_cast<unsigned int>(Editor::LEVEL::TEST):
			return "Test";
		case static_cast<unsigned int>(Editor::LEVEL::OPEN):
			return "Open";
		case static_cast<unsigned int>(Editor::LEVEL::STAGE01):
			return "Stage01";
		case static_cast<unsigned int>(Editor::LEVEL::STAGE02):
			return "Stage02";
		case static_cast<unsigned int>(Editor::LEVEL::END):
			return "End";
		default:
			return "Unknown";
		}
	}

	enum class FILTERTYPE {
		PLAYER = 1 << 0,
		TERRAIN = 1 << 1,
		OBJECT = 1 << 2,
		ENEMY = 1 << 3,
	};

	enum class FILTERCATEGORY
	{
		GROUND = 1 << 0,
		WALL = 1 << 1,
		WATER = 1 << 2,
	};

	enum class FILTEROBJECT
	{
		INTERACTABLE = 1 << 0,
		STATIC = 1 << 1,
	};
}




