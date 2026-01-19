#pragma once
#include "Export.h"
#include "Resource.h"
#include "RenderShared.h"

namespace MMMEngine {
	class MMMENGINE_API StaticMesh : public Resource
	{
	public:
		// 메시 데이터
		std::shared_ptr<MeshData> meshData;
		// GPU 버퍼
		std::shared_ptr<MeshGPU> gpuBuffer;
		// 메시 그룹 <MatIdx, MeshIdx>
		std::unordered_map<UINT, std::vector<UINT>> meshGroupData;

		bool castShadows = true;
		bool receiveShadows = true;

		// TODO::직렬화 시켜야함
		bool LoadFromFilePath(std::wstring _filePath);
	};
}


