#pragma once
#include "Engine_Defines.h"
NS_BEGIN(Engine)
class ENGINE_DLL ISerializable
{
public:
	virtual HRESULT Serialize(json& data) = 0;
	virtual HRESULT Deserialize(const json& data) = 0;
	virtual HRESULT Prototype_Serialize(json& data) = 0;
	virtual HRESULT Prototype_Deserialize(simdjson::ondemand::object & jsonData) = 0;
};

NS_END