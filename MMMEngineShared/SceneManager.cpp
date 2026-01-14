#include "SceneManager.h"

DEFINE_SINGLETON(MMMEngine::SceneManager)


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


void MMMEngine::SceneManager::StartUp()
{
	// 고정된 경로로 json 바이너리를 읽고 씬파일경로들과 씬이름과 ID를 해쉬맵에 등록
	// 아래는 임시 코드
	m_scenes.push_back(std::make_unique<Scene>());
	m_currentSceneID = 0;
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
