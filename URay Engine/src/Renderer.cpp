#include "Renderer.h"
#include "Engine.h"
#include "Common/DirectXHelper.h"
#include "Component/Camera.h"
#include <iostream>

namespace URay
{
	bool Renderer::Initialize(HWND hWnd)
	{
		try
		{
			CreateDeviceAndSwapChain(hWnd);
			CreateFrameBuffer();
			CreateRasterizerState();

			CreateConstantBuffers();
			CreatePrimitiveBuffers();

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

	void Renderer::BeginFrame()
	{
		_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), _clearColor);

		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		_deviceContext->RSSetViewports(1, &_viewport);
		_deviceContext->RSSetState(_rasterizerState.Get());

		_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), nullptr);
		_deviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);

		Engine* engine = Engine::GetInstance();
		Scene* currScene = engine->GetCurrentScene();

		if (currScene)
		{
			Camera* mainCamera = currScene->GetMainCamera();
			if (mainCamera)
			{
				PassConstants passConstants;
				DirectX::XMStoreFloat4x4(&passConstants.viewProj, mainCamera->GetViewProjMatrix());
				UpdatePassConstants(passConstants);
			}
		}

		_deviceContext->VSSetConstantBuffers(0, 1, _passConstantBuffer.GetAddressOf());
		_deviceContext->VSSetConstantBuffers(1, 1, _objectConstantBuffer.GetAddressOf());
	}

	void Renderer::EndFrame()
	{
		_swapChain->Present(1, 0);
	}

	void Renderer::UpdatePassConstants(PassConstants& passConstants)
	{
		D3D11_MAPPED_SUBRESOURCE constantBufferMSR;

		_deviceContext->Map(_passConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &constantBufferMSR);
		memcpy(constantBufferMSR.pData, &passConstants, sizeof(PassConstants));
		_deviceContext->Unmap(_passConstantBuffer.Get(), 0);
	}

	void Renderer::UpdateObjectConstants(ObjectConstants& objectConstants)
	{
		D3D11_MAPPED_SUBRESOURCE constantBufferMSR;

		_deviceContext->Map(_objectConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &constantBufferMSR);
		memcpy(constantBufferMSR.pData, &objectConstants, sizeof(ObjectConstants));
		_deviceContext->Unmap(_objectConstantBuffer.Get(), 0);
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

	void Renderer::CreateVertexBuffer(const std::string& name, const SimpleVertex* vertices, UINT vertexCount)
	{
		try
		{
			D3D11_BUFFER_DESC vertexBufferDesc = {};
			vertexBufferDesc.ByteWidth = sizeof(SimpleVertex) * vertexCount;
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

	ID3D11Buffer* Renderer::GetVertexBuffer(const std::string& name) const
	{
		auto it = _vertexBuffers.find(name);
		if (it != _vertexBuffers.end())
		{
			return it->second.Get();
		}
		else
		{
			std::cout << "Vertex buffer not found: " << name << std::endl;
			return nullptr;
		}
	}

	void Renderer::CreateIndexBuffer(const std::string& name, const UINT* indices, UINT indexCount)
	{
		try
		{
			D3D11_BUFFER_DESC indexBufferDesc = {};
			indexBufferDesc.ByteWidth = sizeof(UINT) * indexCount;
			indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

			D3D11_SUBRESOURCE_DATA indexBufferSRD = { indices };

			ComPtr<ID3D11Buffer> indexBuffer;
			ThrowIfFailed(_device->CreateBuffer(&indexBufferDesc, &indexBufferSRD, indexBuffer.GetAddressOf()));

			_indexBuffers.insert({ name, indexBuffer });
		}
		catch (const std::exception& e)
		{
			std::cout << "Failed to create index buffer [" << name << "]: " << e.what() << std::endl;
		}
	}

	ID3D11Buffer* Renderer::GetIndexBuffer(const std::string& name) const
	{
		auto it = _indexBuffers.find(name);
		if (it != _indexBuffers.end())
		{
			return it->second.Get();
		}
		else
		{
			std::cout << "Index buffer not found: " << name << std::endl;
			return nullptr;
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

	void Renderer::CreateConstantBuffers()
	{
		{
			D3D11_BUFFER_DESC constantBufferDesc = {};
			constantBufferDesc.ByteWidth = (sizeof(PassConstants) + 0xf) & ~0xf;
			constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;;
			constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

			ThrowIfFailed(_device->CreateBuffer(&constantBufferDesc, nullptr, _passConstantBuffer.GetAddressOf()));
		}
		{
			D3D11_BUFFER_DESC constantBufferDesc = {};
			constantBufferDesc.ByteWidth = (sizeof(ObjectConstants) + 0xf) & ~0xf;
			constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;;
			constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

			ThrowIfFailed(_device->CreateBuffer(&constantBufferDesc, nullptr, _objectConstantBuffer.GetAddressOf()));
		}
	}

	void Renderer::ReleaseConstantBuffers()
	{
		if (_passConstantBuffer)
		{
			_passConstantBuffer.Reset();
		}
		if (_objectConstantBuffer)
		{
			_objectConstantBuffer.Reset();
		}
	}

	void Renderer::CreatePrimitiveBuffers()
	{
		SimpleVertex triangleVertices[] =
		{
			{ Vector3(0.0f, 0.5f, 0.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f) },
			{ Vector3(0.5f, -0.5f, 0.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f) },
			{ Vector3(-0.5f, -0.5f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f) }
		};
		CreateVertexBuffer("triangle", triangleVertices, 3);
		UINT triangleIndices[] = { 0, 1, 2 };
		CreateIndexBuffer("triangle", triangleIndices, 3);

		SimpleVertex squareVertices[] =
		{
			{ Vector3(-0.5f, 0.5f, 0.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f) },
			{ Vector3(0.5f, 0.5f, 0.0f), Vector4(0.0f, 1.0f, 0.0f, 1.0f) },
			{ Vector3(0.5f, -0.5f, 0.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f) },
			{ Vector3(-0.5f, -0.5f, 0.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f) }
		};
		CreateVertexBuffer("square", squareVertices, 4);
		UINT squareIndices[] =
		{
			0, 1, 2,
			0, 2, 3
		};
		CreateIndexBuffer("square", squareIndices, 6);
	}
}
