#pragma once
#include "Singleton.hpp"

namespace MMMEngine
{
	class TimeManager : public Singleton<TimeManager>
	{
		void StartUp();
		void ShutDown();
		void Update();
	};
}