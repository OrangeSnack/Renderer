#include "PathMapRuntime.h"
#include <fstream>
#include <vector>

namespace MMMEngine::Player
{
#pragma pack(push, 1)
    struct PathMapHeader
    {
        uint32_t magic;       // 'PMAP'
        uint32_t version;     // 1
        uint32_t entryCount;
        uint32_t stringBytes;
    };

    struct PathMapEntry
    {
        uint32_t pathOffset;
        Utility::MUID muid;
    };
#pragma pack(pop)

    static constexpr uint32_t PMAP_MAGIC =
        (uint32_t('P')) | (uint32_t('M') << 8) | (uint32_t('A') << 16) | (uint32_t('P') << 24);

    bool PathMapRuntime::LoadFromFile(const std::wstring& path)
    {
        std::ifstream f(path, std::ios::binary);
        if (!f) return false;

        PathMapHeader h{};
        f.read(reinterpret_cast<char*>(&h), sizeof(h));
        if (!f) return false;

        if (h.magic != PMAP_MAGIC || h.version != 1) return false;

        std::vector<char> blob(h.stringBytes);
        if (!blob.empty())
        {
            f.read(blob.data(), static_cast<std::streamsize>(blob.size()));
            if (!f) return false;
        }

        std::vector<PathMapEntry> entries(h.entryCount);
        if (!entries.empty())
        {
            f.read(reinterpret_cast<char*>(entries.data()),
                static_cast<std::streamsize>(entries.size() * sizeof(PathMapEntry)));
            if (!f) return false;
        }

        m_map.clear();
        m_map.reserve(entries.size());

        for (const auto& e : entries)
        {
            if (e.pathOffset >= blob.size()) return false;
            const char* p = blob.data() + e.pathOffset;

            // NUL-terminated 문자열이라고 가정
            std::string key(p);
            m_map.emplace(std::move(key), e.muid);
        }

        return true;
    }

    bool PathMapRuntime::TryGetMUID(std::string_view sourcePath, Utility::MUID& out) const
    {
        auto it = m_map.find(std::string(sourcePath));
        if (it == m_map.end()) return false;
        out = it->second;
        return true;
    }
}
