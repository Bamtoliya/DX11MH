#pragma once
#include "Editor_Defines.h"
#include "MyImGui.h"
NS_BEGIN(Engine)
class CTexture;
class CGameObject;
NS_END
NS_BEGIN(Editor)
class CViewport_2D final : public CMyImGui
{
#pragma region Constructor/Destructor
private:
	CViewport_2D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CViewport_2D() = default;
public:
	static CViewport_2D* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
#pragma endregion

public:
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
private:

	HRESULT DrawGrid(ImDrawList* pList, ImVec2& p0, ImVec2& p1, ImVec2& center, _float fZoom);
	
	HRESULT DrawWidget();
	HRESULT DrawResolutionRect(ImDrawList* pList, ImVec2& center, _float fZoom);
	HRESULT DrawTexture(ImDrawList* pList, ImVec2& center, _float fZoom);
	HRESULT DrawImGuizmo2D(_float fZoom, ImVec2& vPanning);


private:
	wstring				m_strRenderTargetKey = {};
	// 이미지 렌더링을 위한 멤버 변수 추가
	ID3D11ShaderResourceView* m_pSRV = nullptr; // 실제 렌더링에 사용할 SRV

	_float2 m_vTargetResolution = { g_iWinSizeX, g_iWinSizeY };
	_float2 m_vImageGridPosition = { 0.f, 0.f };      // 그리드 좌표계 기준 이미지 위치 (예: 좌상단)
	_float2 m_vImageGridSize = { 1920.f, 1080.f };    // 그리드 좌표계 기준 이미지 크기
	bool    m_bShowImage = true;                    // 이미지 표시 여부 플래그 (선택적)

	// 그리드 좌표를 화면 좌표로 변환하는 헬퍼 함수 (선택적)
	ImVec2 GridToScreen(const ImVec2& gridPos, const ImVec2& canvas_center, float zoom);

	//Redering Grid Variables
private:
	const float zoom_speed = 0.1f;
	const float min_zoom = 0.01f;
	const float max_zoom = 10.0f;
private:
private: //IMGUIZMO Variable
	ImGuizmo::OPERATION                 m_currentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE                      m_currentGizmoMode = ImGuizmo::WORLD;
	bool                                m_bUseSnap = true;
	float                               m_fSnapValue[3] = { 0.1f, 0.1f, 0.1f };
};
NS_END
