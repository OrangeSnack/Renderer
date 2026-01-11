#include "AssetIndexRuntime.h"
#include <fstream>
#include <vector>

namespace MMMEngine::Player
{
#pragma pack(push, 1)
    struct AssetIndexHeader
    {
        uint32_t magic;   // 'AIX0'
        uint32_t version; // 1
        uint32_t entryCount;
        uint32_t reserved;
    };

    struct AssetIndexEntry
    {
        Utility::MUID muid;
        uint64_t offset;
        uint64_t size;
    };
#pragma pack(pop)

    static constexpr uint32_t AIX0_MAGIC =
        (uint32_t('A')) | (uint32_t('I') << 8) | (uint32_t('X') << 16) | (uint32_t('0') << 24);

    bool AssetIndexRuntime::LoadFromFile(const std::wstring& path)
    {
        std::ifstream f(path, std::ios::binary);
        if (!f) return false;

        AssetIndexHeader h{};
        f.read(reinterpret_cast<char*>(&h), sizeof(h));
        if (!f) return false;

        if (h.magic != AIX0_MAGIC || h.version != 1) return false;

        std::vector<AssetIndexEntry> entries(h.entryCount);
        if (!entries.empty())
        {
            f.read(reinterpret_cast<char*>(entries.data()),
                static_cast<std::streamsize>(entries.size() * sizeof(AssetIndexEntry)));
            if (!f) return false;
        }

        m_map.clear();
        m_map.reserve(entries.size());

        for (const auto& e : entries)
        {
            m_map.emplace(e.muid, AssetSpan{ e.offset, e.size });
        }

        return true;
    }

    bool AssetIndexRuntime::TryGetSpan(const Utility::MUID& muid, AssetSpan& out) const
    {
        auto it = m_map.find(muid);
        if (it == m_map.end()) return false;
        out = it->second;
        return true;
    }
}
