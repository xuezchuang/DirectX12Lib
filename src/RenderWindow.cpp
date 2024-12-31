#include "RenderWindow.h"
#include "D3D12RHI.h"
#include "WindowWin32.h"
#include "d3dx12.h"
#include "CommandQueue.h"
#include "CommandListManager.h"
#include "CommandContext.h"

const int MSAA_SAMPLE = 1;

extern FCommandListManager g_CommandListManager;


RenderWindow& RenderWindow::Get()
{
	static RenderWindow renderWindow;
	return renderWindow;
}

void RenderWindow::Initialize()
{
	WindowWin32& Window = WindowWin32::Get();
	m_bInit = true;
	OnResetSize(Window.GetWidth(), Window.GetHeight(), false);
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void RenderWindow::OnResetSize(int InWidth, int InHeight,bool m4xMsaaState)
{
	if(!m_bInit)
		return;
	FCommandQueue& GraphicQueue = g_CommandListManager.GetGraphicsQueue();
	GraphicQueue.Flush();

	for(int i = 0; i < BUFFER_COUNT; ++i)
	{
		m_BackBuffers[i].Destroy();
	}

	if(m_swapChain)
	{
		m_swapChain->ResizeBuffers(
			BUFFER_COUNT,
			InWidth,
			InHeight,
			DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	}
	else
	{
		WindowWin32& Window = WindowWin32::Get();
		D3D12RHI& RHI = D3D12RHI::Get();
		m_swapChain = CreateSwapChain(Window.GetWindowHandle(), RHI.GetDXGIFactory(), GraphicQueue.GetD3D12CommandQueue(), InWidth, InHeight, BUFFER_COUNT, m4xMsaaState);
	}

	for(int i = 0; i < BUFFER_COUNT; ++i)
	{
		ComPtr<ID3D12Resource> BackBuffer;
		ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&BackBuffer)));
		m_BackBuffers[i].CreateFromSwapChain(L"BackBuffer", BackBuffer.Detach());
	}
	Present();
}

void RenderWindow::Destroy()
{

}

ComPtr<IDXGISwapChain3> RenderWindow::CreateSwapChain(HWND hwnd, IDXGIFactory4* factory, ID3D12CommandQueue* commandQueue, int width, int height, int bufferCount,bool m4xMsaaState)
{
	DXGI_SWAP_CHAIN_DESC1 Desc = {};
	Desc.Width = width;
	Desc.Height = height;
	Desc.BufferCount = bufferCount;
	Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	Desc.SampleDesc.Quality = m4xMsaaState ? -1 : 0;
	Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	ComPtr<IDXGISwapChain1> swapchain1;
	ThrowIfFailed(factory->CreateSwapChainForHwnd(commandQueue, hwnd, &Desc, nullptr, nullptr, &swapchain1));

	ComPtr<IDXGISwapChain3> swapchain3;
	ThrowIfFailed(swapchain1.As(&swapchain3));
	return swapchain3;
}

UINT RenderWindow::Present()
{
	m_swapChain->Present(1, 0);
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
	return m_frameIndex;
}

FColorBuffer& RenderWindow::GetBackBuffer()
{
	return m_BackBuffers[m_frameIndex];
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderWindow::GetCurrentBackBufferView()
{
	return m_BackBuffers[m_frameIndex].GetRTV();
}

const DXGI_FORMAT& RenderWindow::GetColorFormat() const
{
	return m_BackBuffers[0].GetFormat();
}