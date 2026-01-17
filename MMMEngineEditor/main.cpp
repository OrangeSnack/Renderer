#define NOMINMAX
#include <iostream>

#include "GlobalRegistry.h"
#include "EditorRegistry.h"
#include "App.h"

#include "InputManager.h"
#include "ResourceManager.h"
#include "TimeManager.h"
#include "RenderManager.h"
#include "BehaviourManager.h"

#include "ImGuiEditorContext.h"

using namespace MMMEngine;
using namespace MMMEngine::Utility;
using namespace MMMEngine::Editor;

void Initialize()
{
	auto app = GlobalRegistry::g_pApp;
	auto hwnd = app->GetWindowHandle();
	auto windowInfo = app->GetWindowInfo();

	InputManager::Get().StartUp(hwnd);
	TimeManager::Get().StartUp();
	app->OnWindowSizeChanged.AddListener<InputManager, &InputManager::HandleWindowResize>(&InputManager::Get());

	RenderManager::Get().StartUp(hwnd, windowInfo.width, windowInfo.height);
	app->OnWindowSizeChanged.AddListener<RenderManager, &RenderManager::ResizeScreen>(&RenderManager::Get());

	ImGuiEditorContext::Get().Initialize(hwnd, RenderManager::Get().GetDevice(), RenderManager::Get().GetContext());
	app->OnBeforeWindowMessage.AddListener<ImGuiEditorContext, &ImGuiEditorContext::HandleWindowMessage>(&ImGuiEditorContext::Get());
}

void Update()
{
	TimeManager::Get().BeginFrame();
	InputManager::Get().Update();

	static bool isGameRunning = false;

	TimeManager::Get().ConsumeFixedSteps([&](float fixedDt)
		{
			if (!isGameRunning)
				return;

			//PhysicsManager::Get()->PreSyncPhysicsWorld();
			//PhysicsManager::Get()->PreApplyTransform();
			BehaviourManager::Get().BroadCastBehaviourMessage("FixedUpdate");
			//PhysicsManager::Get()->Simulate(fixedDt);
			//PhysicsManager::Get()->ApplyTransform();
		});

	RenderManager::Get().BeginFrame();
	RenderManager::Get().Render();
	ImGuiEditorContext::Get().BeginFrame();
	ImGuiEditorContext::Get().Render();
	ImGuiEditorContext::Get().EndFrame();
	RenderManager::Get().EndFrame();
}

void Release()
{
	GlobalRegistry::g_pApp = nullptr;
	RenderManager::Get().ShutDown();
	TimeManager::Get().ShutDown();
	InputManager::Get().ShutDown();
}

int main()
{
	App app{ L"MMMEditor",1600,900 };
	GlobalRegistry::g_pApp = &app;

	app.OnInitialize.AddListener<&Initialize>();
	app.OnUpdate.AddListener<&Update>();
	app.OnRelease.AddListener<&Release>();
	app.Run();
}