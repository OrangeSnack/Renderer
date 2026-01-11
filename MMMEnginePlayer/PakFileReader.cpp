#include "PakFileReader.h"

bool MMMEngine::Player::PakFileReader::Open(const std::wstring& pakPath)
{
    m_file.close();
    m_file.open(pakPath, std::ios::binary);
    return (bool)m_file;
}
