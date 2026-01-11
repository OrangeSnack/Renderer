#pragma once
#include "MUID.h"
#include <cstdint>
#include <unordered_map>
#include <string>

namespace MMMEngine::Player
{
    struct AssetSpan
    {
        uint64_t offset = 0;
        uint64_t size = 0;
    };

    class AssetIndexRuntime
    {
    public:
        bool LoadFromFile(const std::wstring& path);

        bool TryGetSpan(const Utility::MUID& muid, AssetSpan& out) const;

    private:
        std::unordered_map<Utility::MUID, AssetSpan, Utility::MUID::Hash> m_map;
    };
}
