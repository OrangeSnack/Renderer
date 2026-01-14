#pragma once
#include "ExportSingleton.hpp"

namespace MMMEngine
{
	class MMMENGINE_API SceneManager : public Utility::ExportSingleton<SceneManager>
	{
	public:
		void StartUp();
		void ShutDown();
		void Update();
	};
}