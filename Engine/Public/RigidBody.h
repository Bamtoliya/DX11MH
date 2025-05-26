#pragma once
#include "Component.h"
NS_BEGIN(Engine)
class ENGINE_DLL CRigidBody : public CComponent
{
	CRigidBody();
	virtual ~CRigidBody() = default;

	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);
	HRESULT Initialize(simdjson::ondemand::object& jsonData);

	static CComponent* Create();
	CComponent* Clone(void* pArg);

public:
	virtual HRESULT Serialize(json& outJson) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object& jsonData) override;
	virtual HRESULT Prototype_Serialize(json& outJson) override;
	virtual HRESULT Prototype_Deserialize(simdjson::ondemand::object& jsonData) override;



};
NS_END