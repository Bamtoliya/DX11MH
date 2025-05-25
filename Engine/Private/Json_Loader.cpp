#include "Json_Loader.h"
#include "GameInstance.h"
#include "Components.h"
#include "Base_Objects.h"

using namespace simdjson;
using namespace simdjson::ondemand;

CJson_Loader::CJson_Loader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

CJson_Loader* CJson_Loader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CJson_Loader* pInstance = new CJson_Loader(pDevice, pContext);
	if(FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CJson_Loader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CJson_Loader::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

template<typename T>
void CJson_Loader::RegisterBaseCreator(const std::string& className) {
    m_PrototypeCreators[className] =
        [this](ID3D11Device* pDevice, ID3D11DeviceContext* pContext) -> CBase* {
        // T::Create(pDevice, pContext)가 존재하고, 이것이 '원시' 객체 생성에 적합하다고 가정
        // 또는 new T(pDevice, pContext) 사용
        // CShader, CModel 등은 Create 시그니처가 복잡하므로, 이 템플릿은
        // CCamera, CLight, CVIBuffer_Rect 등 간단한 Create를 가진 경우에 적합.
        // 또는 모든 클래스가 Prototype_Deserialize_Json에서 모든 초기화를 수행하도록 수정.
        // 여기서는 T가 (pDevice, pContext)를 받는 생성자 또는 Create 정적 함수가 있다고 가정.
        // 가장 좋은 방법은 각 클래스가 프로토타입용 기본 생성자를 제공하고,
        // Prototype_Deserialize_Json에서 모든 것을 설정하는 것입니다.
        // 예: return new T(pDevice, pContext);
        // 또는, T::Create_ForPrototype() 같은 정적 함수를 만들어 호출
        CBase* pInstance = T::Create(pDevice, pContext); // 이 Create는 최소한의 생성만 해야 함
        if (!pInstance) {            
            MSG_BOX("Failed to create raw instance for: ");
        }
        return pInstance;
        };
}

HRESULT CJson_Loader::Initialize() {

#pragma region Components
    RegisterBaseCreator<CCamera>("CCamera");
    RegisterBaseCreator<CLight>("CLight");
    RegisterBaseCreator<CModel>("CModel");
    RegisterBaseCreator<CShader>("CShader");
    RegisterBaseCreator<CTexture>("CTexture");
    RegisterBaseCreator<CVIBuffer_Rect>("CVIBuffer_Rect");
    RegisterBaseCreator<CVIBuffer_Rect>("CVIBuffer_Cube");
    RegisterBaseCreator<CVIBuffer_Terrain>("CVIBuffer_Terrain");
#pragma endregion

#pragma region Base_GameObjects
    RegisterBaseCreator<CLight_Point>("CLight_Point");
    RegisterBaseCreator<CLight_Direction>("CLight_Direction");
    RegisterBaseCreator<CGameObject_Skybox>("CGameObject_Skybox");
#pragma endregion

    return S_OK;
}

HRESULT CJson_Loader::Register_Creator_Function(const string& className, PrototypeCreatorFunc creatorFunc)
{
    if (m_PrototypeCreators.find(className) != m_PrototypeCreators.end())
    {
        return E_FAIL;
	}
	m_PrototypeCreators[className] = creatorFunc;
    return S_OK;
}

HRESULT CJson_Loader::Load_Prototypes(_uint iLevelIndex, const _tchar* pFilePath)
{
    parser parser;
    simdjson::padded_string json_file_data;
    auto load_error = simdjson::padded_string::load(WideToUtf8(pFilePath).c_str()).get(json_file_data);
    if (load_error) {
        // MSG_BOX("Failed to load JSON file in CJson_Loader: " + narrowFilePath);
        return E_FAIL;
    }
    simdjson::ondemand::document doc;
    auto error = parser.iterate(json_file_data).get(doc);
    if (error) {
        // MSG_BOX("Failed to parse JSON data in CJson_Loader");
        return E_FAIL;
    }
    simdjson::ondemand::object root_obj;
    error = doc.get_object().get(root_obj);
    if (error) {
        MSG_BOX("JSON root is not an object.");
        return E_FAIL;
    }

    for (simdjson::ondemand::field field : root_obj) {
        std::string_view tag_sv = field.escaped_key();
        std::wstring prototype_tag = Utf8ToWide(std::string(tag_sv));

        simdjson::ondemand::value prototype_value = field.value();
        simdjson::ondemand::object prototype_data_obj;
        if (prototype_value.get_object().get(prototype_data_obj) != simdjson::SUCCESS) {
            //std::string msg = "Value for prototype_tag '" + std::string(tag_sv) + "' is not an object.";
            //MSG_BOX(msg.c_str());
            continue;
        }

        std::string_view class_name_sv;
        if (prototype_data_obj["Class"].get_string().get(class_name_sv) != simdjson::SUCCESS) {
            //std::string msg = "Prototype '" + std::string(tag_sv) + "' missing 'Class' field or it's not a string.";
            //MSG_BOX(msg.c_str());
            continue;
        }
        std::string class_name(class_name_sv);

        auto it = m_PrototypeCreators.find(class_name);
        if (it == m_PrototypeCreators.end()) {
            //std::string msg = "No creator registered for class: " + class_name;
            //MSG_BOX(msg.c_str());
            continue;
        }

        CBase* pPrototype = it->second(m_pDevice, m_pContext);
        if (!pPrototype) {
            //std::string msg = "Failed to create prototype instance for: " + class_name;
            //MSG_BOX(msg.c_str());
            continue;
        }

        // Prototype_Deserialize를 호출하여 JSON 데이터로부터 객체 초기화
        if (FAILED(pPrototype->Prototype_Deserialize(prototype_data_obj))) {
            //std::string msg = "Failed to deserialize prototype: " + class_name + " with tag: " + std::string(tag_sv);
            //MSG_BOX(msg.c_str());
            Safe_Release(pPrototype);
            continue;
        }

        // Prototype_Manager에 프로토타입 추가
        // Add_Prototype은 성공 시 pPrototype의 소유권을 가져간다고 가정합니다.
        if (FAILED(m_pGameInstance->Add_Prototype(iLevelIndex, prototype_tag, pPrototype))) {
            //std::string msg = "Failed to add prototype to manager: " + class_name + " with tag: " + std::string(tag_sv);
            //MSG_BOX(msg.c_str());
            Safe_Release(pPrototype); // Add_Prototype 실패 시 여기서 해제
            continue;
        }
        // 성공 시 pPrototype은 Prototype_Manager가 관리하므로 여기서 Safe_Release(pPrototype)를 호출하지 않습니다.
    }
    return S_OK;
}


HRESULT CJson_Loader::Load_GameObjects(_uint iLevelIndex, const _tchar* pFilePath)
{
    parser parser;
    simdjson::padded_string json_file_data;
    auto load_error = simdjson::padded_string::load(WideToUtf8(pFilePath).c_str()).get(json_file_data);
    if (load_error) {
        //std::string error_msg = "Failed to load JSON file: " + WideToUtf8(pFilePath) + " Error: " + simdjson::error_message(load_error);
        //MSG_BOX(error_msg.c_str());
        return E_FAIL;
    }

    simdjson::ondemand::document doc;
    auto error = parser.iterate(json_file_data).get(doc);
    if (error) {
        //std::string error_msg = "Failed to parse JSON data from file: " + WideToUtf8(pFilePath) + " Error: " + simdjson::error_message(error);
        //MSG_BOX(error_msg.c_str());
        return E_FAIL;
    }

    simdjson::ondemand::object root_obj; 
    if (doc.get_object().get(root_obj) != simdjson::SUCCESS) {
        MSG_BOX("JSON root is not an object.");
        return E_FAIL;
    }
    
    // JSON 파일의 최상위는 레이어 이름들을 키로 가짐
    for (simdjson::ondemand::field layer_field : root_obj) 
    {
        std::string_view layer_name_sv = layer_field.escaped_key();
        std::wstring strLayerTag = Utf8ToWide(std::string(layer_name_sv));

        simdjson::ondemand::object layer_content_obj; // 예: {"GameObjects": [...]}
        if (layer_field.value().get_object().get(layer_content_obj) != simdjson::SUCCESS) {
            // MSG_BOX(("Layer content for '" + std::string(layer_name_sv) + "' is not an object.").c_str());
            continue;
        }

        simdjson::ondemand::array game_objects_array; // 예: [{"CBackGround": {...data...}}, {"CAnotherClass": {...data...}}]
        if (layer_content_obj["GameObjects"].get_array().get(game_objects_array) != simdjson::SUCCESS) {
            // MSG_BOX(("'GameObjects' array not found or not an array in layer: " + std::string(layer_name_sv)).c_str());
            continue;
        }

        for (simdjson::ondemand::value game_object_container_val : game_objects_array) 
        {
            if (game_object_container_val.type() != simdjson::ondemand::json_type::object) {
                continue;
            }
            // game_object_container_val는 {"ClassName": {data_for_class}} 형태의 객체
            simdjson::ondemand::object gameObjectContainer = game_object_container_val.get_object();

            // gameObjectContainer는 정확히 하나의 필드(클래스 이름)를 가질 것으로 예상
            auto field_iterator = gameObjectContainer.begin();
            if (field_iterator == gameObjectContainer.end()) {
                // 비어있는 게임 오브젝트 컨테이너 객체
                continue;
            }
            
            simdjson::ondemand::field go_field = *field_iterator;
            // std::string_view gameObjectClassName_sv = go_field.key(); // 클래스 이름 (예: "CBackGround")
                                                                    // 이 클래스 이름은 프로토타입 태그를 찾는 데 직접 사용되지 않음.
                                                                    // 실제 프로토타입 태그는 go_field.value() 내의 "PrototypeTag"에서 가져옴.

            simdjson::ondemand::object gameObjectSpecificData; // 이것이 실제 Deserialize에 전달될 데이터
                                                                // (예: {"Active": true, "CShader": ..., "PrototypeTag": "..."})
            if (go_field.value().get_object().get(gameObjectSpecificData) == simdjson::SUCCESS) 
            {
                // gameObjectSpecificData 내부에서 "PrototypeTag"를 읽어옴
                std::string_view prototypeTag_sv;
                if (gameObjectSpecificData["PrototypeTag"].get_string().get(prototypeTag_sv) != simdjson::SUCCESS) {
                    // MSG_BOX(("Missing 'PrototypeTag' in GameObject data for class: " + std::string(gameObjectClassName_sv)).c_str());
                    continue;
                }
                std::wstring strPrototypeTag = Utf8ToWide(std::string(prototypeTag_sv));

                // CGameInstance::Add_GameObject는 내부적으로 CPrototype_Manager::Clone_Prototype을 호출하고,
                // 이 Clone_Prototype은 해당 프로토타입 객체의 Clone(jsonData)를 호출.
                // CGameObject 파생 클래스의 Clone(jsonData)는 new로 객체를 만들고,
                // 그 객체의 Deserialize(jsonData)를 호출하여 상태(자식 포함)를 채움.
                if (FAILED(m_pGameInstance->Add_GameObject(iLevelIndex, strPrototypeTag, iLevelIndex, strLayerTag, gameObjectSpecificData)))
                {
                    //std::wstring errMsg = L"Failed to add GameObject (Prototype: " + strPrototypeTag + L") to layer: " + strLayerTag;
                    //MSG_BOX(errMsg.c_str());
                    return E_FAIL;
                }
            }
        }
    }
    return S_OK;
}