#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameObject;
class CGameInstance;
class CModel;

enum class EInterpolationType
{
    Linear,         // 선형 보간 (기본)
    EaseIn,         // 천천히 시작하여 빠르게 진행
    EaseOut,        // 빠르게 시작하여 천천히 종료
    EaseInOut,      // 천천히 시작하여 천천히 종료
    Cubic,          // 큐빅 스플라인 보간
    Spring,         // 스프링 효과
    Bounce,         // 바운스 효과
    Elastic,        // 탄성 효과
    Custom          // 사용자 정의 보간 함수
};

class ENGINE_DLL IBlendNode : public CBase
{
protected:
	IBlendNode(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	IBlendNode(const IBlendNode& Prototype);
	virtual ~IBlendNode() = default;
public:
    virtual IBlendNode* Clone(void* pArg) = 0;
    virtual void Free() = 0;
protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstnace = { nullptr };
public:
	virtual HRESULT Evaluate(CGameObject* pOwnerGameObject, CModel* pModel, _float fTimeDelta, const BlendParameters& params, vector<_float4x4>& outBoneTransforms);
    virtual HRESULT RenderImGui();
};
NS_END