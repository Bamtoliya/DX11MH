
#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CState;
class CBone;
class CAnimation;
class ENGINE_DLL CAnimator : public CComponent
{
#pragma region Constructor/Destructor
protected:
	CAnimator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnimator(const CAnimator& Prototype);
	virtual ~CAnimator() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize();
public:
	static CAnimator* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CAnimator* Clone(void* pArg)  override;
	virtual void Free() override;
public:
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object& jsonData) override;
#pragma endregion

#pragma region Loops
public:
	virtual void Priority_Update(_float fTimeDelta)  override;
	virtual void Update(_float fTimeDelta)  override;
	virtual void Late_Update(_float fTimeDelta)  override;
	virtual HRESULT Render() override;
#pragma endregion

#pragma region Animation Control
public:
	_bool Play_Animation(_float fTimeDelta);
	void Change_Animation(_uint animationIndex, _bool bLoop = true, _float fPlaybackSpeed = 1.0f, _float fBlendDuration = 0.0f);
	const vector<_float4x4>& GetFinalBoneTransforms() const { return m_FinalBoneTransforms; }

private:
	void UpdateAnimationTimes(_float fTimeDelta);

#pragma endregion

#pragma region Variables
private:	
	_uint			m_iCurStateIndex = {};
	CAnimation*		m_pCurAnimation = { nullptr };
	_bool			m_bCurAnimLoop = { false };
	_float			m_fCurAnimPlaybackSpeed = { 1.0f };
	_float			m_fCurTrackPosition = { 0.f };

	_uint			m_iPreStateIndex = {};
	CAnimation*		m_pPreAnimation = { nullptr };
	_bool			m_bPreAnimLoop = { false };
	_float			m_fPreAnimPlaybackSpeed = { 1.0f };
	_float			m_fPreTrackPosition = { 0.f };


	_bool			m_bBlend = { false };
	_float			m_fBlendDuration = { 0.1f };
	_float			m_fBlendElapsedTime = { 0.f };

	// 최종 뼈 변환 행렬 (스키닝 매트릭스)
	vector<_float4x4> m_FinalBoneTransforms;
	// 중간 계산용 변환 행렬
	vector<_float4x4> m_CurAnimSnapshotTransforms = {};
	vector<_float4x4> m_PreAnimSnapshotTransforms = {};
	_float			m_fDelayTime = { 0.0f };
	_float			m_fElapsedTime = { 0.0f };
#pragma endregion

public:
	virtual HRESULT RenderImGui() override;
};
NS_END


