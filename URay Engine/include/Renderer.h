#ifndef RENDERER_H
#define RENDERER_H

#include "Resource/Shader.h"
#include <d3d11.h>
#include <string>
#include <memory>
#include <unordered_map>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace URay
{
	struct SimpleVertex
	{
		float x, y, z;
		float r, g, b, a;
	};

	class Renderer
	{
	public:
		Renderer() = default;
		~Renderer() = default;

	public:
		bool Initialize(HWND hWnd);
		void Finalize();

		void SwapBuffer();

		std::unique_ptr<Shader> CreateShader(const wchar_t* shaderPath);

		void CreateVertexBuffer(const std::string& name, SimpleVertex vertices[]);

		ID3D11Device* GetDevice() const { return _device.Get(); }
		ID3D11DeviceContext* GetDeviceContext() const { return _deviceContext.Get(); }

	private:
		void CreateDeviceAndSwapChain(HWND hWnd);
		void ReleaseDeviceAndSwapChain();

		void CreateFrameBuffer();
		void ReleaseFrameBuffer();

		void CreateRasterizerState();
		void ReleaseRasterizerState();

	private:
		ComPtr<ID3D11Device> _device = nullptr;
		ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;

		ComPtr<IDXGISwapChain> _swapChain = nullptr;

		D3D11_VIEWPORT _viewport = {};

		ComPtr<ID3D11Texture2D> _frameBuffer = nullptr;
		ComPtr<ID3D11RenderTargetView> _renderTargetView = nullptr;

		ComPtr<ID3D11RasterizerState> _rasterizerState = nullptr;

		FLOAT _clearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f };

		std::unordered_map<std::string, ComPtr<ID3D11Buffer>> _vertexBuffers;
		std::unordered_map<std::string, ComPtr<ID3D11Buffer>> _indexBuffers;

		static SimpleVertex triangleVertices[];
	};
}

#endif
