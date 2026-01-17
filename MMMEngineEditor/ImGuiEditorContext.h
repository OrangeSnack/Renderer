#pragma once
#include <d3d11.h>
#include <d3d11_1.h>
//#include <wrl/client.h> // Microsoft::WRL::ComPtr

#include <imgui.h>
#include "Singleton.hpp"

#pragma comment(lib, "d3d11.lib")

namespace MMMEngine::Editor
{
	class ImGuiEditorContext : public Utility::Singleton<ImGuiEditorContext>
	{
	private:
		HWND m_hWnd;
		ID3D11Device* m_pD3DDevice;
		ID3D11DeviceContext* m_pD3DContext;

		bool m_isImGuiInit = false;
		bool m_isWin32BackendInit = false;
		bool m_isD3D11BackendInit = false;
		bool m_isHovered = false;

		void UpdateInfiniteDrag();

		ImFont* m_bigFont;
		ImFont* m_defaultFont;
	public:
		bool Initialize(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
		void BeginFrame();
		void Render();
		void EndFrame();
		void Uninitialize();

		bool GetIsHovered();
		void HandleWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}