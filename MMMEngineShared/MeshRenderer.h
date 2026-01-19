#pragma once

#include "Export.h"
#include "Behaviour.h"
#include "ResourceManager.h"

namespace MMMEngine {
	class StaticMesh;
	class RendererBase;
	class MMMENGINE_API MeshRenderer : public Behaviour
	{
	private:

		// GPU ¹öÆÛ
		ResPtr<StaticMesh> mesh = nullptr;
		std::vector<std::weak_ptr<RendererBase>> renderers;

		void SetMesh(ResPtr<StaticMesh>& _mesh);

	public:
		MeshRenderer();

		void Start();
		void Update();
	};
}

