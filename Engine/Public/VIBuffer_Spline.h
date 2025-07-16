
#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Spline : public CVIBuffer
{
#pragma region Constructor/Destructor
protected:
	CVIBuffer_Spline(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Spline(const CVIBuffer_Spline& Prototype);
	virtual ~CVIBuffer_Spline() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize() override;
public:
	static CVIBuffer_Spline* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CVIBuffer_Spline* Clone(void* pArg)  override;
	virtual void Free() override;
public:
	virtual HRESULT Serialize(json& data) override;
	virtual HRESULT Deserialize(simdjson::ondemand::object& jsonData) override;
	virtual HRESULT Prototype_Serialize(json& data) override;
	virtual HRESULT Prototype_Deserialize(simdjson::ondemand::object& jsonData) override;
#pragma endregion

#pragma region Loops
public:
	// 스플라인 점들이 변경되었을 때 버퍼를 업데이트하는 함수
	HRESULT Update_Spline(const vector<_float3>& vPoints);
	HRESULT Update_Spline(const vector<CSpline_Component::SPLINE_FRAME>& vSplineFrames);
	virtual HRESULT Render() override;

#pragma endregion
public:
	virtual HRESULT RenderImGui() override;
};
NS_END
