
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

vector<_float3> CSpline_Component::GetSplinePoints()
{
	vector<_float3> splinePoints;
	if (m_ControlPoints.size() < 4)
	{
		// Catmull-Rom 스플라인은 최소 4개의 제어점이 필요합니다.
		return splinePoints;
	}

	splinePoints.reserve((m_ControlPoints.size() - 3) * m_tSplineDesc.iNumPointsPerSegment);

	for (size_t i = 0; i < m_ControlPoints.size() - 3; ++i)
	{
		const _float3& p0 = m_ControlPoints[i];
		const _float3& p1 = m_ControlPoints[i + 1];
		const _float3& p2 = m_ControlPoints[i + 2];
		const _float3& p3 = m_ControlPoints[i + 3];

		for (_uint j = 0; j < m_tSplineDesc.iNumPointsPerSegment; ++j)
		{
			float t = static_cast<float>(j) / (m_tSplineDesc.iNumPointsPerSegment - 1);
			splinePoints.push_back(CatmullRom(p0, p1, p2, p3, t));
		}
	}

	return splinePoints;
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

