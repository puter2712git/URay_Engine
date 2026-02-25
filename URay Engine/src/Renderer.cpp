#include "Renderer.h"
#include "Common/DirectXHelper.h"
#include <iostream>

namespace URay
{
	SimpleVertex triangleVertices[] =
	{
		{ 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
		{ -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }
	};

	bool Renderer::Initialize(HWND hWnd)
	{
		try
		{
			CreateDeviceAndSwapChain(hWnd);
			CreateFrameBuffer();
			CreateRasterizerState();

			return true;
		}
		catch (const std::exception& e)
		{
			std::cout << "Renderer initialization failed: " << e.what() << std::endl;
			Finalize();

			return false;
		}
	}

	void Renderer::Finalize()
	{
		ReleaseRasterizerState();
		ReleaseFrameBuffer();
		ReleaseDeviceAndSwapChain();
	}

	void Renderer::SwapBuffer()
	{
		_swapChain->Present(1, 0);
	}

	std::unique_ptr<Shader> Renderer::CreateShader(const wchar_t* shaderPath)
	{
		try
		{
			auto shader = std::make_unique<Shader>();
			shader->Create(shaderPath, _device.Get());

			return shader;
		}
		catch (const std::exception& e)
		{
			std::cout << "Create shader failed: " << e.what() << std::endl;
			return nullptr;
		}
	}

	void Renderer::CreateVertexBuffer(const std::string& name, SimpleVertex vertices[])
	{
		try
		{
			D3D11_BUFFER_DESC vertexBufferDesc = {};
			vertexBufferDesc.ByteWidth = sizeof(vertices);
			vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA vertexBufferSRD = { vertices };

			ComPtr<ID3D11Buffer> vertexBuffer;
			ThrowIfFailed(_device->CreateBuffer(&vertexBufferDesc, &vertexBufferSRD, vertexBuffer.GetAddressOf()));

			_vertexBuffers.insert({ name, vertexBuffer });
		}
		catch (const std::exception& e)
		{
			std::cout << "Failed to create vertex buffer [" << name << "]: " << e.what() << std::endl;
		}
	}


	void Renderer::CreateDeviceAndSwapChain(HWND hWnd)
	{
		D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferDesc.Width = 0;
		swapChainDesc.BufferDesc.Height = 0;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.OutputWindow = hWnd;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		ThrowIfFailed(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
			D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,
			featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION,
			&swapChainDesc, _swapChain.GetAddressOf(), _device.GetAddressOf(),
			nullptr, _deviceContext.GetAddressOf()));

		_swapChain->GetDesc(&swapChainDesc);

		_viewport = { 0.0f, 0.0f, (float)swapChainDesc.BufferDesc.Width, (float)swapChainDesc.BufferDesc.Height, 0.0f, 1.0f };
	}

	void Renderer::ReleaseDeviceAndSwapChain()
	{
		if (_deviceContext)
		{
			_deviceContext->Flush();
			_deviceContext.Reset();
		}

		if (_swapChain)
		{
			_swapChain.Reset();
		}

		if (_device)
		{
			_device.Reset();
		}
	}

	void Renderer::CreateFrameBuffer()
	{
		ThrowIfFailed(_swapChain->GetBuffer(0, IID_PPV_ARGS(_frameBuffer.GetAddressOf())));

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		ThrowIfFailed(_device->CreateRenderTargetView(_frameBuffer.Get(), &rtvDesc, _renderTargetView.GetAddressOf()));
	}

	void Renderer::ReleaseFrameBuffer()
	{
		if (_renderTargetView)
		{
			_renderTargetView.Reset();
		}

		if (_frameBuffer)
		{
			_frameBuffer.Reset();
		}
	}

	void Renderer::CreateRasterizerState()
	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;

		ThrowIfFailed(_device->CreateRasterizerState(&rasterizerDesc, _rasterizerState.GetAddressOf()));
	}

	void Renderer::ReleaseRasterizerState()
	{
		if (_rasterizerState)
		{
			_rasterizerState.Reset();
		}
	}
}
