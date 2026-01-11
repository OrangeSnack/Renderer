#pragma once
#include "MUID.h"
#include <unordered_map>
#include <string>

namespace MMMEngine::Player
{
    class PathMapRuntime
    {
    public:
        bool LoadFromFile(const std::wstring& path);

        bool TryGetMUID(std::string_view sourcePath, Utility::MUID& out) const;

    private:
        std::unordered_map<std::string, Utility::MUID> m_map;
    };
}
