#pragma once

#include "Base.h"

NS_BEGIN(Engine)
class CLight;
class CLight_Manager final : public CBase
{
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHT_DESC* Get_Light(_uint iIndex);

public:
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Add_Light(CLight* pLight);

private:
	list<CLight*>				m_Lights;
	

public:
	static CLight_Manager* Create();
	virtual void Free() override;
};

NS_END