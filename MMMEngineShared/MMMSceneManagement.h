#pragma once
#include "SceneManager.h"

namespace MMMEngine::SceneManagement
{
	void ChangeScene(const std::string& name) { SceneManager::Get().ChangeScene(name); }
	void ChangeScene(const size_t& id) { SceneManager::Get().ChangeScene(id); }
}