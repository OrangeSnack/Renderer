#pragma once
#include "IAssetResolver.h"
#include "PathMapRuntime.h"
#include "AssetIndexRuntime.h"

namespace MMMEngine::Player
{
    class PlayerResolver final : public MMMEngine::IAssetResolver
    {
    public:
        PlayerResolver(const PathMapRuntime* pm, const AssetIndexRuntime* ix)
            : m_pathMap(pm), m_index(ix) {
        }

        bool Resolve(std::string_view sourcePath, MMMEngine::AssetEntry& out) const override;

        bool Resolve(const Utility::MUID& muid, MMMEngine::AssetEntry& out) const override;

    private:
        const PathMapRuntime* m_pathMap = nullptr;
        const AssetIndexRuntime* m_index = nullptr;
    };
}
