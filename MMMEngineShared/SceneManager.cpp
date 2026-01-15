#include "SceneManager.h"

DEFINE_SINGLETON(MMMEngine::SceneManager)


void MMMEngine::SceneManager::LoadScenes(std::wstring rootPath)
{
}

void MMMEngine::SceneManager::CreateEmptyScene()
{
	m_scenes.push_back(std::make_unique<Scene>());
	m_currentSceneID = 0;
}


const MMMEngine::SceneRef MMMEngine::SceneManager::GetCurrentScene() const
{
	return { m_currentSceneID , false };
}

MMMEngine::Scene* MMMEngine::SceneManager::GetSceneRaw(const SceneRef& ref)
{
	if (m_scenes.size() <= ref.id && !ref.id_DDOL)
		return nullptr;

	if (ref.id_DDOL)
		return m_dontDestroyOnLoadScene.get();

	return m_scenes[ref.id].get();
}

void MMMEngine::SceneManager::ChangeScene(const std::string& name)
{
	auto it = m_sceneNameToID.find(name);
	if (it != m_sceneNameToID.end())
	{
		m_nextSceneID = it->second;
	}
}

void MMMEngine::SceneManager::ChangeScene(const size_t& id)
{
	if (id < m_scenes.size())
		m_nextSceneID = id;
}

void MMMEngine::SceneManager::StartUp(std::wstring rootPath, bool allowEmptyScene)
{
	// 고정된 경로로 json 바이너리를 읽고 씬파일경로를 불러오 ID맵을 초기화하고 초기씬을 생성함
	LoadScenes(rootPath);

	if (allowEmptyScene)
	{
		CreateEmptyScene();
	}
	else if(m_scenes.empty())
	{
		assert(false && "씬리스트가 비어있습니다!, 초기씬 로드에 실패했습니다!");
	}
}


void MMMEngine::SceneManager::ShutDown()
{
}

bool MMMEngine::SceneManager::CheckSceneIsChanged()
{
	if (m_nextSceneID != static_cast<size_t>(-1) &&
		m_nextSceneID < m_scenes.size())
	{
		if (m_currentSceneID != static_cast<size_t>(-1) && 
			m_currentSceneID < m_scenes.size())
		 	m_scenes[m_currentSceneID]->Clear();

		m_currentSceneID = m_nextSceneID;
		m_nextSceneID = static_cast<size_t>(-1);

		// todo : SceneSerializer 호출
		//m_currentSceneID->Initialize();
		return true;
	}

	return false;
}

MMMEngine::ObjPtr< MMMEngine::GameObject> MMMEngine::SceneManager::FindFromAllScenes(const std::string& name)
{
	for (auto& scene : m_scenes)
	{
		const auto& gameobjs_cache = scene->GetGameObjects();
		for (auto& go : gameobjs_cache)
		{
			if (go->GetName() == name)
			{
				return go;
			}
		}
	}
	const auto& ddol_gameobjs_cache = m_dontDestroyOnLoadScene->GetGameObjects();
	for (auto& ddol_go : ddol_gameobjs_cache)
	{
		if (ddol_go->GetName() == name)
		{
			return ddol_go;
		}
	}

	return nullptr;
}

MMMEngine::ObjPtr< MMMEngine::GameObject> MMMEngine::SceneManager::FindWithTagFromAllScenes(const std::string& tag)
{
	for (auto& scene : m_scenes)
	{
		const auto& gameobjs_cache = scene->GetGameObjects();
		for (auto& go : gameobjs_cache)
		{
			if (go->GetTag() == tag)
			{
				return go;
			}
		}
	}
	const auto& ddol_gameobjs_cache = m_dontDestroyOnLoadScene->GetGameObjects();
	for (auto& ddol_go : ddol_gameobjs_cache)
	{
		if (ddol_go->GetTag() == tag)
		{
			return ddol_go;
		}
	}

	return nullptr;
}

std::vector<MMMEngine::ObjPtr< MMMEngine::GameObject>> MMMEngine::SceneManager::FindGameObjectsWithTagFromAllScenes(const std::string& tag)
{
	std::vector<ObjPtr<GameObject>> cache;

	for (auto& scene : m_scenes)
	{
		const auto& gameobjs_cache = scene->GetGameObjects();
		for (auto& go : gameobjs_cache)
		{
			if (go->GetTag() == tag)
			{
				cache.push_back(go);
			}
		}
	}
	const auto& ddol_gameobjs_cache = m_dontDestroyOnLoadScene->GetGameObjects();
	for (auto& ddol_go : ddol_gameobjs_cache)
	{
		if (ddol_go->GetTag() == tag)
		{
			cache.push_back(ddol_go);
		}
	}

	return cache;
}
