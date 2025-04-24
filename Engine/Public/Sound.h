#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CSound final : public CBase
{
private:
	CSound();
	virtual ~CSound() = default;
public:
	void Update();
private:
	FMOD::Sound*		m_Sound;

public:
	static CSound* Create();
	virtual void Free() override;
};

END