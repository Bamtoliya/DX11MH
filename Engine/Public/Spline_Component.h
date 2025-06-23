#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class ENGINE_DLL CSpline_Component : public CComponent
{
public:
	typedef struct tagSplineDesc
	{
		// 한 제어점 구간을 몇 개의 정점으로 나눌지 결정합니다. 높을수록 곡선이 부드러워집니다.
		_uint iNumPointsPerSegment = 20;
	} SPLINE_DESC;
#pragma region Constructor/Destructor
protected:
	CSpline_Component(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpline_Component(const CSpline_Component& Prototype);
	virtual ~CSpline_Component() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize();
public:
	static CSpline_Component* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSpline_Component* Clone(void* pArg)  override;
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
public:
	virtual HRESULT RenderImGui() override;
public:
	// 제어점 관리
	void AddControlPoint(const _float3& vPoint);
	void SetControlPoint(_uint iIndex, const _float3& vPoint);
	const vector<_float3>& GetControlPoints() const { return m_ControlPoints; }
	void ClearControlPoints();

	// 스플라인 곡선 위의 점들을 계산
	vector<_float3> GetSplinePoints();

	// 제어점이 변경되었는지 확인
	_bool IsDirty() const { return m_bIsDirty; }
	void SetDirty(_bool bDirty) { m_bIsDirty = bDirty; }
private:
	// Catmull-Rom 보간 함수
	_float3 CatmullRom(const _float3& P0, const _float3& P1, const _float3& P2, const _float3& P3, _float t);

public:
	// 계산된 프레임과 총 길이를 반환하는 함수
	const vector<VTXSPLINEMODEL>& GetSplineFrames() const { return m_SplinePoints; }
	_float GetTotalLength() const { return m_fTotalLength; }
	void UpdateSplineFrames(); // 프레임 계산을 위한 함수

private:
	vector<_float3> m_ControlPoints;
	vector<VTXSPLINEMODEL> m_SplinePoints;
	_float			m_fTotalLength = { 0.f };
	SPLINE_DESC     m_tSplineDesc;
	_bool           m_bIsDirty = { true }; // 제어점이 변경되면 true로 설정
};

NS_END

