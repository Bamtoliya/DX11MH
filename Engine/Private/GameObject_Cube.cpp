#include "GameObject_Cube.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Cube.h"
#include "GameInstance.h"

CGameObject_Cube::CGameObject_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject { pDevice, pContext }
{
}

CGameObject_Cube::CGameObject_Cube(const CGameObject_Cube& Prototype)
	: CGameObject { Prototype }
{
}

CGameObject_Cube* CGameObject_Cube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameObject_Cube* pInstance = new CGameObject_Cube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGameObject_Cube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject_Cube* CGameObject_Cube::Clone(void* pArg)
{
	CGameObject_Cube* pInstance = new CGameObject_Cube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGameObject_Cube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject_Cube* CGameObject_Cube::Clone(simdjson::ondemand::object& jsonData)
{
	CGameObject_Cube* pInstance = new CGameObject_Cube(*this);

	if (FAILED(pInstance->Initialize(jsonData)))
	{
		MSG_BOX("Failed to Cloned : CGameObject_Cube");
		Safe_Release(pInstance);
	}

	return pInstance;
}



HRESULT CGameObject_Cube::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CGameObject_Cube::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			Desc{};
	lstrcpy(Desc.szName, TEXT("Cube"));
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	return S_OK;
}

HRESULT CGameObject_Cube::Initialize(simdjson::ondemand::object& jsonData)
{
	if (FAILED(__super::Initialize(jsonData)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Deserialize(jsonData);
	return S_OK;
}

HRESULT CGameObject_Cube::Ready_Components()
{
	/* For.Com_Texture */
	m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, L"../../Assets/Textures/Skybox/Sky_0.dds", 1);
	m_Components.emplace(L"Com_Texture", m_pTextureCom);
	Safe_AddRef(m_pTextureCom);
	/* For.Com_VIBuffer */
	m_pVIBufferCom = CVIBuffer_Cube::Create(m_pDevice, m_pContext);
	m_Components.emplace(L"Com_VIBuffer", m_pVIBufferCom);
	Safe_AddRef(m_pVIBufferCom);

	//여기서 만드는게아니라 나중엔 프로토타입에서 찾아서 대체해주기
	/* For.Com_Shader */
	m_pShaderCom = CShader::Create(m_pDevice, m_pContext, L"../../Assets/ShaderFiles/Shader_VtxCubeTex.hlsl", VTXCUBETEX::Elements, VTXCUBETEX::iNumElements, "VTXCUBETEX");
	m_Components.emplace(L"Com_Shader", m_pShaderCom);
	Safe_AddRef(m_pShaderCom);
	/*if (FAILED(__super::Add_Component(ENUM_CLASS(0), TEXT("Prototype_Component_Shader_Cube"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;*/
	return S_OK;
}

void CGameObject_Cube::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}

void CGameObject_Cube::Priority_Update(_float fTimeDelta)
{
}

void CGameObject_Cube::Update(_float fTimeDelta)
{

}

void CGameObject_Cube::Late_Update(_float fTimeDelta)
{
}

HRESULT CGameObject_Cube::Pre_Render()
{
	return S_OK;
}

HRESULT CGameObject_Cube::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	/* dx9 : 장치에 뷰, 투영행렬을 저장해두면 렌더링시 알아서 정점에 Transform해주었다. */
	/* dx11 : 셰이더에 뷰, 투영행렬을 저장해두고 우리가 직접 변환해주어야한다. */
	if (FAILED(m_pShaderCom->Bind_RawValue("objectId", &m_oID, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_TextureCube", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject_Cube::Post_Render()
{
	return S_OK;
}
