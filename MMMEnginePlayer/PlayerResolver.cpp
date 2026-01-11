#include "PlayerResolver.h"

bool MMMEngine::Player::PlayerResolver::Resolve(std::string_view sourcePath, MMMEngine::AssetEntry& out) const
{
    if (!m_pathMap || !m_index) return false;

    Utility::MUID muid{};
    if (!m_pathMap->TryGetMUID(sourcePath, muid)) return false;

    AssetSpan sp{};
    if (!m_index->TryGetSpan(muid, sp)) return false;

    out.muid = muid;
    out.source = MMMEngine::AssetEntry::Source::Pak;
    out.filePath.clear();
    out.offset = sp.offset;
    out.size = sp.size;
    return true;
}

bool MMMEngine::Player::PlayerResolver::Resolve(const Utility::MUID& muid, MMMEngine::AssetEntry& out) const
{
    if (!m_index) return false;

    AssetSpan sp{};
    if (!m_index->TryGetSpan(muid, sp)) return false;

    out.muid = muid;
    out.source = MMMEngine::AssetEntry::Source::Pak;
    out.filePath.clear();
    out.offset = sp.offset;
    out.size = sp.size;
    return true;
}