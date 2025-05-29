#pragma once
#include "BlendNode.h"
NS_BEGIN(Engine)
class ENGINE_DLL CAnimationNode final : public IBlendNode
{
#pragma region Constructor&Destructor
private:
	CAnimationNode(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnimationNode(const CAnimationNode& Protottype);
	virtual ~CAnimationNode() = default;
public:
	static CAnimationNode* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CAnimationNode* Clone(void* pArg) override;
private:
	HRESULT Intialize_Prototype();
	HRESULT Intialize(void* pArg);
public:
	virtual void Free() override;
#pragma endregion

public:
	void Set_AnimationIndex(_uint iAnimationIndex) { m_iAnimationIndex = iAnimationIndex; m_fCurrentTime = 0.f; }
	_uint Get_AnimationIndex() const { return m_iAnimationIndex; }
	void Set_Loop(_bool bIsLoop) { m_bIsLoop = bIsLoop; }
	_bool IsLoop() const { return m_bIsLoop; }
	void Set_PlaybackSpeed(_float fSpeed) { m_fPlaybackSpeed = fSpeed; }
	_float Get_PlaybackSpeed() const { return m_fPlaybackSpeed; }
	_float Get_CurrentTime() const { return m_fCurrentTime; }
	void Reset_Time() { m_fCurrentTime = 0.f; }

private:
	_uint m_iAnimationIndex = { 0 };
	_bool m_bIsLoop = { true };
	_float m_fCurrentTime = {};
	_float m_fPlaybackSpeed = {};

};

NS_END