#pragma once
#include "Editor_Defines.h"
#include "MyImGui.h"
NS_BEGIN(Engine)
class CGameObject;
NS_END
NS_BEGIN(Editor)
class CViewport_3D final : public CMyImGui
{
#pragma region Constructor/Destructor
private:
	CViewport_3D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CViewport_3D() = default;
public:
	static CViewport_3D* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
#pragma endregion

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	ID3D11ShaderResourceView* m_pSRV = nullptr; // 실제 렌더링에 사용할 SRV

private:
	HRESULT DrawGrid(ImVec2& p0, ImVec2& size, _float fZoom);
	HRESULT DrawTexture(ImDrawList* pList, const _wstring& strRTKey, ImVec2& center, _float fZoom);
	HRESULT DrawImGuizmo3D(_float fZoom, ImVec2& vPanning);

	wstring			m_strRenderTargetKey = {};

private:
	_float2 m_vImageGridPosition = { 0.f, 0.f };      // 그리드 좌표계 기준 이미지 위치 (예: 좌상단)
	_float2 m_vImageGridSize = { 1920.f, 1080.f };    // 그리드 좌표계 기준 이미지 크기
	bool    m_bShowImage = true;
	// 그리드 좌표를 화면 좌표로 변환하는 헬퍼 함수 (선택적)
	ImVec2 GridToScreen(const ImVec2& gridPos, const ImVec2& canvas_center, float zoom);

private: //IMGUIZMO Variable
	ImGuizmo::OPERATION                 m_currentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE                      m_currentGizmoMode = ImGuizmo::WORLD;
	bool                                m_bUseSnap = true;
	float                               m_fSnapValue[3] = { 0.1f, 0.1f, 0.1f };
};
NS_END