#pragma once
#include <fstream>
#include <vector>
#include <cstdint>
#include <string>

namespace MMMEngine::Player
{
    class PakFileReader
    {
    public:
        bool Open(const std::wstring& pakPath);

        bool Read(uint64_t offset, uint64_t size, std::vector<uint8_t>& outBytes)
        {
            if (!m_file) return false;

            outBytes.resize(static_cast<size_t>(size));
            m_file.seekg(static_cast<std::streamoff>(offset), std::ios::beg);
            if (!m_file) return false;

            if (size > 0)
            {
                m_file.read(reinterpret_cast<char*>(outBytes.data()),
                    static_cast<std::streamsize>(size));
                if (!m_file) return false;
            }
            return true;
        }

    private:
        std::ifstream m_file;
    };
}
