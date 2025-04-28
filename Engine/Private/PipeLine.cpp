#include "PipeLine.h"
#pragma region Constructor/Destructor
CPipeLine::CPipeLine()
{
}

CPipeLine* CPipeLine::Create()
{
	return new CPipeLine;
}

void CPipeLine::Free()
{
	__super::Free();
}
#pragma endregion

void CPipeLine::Update()
{
	for (size_t i = 0; i < ENUM_CAST(TRANSFORMSTATE::D3DTS_END); i++)
	{
		XMStoreFloat4x4(&m_TransformationMatrix_Inverse[i],
			XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformationMatrix[i])));
	}

	memcpy(&m_vCamPosition, &m_TransformationMatrix_Inverse[ENUM_CAST(TRANSFORMSTATE::D3DTS_VIEW)].m[3][0], sizeof(_float4));

}