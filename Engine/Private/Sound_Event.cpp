#pragma once
#include "Sound_Event.h"
#include "Sound_Manager.h"
#include "Transform.h"

CSound_Event::CSound_Event(CSound_Manager* pSound_Manager, _uint iID)
	: m_pSound_Manager{ pSound_Manager }
	, m_iID { iID }
{
	//Safe_AddRef(m_pSound_Manager);
}

void CSound_Event::Restart()
{
	Studio::EventInstance* event = m_pSound_Manager ? m_pSound_Manager->Get_EventInstance(m_iID) : nullptr;
	if (event)
	{
		event->start();
	}
}

void CSound_Event::Stop(bool allowFadeOut)
{
	auto event = m_pSound_Manager ? m_pSound_Manager->Get_EventInstance(m_iID) : nullptr;
	if (event)
	{
		FMOD_STUDIO_STOP_MODE mode = allowFadeOut ?
			FMOD_STUDIO_STOP_ALLOWFADEOUT :
			FMOD_STUDIO_STOP_IMMEDIATE;
		event->stop(mode);
	}
}

void CSound_Event::SetPaused(bool pause)
{
	auto event = m_pSound_Manager ? m_pSound_Manager->Get_EventInstance(m_iID) : nullptr;
	if (event)
	{
		event->setPaused(pause);
	}
}

void CSound_Event::SetVolume(float value)
{
	auto event = m_pSound_Manager ? m_pSound_Manager->Get_EventInstance(m_iID) : nullptr;
	if (event)
	{
		event->setVolume(value);
	}
}

void CSound_Event::SetPitch(float value)
{
	auto event = m_pSound_Manager ? m_pSound_Manager->Get_EventInstance(m_iID) : nullptr;
	if (event)
	{
		event->setPitch(value);
	}
}

void CSound_Event::SetParameter(const std::string& name, float value)
{
	auto event = m_pSound_Manager ? m_pSound_Manager->Get_EventInstance(m_iID) : nullptr;
	if (event)
	{
		event->setParameterByName(name.c_str(), value);
	}
}

void CSound_Event::Set3DAttributes(CTransform* worldTrans)
{
	auto event = m_pSound_Manager ? m_pSound_Manager->Get_EventInstance(m_iID) : nullptr;
	if (event)
	{
		FMOD_3D_ATTRIBUTES attr = {};
		// Set position, forward, up
		attr.position = VecToFMOD(worldTrans->Get_State(STATE::POSITION));
		//// 세계 공간에서 첫 번째 행은 전방 벡터
		attr.forward = VecToFMOD(worldTrans->Get_State(STATE::LOOK));
		//// 세 번째 행은 상향 벡터
		attr.up = VecToFMOD(worldTrans->Get_State(STATE::UP));
		// 속도를 0으로 설정 (도플러 효과를 사용한다면 수정)
		attr.velocity = { 0.0f, 0.0f, 0.0f };
		event->set3DAttributes(&attr);
	}
}

_bool CSound_Event::IsValid()
{
	return (m_pSound_Manager && m_pSound_Manager->Get_EventInstance(m_iID) != nullptr);
}

_bool CSound_Event::Is3D() const
{
	bool retVal = false;
	auto event = m_pSound_Manager ? m_pSound_Manager->Get_EventInstance(m_iID) : nullptr;
	if (event)
	{
		// 이벤트 디스크립션을 얻는다.
		FMOD::Studio::EventDescription* ed = nullptr;
		event->getDescription(&ed);
		if (ed)
		{
			ed->is3D(&retVal);
		}
	}
	return retVal;
}

_bool CSound_Event::GetPaused() const
{
	bool retVal = false;
	auto event = m_pSound_Manager ? m_pSound_Manager->Get_EventInstance(m_iID) : nullptr;
	if (event)
	{
		event->getPaused(&retVal);
	}
	return retVal;
}

_float CSound_Event::GetVolume() const
{
	float retVal = 0.0f;
	auto event = m_pSound_Manager ? m_pSound_Manager->Get_EventInstance(m_iID) : nullptr;
	if (event)
	{
		event->getVolume(&retVal);
	}
	return retVal;
}

_float CSound_Event::GetPitch() const
{
	float retVal = 0.0f;
	auto event = m_pSound_Manager ? m_pSound_Manager->Get_EventInstance(m_iID) : nullptr;
	if (event)
	{
		event->getPitch(&retVal);
	}
	return retVal;
}

_float CSound_Event::GetParameter(const std::string& name)
{
	float retVal = 0.0f;
	auto event = m_pSound_Manager ? m_pSound_Manager->Get_EventInstance(m_iID) : nullptr;
	if (event)
	{
		event->getParameterByName(name.c_str(), &retVal);
	}
	return retVal;
}

void CSound_Event::SetCallBack(FMOD_STUDIO_EVENT_CALLBACK callback, FMOD_STUDIO_EVENT_CALLBACK_TYPE callbackmask)
{
	auto event = m_pSound_Manager ? m_pSound_Manager->Get_EventInstance(m_iID) : nullptr;
	if (event)
	{
		event->setCallback(callback, callbackmask);
	}
}

void CSound_Event::Free()
{
	__super::Free();
	//Safe_Release(m_pSound_Manager);
}