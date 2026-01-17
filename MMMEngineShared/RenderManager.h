#pragma once
#include <memory>
#include <unordered_map>
#include <vector>
#include "ExportSingleton.hpp"

//#include <directxtk/CommonStates.h>
//#include <directxtk/Effects.h>

#include <Windows.h>
#include <d3d11.h>
#include <d3d11_1.h>

#include <wrl/client.h> // Microsoft::WRL::ComPtr


#pragma comment(lib, "d3d11.lib")

namespace MMMEngine
{
	class MMMENGINE_API RenderManager : public Utility::ExportSingleton<RenderManager>
	{
	private:
        // 핵심 인프라
        Microsoft::WRL::ComPtr<ID3D11Device>            m_device;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext>     m_context;
        Microsoft::WRL::ComPtr<IDXGISwapChain>          m_swapChain;

        // 렌더 타겟 및 뎁스 버퍼
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;

        // 공용 스테이트 (초기화 시 미리 생성)
        Microsoft::WRL::ComPtr<ID3D11BlendState>        m_blendStateAlpha;
        Microsoft::WRL::ComPtr<ID3D11BlendState>        m_blendStateOpaque;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilStateLess;
        Microsoft::WRL::ComPtr<ID3D11SamplerState>      m_samplerLinear;
	public:
        // 리소스 생성 도우미 (Renderer 컴포넌트들이 사용)
        ID3D11Device* GetDevice() { return m_device.Get(); }
        ID3D11DeviceContext* GetContext() { return m_context.Get(); }

        void StartUp();
        void ShutDown();

        // 렌더링 루프 진입점
        void BeginFrame();
        void EndFrame(); // Present() 호출
	};
}