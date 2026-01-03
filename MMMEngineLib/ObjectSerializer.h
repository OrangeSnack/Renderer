#pragma once
#include "rttr/type.h"
#include "json/json.hpp"

using namespace nlohmann::json_abi_v3_12_0;
using namespace rttr;

namespace MMMEngine
{
    class ObjectSerializer
    {
    private:
        // 직렬화 헬퍼
        std::optional<json> SerializeVariant(const variant& var);
        std::optional<json> SerializeObjectPtr(const variant& var);
        std::optional<json> SerializeSequentialContainer(const variant& var);

        // 역직렬화 헬퍼
        bool DeserializeVariant(const json& j, variant& var);
        bool DeserializeObjectPtr(const json& j, variant& var);
        bool DeserializeSequentialContainer(const json& j, variant& containerVar);

    public:
        // Object 하나를 직렬화 (ObjectPtr<T>를 variant로 받음)
        json Serialize(const variant& objectHandle);

        // Object 하나를 역직렬화 (ObjectPtr 참조는 GUID로만 남겨둠)
        bool Deserialize(const json& j, const variant& objectHandle);
    };
}
