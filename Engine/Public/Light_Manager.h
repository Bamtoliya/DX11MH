#pragma once

#include "Base.h"

NS_BEGIN(Engine)
class CLight;
class CLight_Manager final : public CBase
{
#pragma region Constructor/Destructor
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;
public:
	static CLight_Manager* Create();
	virtual void Free() override;
#pragma endregion
#pragma region Variables
private:
	list<CLight*>				m_Lights;
#pragma endregion

public:
	void Clear();
	void Update();
public:
	const LIGHT_DESC* Get_Light(_uint iIndex);
public:
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Add_Light(CLight* pLight);


	

};

NS_END