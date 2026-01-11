#include "PakBytesProvider.h"
#include "PakFileReader.h"

bool MMMEngine::Player::PakBytesProvider::ReadAll(const MMMEngine::AssetEntry& entry, std::vector<uint8_t>& outBytes)
{
    if (!m_reader) return false;
    if (entry.source != MMMEngine::AssetEntry::Source::Pak) return false;
    return m_reader->Read(entry.offset, entry.size, outBytes);
}
