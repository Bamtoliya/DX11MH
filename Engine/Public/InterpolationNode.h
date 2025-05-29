#pragma once
#include "BlendNode.h"
NS_BEGIN(Engine)
class ENGINE_DLL CInterpolationNode : public IBlendNode
{
#pragma region Constructor&Destructor
private:
	CInterpolationNode(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInterpolationNode(const CInterpolationNode& Protottype);
	virtual ~CInterpolationNode() = default;
public:
	static CInterpolationNode* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CInterpolationNode* Clone(void* pArg) override;
private:
	HRESULT Intialize_Prototype();
	HRESULT Intialize(void* pArg);
public:
	virtual void Free() override;
#pragma endregion

};

NS_END