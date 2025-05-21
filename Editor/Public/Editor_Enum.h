#pragma once


namespace Editor
{
	enum class LEVEL { STATIC, LOADING, LOGO, UI, MAP, EFFECT, GAMEPLAY, END };
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
		case Editor::LEVEL::GAMEPLAY:
			return "Gameplay";
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
		case static_cast<unsigned int>(Editor::LEVEL::GAMEPLAY):
			return "Gameplay";
		case static_cast<unsigned int>(Editor::LEVEL::END):
			return "End";
		default:
			return "Unknown";
		}
	}
}




