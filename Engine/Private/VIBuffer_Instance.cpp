#include "VIBuffer_Instance.h"

CVIBuffer_Instance::CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance& Prototype)
	: CVIBuffer{ Prototype }
	, m_pVBInstance { Prototype.m_pVBInstance }
	, m_VBInstanceDesc { Prototype.m_VBInstanceDesc }
	, m_VBInstanceSubresourceData{ Prototype.m_VBInstanceSubresourceData }
	, m_iNumInstance { Prototype.m_iNumInstance }
	, m_iVertexInstanceStride { Prototype.m_iVertexInstanceStride }
	, m_iNumIndexPerInstance { Prototype.m_iNumIndexPerInstance }
{
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Instance::Initialize(void* pArg)
{

	return S_OK;
}

HRESULT CVIBuffer_Instance::Bind_Buffers()
{
	ID3D11Buffer* pVertexBuffers[] = {
		   m_pVB,
		   m_pVBInstance,		   
	};

	_uint		iVertexStrides[] = {
		m_iVertexStride,
		m_iVertexInstanceStride

	};

	_uint		iOffsets[] = {
		0, 
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

HRESULT CVIBuffer_Instance::Render()
{	
	m_pContext->DrawIndexedInstanced(m_iNumIndexPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Instance::RenderImGui()
{
    ImGui::PushID(this);
    if (ImGui::CollapsingHeader("VIBuffer_Instance", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // 인스턴스 개수 표시 및 편집
        _uint iNewInstanceCount = m_iNumInstance;
        if (ImGui::InputInt("Instance Count", (int*)&iNewInstanceCount, 1, 10))
        {
            // 인스턴스 개수가 변경되면 최소 1개 이상 유지
            iNewInstanceCount = max(1, iNewInstanceCount);

            // 파생 클래스에서 구현할 리사이징 메서드 호출
            if (iNewInstanceCount != m_iNumInstance) {
                if (FAILED(Resize(iNewInstanceCount))) {
                    ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Failed to resize instance buffer!");
                }
            }
        }

        // 인스턴스 버퍼 정보 표시
        ImGui::Text("Instance Buffer: %p", m_pVBInstance);
        ImGui::Text("Instance Stride: %d bytes", m_iVertexInstanceStride);
        ImGui::Text("Indices Per Instance: %d", m_iNumIndexPerInstance);

        // 인스턴스 위치 편집 UI
        if (ImGui::TreeNode("Instance Transforms"))
        {
            // 스크롤 가능한 영역 생성 (많은 인스턴스가 있을 경우를 대비)
            ImGui::BeginChild("InstanceList", ImVec2(0, 300), true);

            // 현재 선택된 인스턴스 추적
            static int selectedInstance = -1;

            // 인스턴스 목록 표시
            for (int i = 0; i < (int)m_iNumInstance; i++)
            {
                char label[32];
                sprintf_s(label, "Instance %d", i);
                if (ImGui::Selectable(label, selectedInstance == i))
                    selectedInstance = i;

                // 클릭해서 상세 편집 가능하도록
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                    ImGui::OpenPopup(label);

                // 개별 인스턴스 편집 팝업
                if (ImGui::BeginPopup(label))
                {
                    ImGui::Text("Edit Instance %d", i);
                    ImGui::Separator();

                    // 파생 클래스에서 정의할 가상 메서드 호출
                    EditInstance(i);

                    ImGui::EndPopup();
                }
            }
            ImGui::EndChild();
            ImGui::TreePop();
        }

        // 변경 내용 적용 버튼
        if (ImGui::Button("Apply Changes"))
        {
            // 파생 클래스에서 정의할 변경 사항 적용 메서드
            ApplyChanges();
        }

        ImGui::SameLine();

        // 기본값으로 재설정 버튼
        if (ImGui::Button("Reset"))
        {
            // 파생 클래스에서 정의할 재설정 메서드
            ResetInstances();
        }
    }
    ImGui::PopID();
    return S_OK;
}

void CVIBuffer_Instance::Drop(_float fTimeDelta)
{
	

}

void CVIBuffer_Instance::Spread(_float fTimeDelta)
{
}

void CVIBuffer_Instance::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);

}
