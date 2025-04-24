#include "Component.h"
#pragma once
class CRectTransform final : public CComponent
{
#pragma region Constructor/Destructor
protected:
	CRectTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRectTransform(const CComponent& Prototype);
	virtual ~CRectTransform() = default;

public:
	virtual CRectTransform* Clone(void* pArg) = 0;
	virtual void Free();
#pragma endregion

#pragma region Initalize
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
#pragma endregion
private:
	_float2			m_fPos;
	_float2			m_fSize;
	_float2			m_fScale;

};

