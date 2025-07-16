
#include "Spline_Component.h"
#include "GameInstance.h"

#pragma region Constructor & Destructor
CSpline_Component::CSpline_Component(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CSpline_Component::CSpline_Component(const CSpline_Component& Prototype)
	: CComponent{ Prototype }
	, m_ControlPoints{ Prototype.m_ControlPoints }
	, m_tSplineDesc{Prototype.m_tSplineDesc}
	, m_bIsDirty{ Prototype.m_bIsDirty }
{
}

HRESULT CSpline_Component::Initialize_Prototype()
{
	return S_OK;
}
HRESULT CSpline_Component::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (pArg)
	{
		m_tSplineDesc = *static_cast<SPLINE_DESC*>(pArg);
	}
	return S_OK;
}
HRESULT CSpline_Component::Late_Initialize()
{
	return S_OK;
}

CSpline_Component* CSpline_Component::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpline_Component* pInstance = new CSpline_Component(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpline_Component");
		Safe_Release(pInstance);
	}
	return pInstance;
}
CSpline_Component* CSpline_Component::Clone(void* pArg)
{
	CSpline_Component* pInstance = new CSpline_Component(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CSpline_Component");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CSpline_Component::Free()
{
	__super::Free();
}
#pragma endregion

#pragma region Save&Load
HRESULT CSpline_Component::Serialize(json& data)
{
	if (FAILED(__super::Serialize(data)))
		return E_FAIL;

	// 제어 포인트 저장
	json controlPoints = json::array();
	for (const auto& point : m_ControlPoints)
	{
		json pointData;
		pointData["x"] = point.x;
		pointData["y"] = point.y;
		pointData["z"] = point.z;
		controlPoints.push_back(pointData);
	}
	data["ControlPoints"] = controlPoints;

	// 스플라인 설정 저장
	data["SplineDesc"]["NumPointsPerSegment"] = m_tSplineDesc.iNumPointsPerSegment;

	return S_OK;
}
HRESULT CSpline_Component::Deserialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Deserialize(jsonData)))
		return E_FAIL;

	// 제어 포인트 로드
	m_ControlPoints.clear();

	// simdjson 라이브러리로 ControlPoints 배열 읽기
	simdjson::ondemand::array controlPoints;
	if (jsonData["ControlPoints"].get(controlPoints) == simdjson::SUCCESS)
	{
		for (auto point : controlPoints)
		{
			_float3 newPoint;
			double x, y, z;
			if (point["x"].get(x) == simdjson::SUCCESS &&
				point["y"].get(y) == simdjson::SUCCESS &&
				point["z"].get(z) == simdjson::SUCCESS)
			{
				newPoint.x = static_cast<_float>(x);
				newPoint.y = static_cast<_float>(y);
				newPoint.z = static_cast<_float>(z);
				m_ControlPoints.push_back(newPoint);
			}
		}
	}

	// 스플라인 설정 로드
	simdjson::ondemand::object splineDesc;
	if (jsonData["SplineDesc"].get(splineDesc) == simdjson::SUCCESS)
	{
		uint64_t numPointsPerSegment;
		if (splineDesc["NumPointsPerSegment"].get(numPointsPerSegment) == simdjson::SUCCESS)
			m_tSplineDesc.iNumPointsPerSegment = static_cast<_uint>(numPointsPerSegment);
	}

	m_bIsDirty = true; // 데이터가 변경되었으므로 업데이트 필요

	return S_OK;
}
#pragma endregion

#pragma region Loops
void CSpline_Component::Priority_Update(_float fTimeDelta)
{
}
void CSpline_Component::Update(_float fTimeDelta)
{
}
void CSpline_Component::Late_Update(_float fTimeDelta)
{

}
HRESULT CSpline_Component::Render()
{
	return S_OK;
}
#pragma endregion
HRESULT CSpline_Component::RenderImGui()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Spline_Component", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool isDirty = false;

		// 스플라인 속성 편집
		_uint numPointsPerSegment = m_tSplineDesc.iNumPointsPerSegment;
		if (ImGui::SliderInt("Points Per Segment", reinterpret_cast<int*>(&numPointsPerSegment), 2, 100))
		{
			m_tSplineDesc.iNumPointsPerSegment = numPointsPerSegment;
			isDirty = true;
		}

		// 제어 포인트 표시 및 편집
		if (ImGui::TreeNode("Control Points"))
		{
			// 새 포인트 추가 기능
			static _float3 newPoint = { 0.0f, 0.0f, 0.0f };
			ImGui::DragFloat3("New Point", &newPoint.x, 0.1f);
			if (ImGui::Button("Add Point"))
			{
				AddControlPoint(newPoint);
				isDirty = true;
			}

			ImGui::Separator();

			// 기존 포인트 편집
			for (size_t i = 0; i < m_ControlPoints.size(); ++i)
			{
				ImGui::PushID(static_cast<int>(i));

				char label[32];
				sprintf_s(label, "Point %zu", i);

				if (ImGui::DragFloat3(label, &m_ControlPoints[i].x, 0.1f))
				{
					isDirty = true;
				}

				ImGui::SameLine();
				if (ImGui::Button("X"))
				{
					m_ControlPoints.erase(m_ControlPoints.begin() + i);
					isDirty = true;
					ImGui::PopID();
					break; // 삭제 후 포문 탈출 (인덱스 꼬임 방지)
				}

				ImGui::PopID();
			}

			ImGui::TreePop();
		}

		if (ImGui::Button("Clear All Points"))
		{
			m_ControlPoints.clear();
			isDirty = true;
		}

		// 변경점이 있으면 Dirty 플래그 설정
		if (isDirty)
		{
			m_bIsDirty = true;
		}
	}
	ImGui::PopID();
	return S_OK;
}

void CSpline_Component::AddControlPoint(const _float3& vPoint)
{
	m_ControlPoints.push_back(vPoint);
	m_bIsDirty = true;
}

void CSpline_Component::SetControlPoint(_uint iIndex, const _float3& vPoint)
{
	if (iIndex >= m_ControlPoints.size())
		return;

	m_ControlPoints[iIndex] = vPoint;
	m_bIsDirty = true;
}

void CSpline_Component::ClearControlPoints()
{
	m_ControlPoints.clear();
	m_bIsDirty = true;
}

void CSpline_Component::Create_CirclePath(const _vector& vCenter, _float fRadius, _uint iNumPoints)
{
	if (iNumPoints < 4) return; // 최소 4개의 점이 필요합니다.

	m_ControlPoints.clear();

	const _float fAngleStep = XM_2PI / iNumPoints;
	// 캣멀롬 스플라인이 원에 더 가깝게 생성되도록 제어점을 원보다 약간 더 큰 반경에 배치합니다.
	// 이 값(1.155f)은 경험적으로 얻어진 값이며, 필요에 따라 조정할 수 있습니다.
	const _float fControlRadius = fRadius * 1.155f;

	for (_uint i = 0; i < iNumPoints; ++i)
	{
		_float fAngle = i * fAngleStep;
		_vector vPoint = vCenter + XMVectorSet(cosf(fAngle) * fControlRadius, 0.f, sinf(fAngle) * fControlRadius, 0.f);

		_float3 vPos;
		XMStoreFloat3(&vPos, vPoint);
		m_ControlPoints.push_back(vPos);
	}

	// 스플라인 프레임을 다시 계산합니다.
	UpdateSplineFrames();
}

vector<_float3>& CSpline_Component::GetSplinePoints()
{
	m_SplinePoints.clear();
	if (m_ControlPoints.size() < 4)
	{
		// Catmull-Rom 스플라인은 최소 4개의 제어점이 필요합니다.
		return m_SplinePoints;
	}

	m_SplinePoints.reserve((m_ControlPoints.size() - 3) * m_tSplineDesc.iNumPointsPerSegment);

	for (size_t i = 0; i < m_ControlPoints.size() - 3; ++i)
	{
		const _float3& p0 = m_ControlPoints[i];
		const _float3& p1 = m_ControlPoints[i + 1];
		const _float3& p2 = m_ControlPoints[i + 2];
		const _float3& p3 = m_ControlPoints[i + 3];

		for (_uint j = 0; j < m_tSplineDesc.iNumPointsPerSegment; ++j)
		{
			float t = static_cast<float>(j) / (m_tSplineDesc.iNumPointsPerSegment - 1);
			m_SplinePoints.push_back(CatmullRom(p0, p1, p2, p3, t));
		}
	}

	return m_SplinePoints;
}

vector<CSpline_Component::SPLINE_FRAME>& CSpline_Component::Get_SplineFrames()
{
	if (m_bIsDirty)
	{
		UpdateSplineFrames();
		m_bIsDirty = false;
	}
	return m_SplineFrames;
}

_float CSpline_Component::Find_Closest_Param(const _vector& vPosition)
{
	// 스플라인 데이터가 유효한지 확인합니다.
	const auto& frames = Get_SplineFrames();
	if (frames.size() < 2)
		return 0.f;

	_float fMinDistanceSq = FLT_MAX;
	_float fClosestParam = 0.f;

	// 각 스플라인 선분을 순회하며 가장 가까운 점을 찾습니다.
	for (size_t i = 0; i < frames.size() - 1; ++i)
	{
		_vector vStartPoint = XMLoadFloat3(&frames[i].vPosition);
		_vector vEndPoint = XMLoadFloat3(&frames[i + 1].vPosition);

		_vector vSegment = vEndPoint - vStartPoint;
		_vector vToPosition = vPosition - vStartPoint;

		// 현재 위치를 선분에 투영하여 선분 상의 t 값을 계산합니다. (0.0 ~ 1.0)
		_float fSegmentLengthSq = XMVectorGetX(XMVector3LengthSq(vSegment));
		_float t = 0.f;
		if (fSegmentLengthSq > 1e-6f) // 선분 길이가 0에 가까운 경우 방지
		{
			t = XMVectorGetX(XMVector3Dot(vToPosition, vSegment)) / fSegmentLengthSq;
			t = max(0.f, min(1.f, t)); // t 값을 0과 1 사이로 제한하여 선분 내에 있도록 함
		}

		// 선분 상에서 가장 가까운 점을 계산합니다.
		_vector vClosestPointOnSegment = vStartPoint + t * vSegment;

		// 주어진 위치와 가장 가까운 점 사이의 거리를 계산합니다.
		_float fDistanceSq = XMVectorGetX(XMVector3LengthSq(vPosition - vClosestPointOnSegment));

		// 최소 거리를 갱신합니다.
		if (fDistanceSq < fMinDistanceSq)
		{
			fMinDistanceSq = fDistanceSq;

			// 가장 가까운 점에 해당하는 전체 스플라인의 파라미터(T)를 계산합니다.
			// 현재 선분의 시작점까지의 누적 길이에, 선분 내에서의 진행 거리를 더합니다.
			_float fLengthAtStart = frames[i].fTotalLength;
			_float fSegmentLength = frames[i + 1].fTotalLength - fLengthAtStart;
			_float fLengthToClosestPoint = fLengthAtStart + t * fSegmentLength;

			// 전체 길이에 대한 비율로 정규화하여 최종 파라미터를 구합니다.
			if (m_fTotalLength > 1e-6f)
				fClosestParam = fLengthToClosestPoint / m_fTotalLength;
			else
				fClosestParam = 0.f;
		}
	}

	return fClosestParam;
}

_float3 CSpline_Component::CatmullRom(const _float3& P0, const _float3& P1, const _float3& P2, const _float3& P3, _float t)
{
	_vector vP0 = XMLoadFloat3(&P0);
	_vector vP1 = XMLoadFloat3(&P1);
	_vector vP2 = XMLoadFloat3(&P2);
	_vector vP3 = XMLoadFloat3(&P3);

	_vector result = XMVectorCatmullRom(vP0, vP1, vP2, vP3, t);

	_float3 vResult;
	XMStoreFloat3(&vResult, result);
	return vResult;
}

void CSpline_Component::UpdateSplineFrames()
{
	m_SplineFrames.clear();
	m_fTotalLength = 0.f;

	if (m_ControlPoints.size() < 4)
		return;

	if (m_SplinePoints.size() < 2)
		return;

	m_SplineFrames.resize(m_SplinePoints.size());

	// 1. Tangent 계산
	for (size_t i = 0; i < m_SplinePoints.size() - 1; ++i)
	{
		m_SplineFrames[i].vPosition = m_SplinePoints[i];
		XMStoreFloat3(&m_SplineFrames[i].vTangent, XMVector3Normalize(XMLoadFloat3(&m_SplinePoints[i + 1]) - XMLoadFloat3(&m_SplinePoints[i])));
	}
	// 마지막 점의 Tangent는 그 전의 Tangent를 그대로 사용
	m_SplineFrames.back().vPosition = m_SplinePoints.back();
	m_SplineFrames.back().vTangent = m_SplineFrames[m_SplinePoints.size() - 2].vTangent;

	// 2. Normal, Binormal 계산 (간단한 버전: "Up" 벡터 사용)
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vLastNormal = vUp;

	for (size_t i = 0; i < m_SplinePoints.size(); ++i)
	{
		_vector T = XMLoadFloat3(&m_SplineFrames[i].vTangent);
		_vector B = XMVector3Normalize(XMVector3Cross(T, vLastNormal)); // Binormal
		_vector N = XMVector3Normalize(XMVector3Cross(B, T));         // Normal

		XMStoreFloat3(&m_SplineFrames[i].vBinormal, B);
		XMStoreFloat3(&m_SplineFrames[i].vNormal, N);
		vLastNormal = N; // 다음 계산을 위해 현재 Normal을 저장
	}

	// 3. 누적 길이 계산
	m_SplineFrames[0].fTotalLength = 0.f;
	for (size_t i = 1; i < m_SplinePoints.size(); ++i)
	{
		_float fDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_SplinePoints[i]) - XMLoadFloat3(&m_SplinePoints[i - 1])));
		m_SplineFrames[i].fTotalLength = m_SplineFrames[i - 1].fTotalLength + fDistance;
	}
	m_fTotalLength = m_SplineFrames.back().fTotalLength;
}