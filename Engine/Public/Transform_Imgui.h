#pragma once
#include "Base.h"
#include "Transform.h"
BEGIN(Engine)
class CTransform_Imgui : public CBase
{
public:
	void Draw(void* pArg)
	{
		ImGui::Text("");
	}
	void Transform(CTransform* pTransform, void* pArg)
	{
		CTransform::TRANSFORM_DESC tDesc = *reinterpret_cast<CTransform::TRANSFORM_DESC*>(pTransform->Dump());
		ImGui::Text(to_string(tDesc.WorldMatrix._11).c_str());
	}
};
END