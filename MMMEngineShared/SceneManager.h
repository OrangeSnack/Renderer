#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include "ExportSingleton.hpp"
#include "Scene.h"
#include "SceneRef.h"

namespace MMMEngine
{
	class MMMENGINE_API SceneManager : public Utility::ExportSingleton<SceneManager>
	{
	private:
		std::unordered_map<std::string, size_t> m_sceneNameToID;			// <Name , ID>
		std::vector<std::unique_ptr<Scene>> m_scenes;

		size_t m_currentSceneID;
		size_t m_nextSceneID;

		std::unique_ptr<Scene> m_dontDestroyOnLoadScene;

		// todo : json 메세지팩으로 to index, scene snapshot을 Scene생성하면서 로드시키기
		//void LoadScenes(); 
	public:
		const SceneRef GetCurrentScene() const;

		Scene* GetSceneRaw(const SceneRef& ref);

		void ChangeScene(const std::string& name);
		void ChangeScene(const size_t& id);

		void StartUp();
		void ShutDown();
		bool CheckSceneIsChanged();
	};
}