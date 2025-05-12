#include "Picking_System.h"
#include "GameInstance.h"
#include "RenderTarget.h"
#include "GameObject.h"

#pragma region Constructor/Destructor
CPicking_System::CPicking_System(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

CPicking_System* CPicking_System::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPicking_System* pInstance = new CPicking_System(pDevice, pContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPicking_System");
		return nullptr;
	}
	return pInstance;
}

void CPicking_System::Free()
{
	__super::Free();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
#pragma endregion

HRESULT CPicking_System::ReadPickingTargetPixel(_int screenX, _int screenY, _float4* pPickedColor)
{
	CRenderTarget* pRT = m_pGameInstance->Get_RenderTarget(L"RT_Picking");
	ID3D11Texture2D* pTexture = pRT->Get_Texture();
	if (pTexture == nullptr) return E_FAIL;

	D3D11_TEXTURE2D_DESC pickingTexDesc = {};

	pTexture->GetDesc(&pickingTexDesc);

	// 1. Staging Texture 생성

	ID3D11Texture2D* pStagingTexture = nullptr;

	D3D11_TEXTURE2D_DESC stagingDesc = pickingTexDesc;

	stagingDesc.Usage = D3D11_USAGE_STAGING;

	stagingDesc.BindFlags = 0;

	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	stagingDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&stagingDesc, nullptr, &pStagingTexture)))
		return E_FAIL;
	// 2. Picking Render Target 내용을 Staging Texture로 복사

	m_pContext->CopyResource(pStagingTexture, pTexture);

	// 3. Staging Texture 맵핑하여 픽셀 데이터 읽기

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	HRESULT hr = m_pContext->Map(pStagingTexture, 0, D3D11_MAP_READ, 0, &mappedResource);

	if (SUCCEEDED(hr))
	{
		const BYTE* pPixels = reinterpret_cast<const BYTE*>(mappedResource.pData);

		// 주의: screenX, screenY가 텍스처 범위 내에 있는지 확인 필요

		if (screenX >= 0 && screenX < (int)pickingTexDesc.Width && screenY >= 0 && screenY < (int)pickingTexDesc.Height)
		{
			// DXGI_FORMAT_R8G8B8A8_UNORM 가정 (픽셀 당 4바이트)

			UINT pixelOffset = (screenY * mappedResource.RowPitch) + (screenX * 4);

			BYTE r = pPixels[pixelOffset + 0];

			BYTE g = pPixels[pixelOffset + 1];

			BYTE b = pPixels[pixelOffset + 2];
			// BYTE a = pPixels[pixelOffset + 3]; // 알파는 보통 ID에 사용 안 함

			pPickedColor->x = (float)r / 255.0f;

			pPickedColor->y = (float)g / 255.0f;

			pPickedColor->z = (float)b / 255.0f;

			pPickedColor->w = 1.0f; // 또는 (float)a / 255.0f;
		}
		else
		{			
			hr = E_INVALIDARG;
			pPickedColor = nullptr;
		}
		m_pContext->Unmap(pStagingTexture, 0);
	}

	Safe_Release(pStagingTexture);
	return S_OK;
}

HRESULT CPicking_System::PickObjectById(_uint iId)
{
	if (m_pPickedObject != nullptr)
		m_pPickedObject->Set_Picked(false);
	m_pPickedObject = m_pGameInstance->Get_GameObjectById(iId);
	if (m_pPickedObject != nullptr)
		m_pPickedObject->Set_Picked(true);
	if (m_pPickedObject == nullptr)
		return E_FAIL;
	return S_OK;
}

HRESULT CPicking_System::PickObjectByColor(_float4* pColor)
{
	if (m_pPickedObject != nullptr)
		m_pPickedObject->Set_Picked(false);
	m_pPickedObject = m_pGameInstance->Get_GameObjectById(ConvertColorToId(*pColor));
	if(m_pPickedObject != nullptr)
		m_pPickedObject->Set_Picked(true);
	if (m_pPickedObject == nullptr)
		return E_FAIL;
	return S_OK;
}

HRESULT CPicking_System::Initialize()
{
	return S_OK;
}

void CPicking_System::Priority_Update(_float fTimeDelta)
{

}

void CPicking_System::Update(_float fTimeDelta)
{
	if (MOUSE_DOWN(DIM::LBUTTON))
	{
		POINT cursorPos;
		if (GetCursorPos(&cursorPos))
		{
			_float4 testColor = { 1.0f, 1.0f, 1.0f, 1.0f };
			if (FAILED(ReadPickingTargetPixel(cursorPos.x, cursorPos.y, &testColor)))
				return;
			int a = 10;
		}
	}
}

void CPicking_System::Late_Update(_float fTimeDelta)
{

}
