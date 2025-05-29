
#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CState;
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
	// 애니메이션 재생 요청 (블렌딩 옵션 포함)
	// fBlendDuration이 0보다 크면 이전 애니메이션과 블렌딩
	void PlayAnimation(_uint animationIndex, _bool bLoop = true, _float fPlaybackSpeed = 1.0f, _float fBlendDuration = 0.0f);
	const vector<_float4x4>& GetFinalBoneTransforms() const { return m_FinalBoneTransforms; }

private:
	void UpdateAnimationTimes(_float fTimeDelta);
	void CalculateBlendedTransforms();
	void CalculateSingleAnimationTransforms(CAnimation* pAnimation, _float fAnimationTime, _bool bIsLoop, vector<_float4x4>& outTransforms);

#pragma endregion

public:
	void Set_NextState(_uint iIndex, _bool bBlend = false) { m_iNextStateIndex = iIndex; };
	void Resize_States(_uint iSize) { m_StateLists.resize(iSize); }
	void Add_State(_uint iIndex, CState* pState);

#pragma region Variables
private:
	vector<CState*> m_StateLists;

	CState* m_pCurState = { nullptr };
	_uint			m_iCurStateIndex = {};
	CAnimation* m_pCurAnimation = { nullptr };
	_float			m_fCurAnimTime = { 0.f };
	_float			m_fCurAnimPlaybackSpeed = { 1.0f };
	_float			m_bCurAnimLoop = { true };



	CState* m_pNextState = { nullptr };
	_uint			m_iNextStateIndex = {};
	CAnimation* m_pNextAnimation = { nullptr };
	_float			m_fNextAnimTime = { 0.f };
	_float			m_fNextAnimPlaybackSpeed = { 1.0f };
	_float			m_bNextAnimLoop = { true };

	_bool			m_bBlend = { false };
	_float			m_fBlendDuration = { 0.3f };
	_float			m_fBlendElapsedTime = { 0.f };

	// 최종 뼈 변환 행렬 (스키닝 매트릭스)
	vector<_float4x4> m_FinalBoneTransforms;
	// 중간 계산용 변환 행렬
	vector<_float4x4> m_CurAnimSnapshotTransforms;
	vector<_float4x4> m_NextAnimSnapshotTransforms;
	_float			m_fDelayTime = { 0.2f };
	_float			m_fElapsedTime = { 0.0f };
#pragma endregion

public:
	virtual HRESULT RenderImGui() override;
};
NS_END


