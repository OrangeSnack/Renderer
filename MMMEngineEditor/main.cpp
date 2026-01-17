#define NOMINMAX
#include <iostream>

#include "GlobalRegistry.h"
#include "EditorRegistry.h"
#include "App.h"

#include "InputManager.h"
#include "ResourceManager.h"
#include "TimeManager.h"

using namespace MMMEngine;
using namespace MMMEngine::Utility;

void Init()
{
	InputManager::Get().StartUp(GlobalRegistry::g_pApp->GetWindowHandle());
	GlobalRegistry::g_pApp->OnWindowSizeChanged.AddListener<InputManager, &InputManager::HandleWindowResize>(&InputManager::Get());
}

void Update()
{
	InputManager::Get().Update();
}

void Release()
{
	GlobalRegistry::g_pApp = nullptr;
}

int main()
{
	App app{ L"MMMEditor",1600,900 };
	GlobalRegistry::g_pApp = &app;

	app.OnInitialize.AddListener<&Init>();
	app.OnUpdate.AddListener<&Update>();
	app.Run();
}