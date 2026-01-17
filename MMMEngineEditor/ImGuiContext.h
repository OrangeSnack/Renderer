#pragma once

#include <d3d11.h>
#include <d3d11_1.h>
//#include <wrl/client.h> // Microsoft::WRL::ComPtr

#include <imgui.h>
#include "Singleton.hpp"

#pragma comment(lib, "d3d11.lib")

namespace MMMEngine::Editor
{
	class ImGuiContext : public Utility::Singleton<ImGuiContext>
	{
	private:
		HWND m_hWnd;
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pImmediateContext;

		bool m_isImGuiInit = false;
		bool m_isWin32BackendInit = false;
		bool m_isD3D11BackendInit = false;
		bool m_isHovered = false;

		void UpdateInfiniteDrag();

		ImFont* m_bigFont;
		ImFont* m_defaultFont;
	public:
		bool Initialize(ID3D11Device* pDevice);
		void BeginFrame();
		void Update();
		void Render();
		void Uninitialize();
		bool GetIsHovered();
	};
}