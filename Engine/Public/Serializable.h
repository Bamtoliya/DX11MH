#pragma once
#include "Engine_Defines.h"
NS_BEGIN(Engine)
class ENGINE_DLL ISerializable
{
public:
	virtual HRESULT Serialize() = 0;
	virtual HRESULT Deserialize() = 0;
};

NS_END