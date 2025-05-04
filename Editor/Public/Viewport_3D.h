#pragma once
#include "Editor_Defines.h"
#include "MyImGui.h"
BEGIN(Engine)
class CRenderTarget;
END
BEGIN(Editor)
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
	// Render Target 관련 멤버
	CRenderTarget*	 m_pViewportRT = nullptr; // 뷰포트 렌더링 결과를 담을 RenderTarget
	std::wstring     m_strRenderTargetTag = L"Viewport3D_RT"; // TargetManager에서 사용할 태그
	_uint            m_iRenderTargetWidth = 0;  // 현재 렌더 타겟 너비
	_uint            m_iRenderTargetHeight = 0; // 현재 렌더 타겟 높이
	bool             m_bRenderTargetDirty = true; // RT 재생성 필요 여부 플래그

	// 뷰포트 크기에 맞춰 RenderTarget을 생성/조정하는 함수
	HRESULT EnsureRenderTargetSize(UINT width, UINT height);

	// 실제 3D 씬을 렌더 타겟에 그리는 함수 (구현 필요)
	void RenderSceneToRenderTarget();

	// ImGui 창 내 좌표로 그리드 그리는 함수 (수정 필요)
	void DrawGridInWindow(ImDrawList* draw_list, const ImVec2& canvas_origin, const ImVec2& canvas_size, const ImVec2& canvas_center, float zoom);
};
END

