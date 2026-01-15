#include "SceneSerializer.h"
#include "GameObject.h"
#include "Component.h"
#include "rttr/type"

DEFINE_SINGLETON(MMMEngine::SceneSerializer)

void MMMEngine::SceneSerializer::Serialize(const Scene& scene, std::wstring path)
{
	for (auto& goPtr : scene.m_gameObjects)
	{

	}
}
