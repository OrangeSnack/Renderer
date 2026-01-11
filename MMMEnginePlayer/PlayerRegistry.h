#pragma once
#include "PathMapRuntime.h"
#include "AssetIndexRuntime.h"
#include "PakFileReader.h"
#include "PakBytesProvider.h"
#include "PlayerResolver.h"

namespace MMMEngine::Player
{
	inline static PathMapRuntime g_pathMap;
	inline static AssetIndexRuntime g_assetIndex;
	inline static PakFileReader g_pak;
	inline static PlayerResolver g_resolver{ &g_pathMap,&g_assetIndex };
	inline static PakBytesProvider g_bytes{ &g_pak };
}
