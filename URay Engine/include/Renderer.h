#ifndef RENDERER_H
#define RENDERER_H

#include "Resource/Shader.h"
#include "Common/Math/Math.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <memory>
#include <unordered_map>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace URay
{
	struct SimpleVertex
	{
		Vector3 position;
		Vector4 color;
	};

	struct PassConstants
	{
		DirectX::XMFLOAT4X4 viewProj = {};
	};

	struct ObjectConstants
	{
		DirectX::XMFLOAT4X4 world = {};
	};

	struct Mesh
	{
		ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
		ComPtr<ID3D11Buffer> indexBuffer = nullptr;

		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;
		UINT indexCount = 0;

		DXGI_FORMAT indexFormat = DXGI_FORMAT_R32_UINT;
		D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	};

	class Renderer
	{
	public:
		Renderer() = default;
		~Renderer() = default;

	public:
		bool Initialize(HWND hWnd);
		void Finalize();

		void BeginFrame();
		void EndFrame();

		void UpdatePassConstants(PassConstants& passConstants);
		void UpdateObjectConstants(ObjectConstants& objectConstants);

		std::unique_ptr<Shader> CreateShader(const wchar_t* shaderPath);

		ComPtr<ID3D11Buffer> CreateVertexBuffer(const SimpleVertex* vertices, UINT vertexCount);
		ComPtr<ID3D11Buffer> CreateIndexBuffer(const UINT* indices, UINT indexCount);
		Mesh* GetMesh(const std::string& name);

		ID3D11Device* GetDevice() const { return _device.Get(); }
		ID3D11DeviceContext* GetDeviceContext() const { return _deviceContext.Get(); }

	private:
		void CreateDeviceAndSwapChain(HWND hWnd);
		void ReleaseDeviceAndSwapChain();

		void CreateFrameBuffer();
		void ReleaseFrameBuffer();

		void CreateRasterizerState();
		void ReleaseRasterizerState();

		void CreateConstantBuffers();
		void ReleaseConstantBuffers();

		void CreateDefaultMeshes();

	private:
		ComPtr<ID3D11Device> _device = nullptr;
		ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;

		ComPtr<IDXGISwapChain> _swapChain = nullptr;

		D3D11_VIEWPORT _viewport = {};

		ComPtr<ID3D11Texture2D> _frameBuffer = nullptr;
		ComPtr<ID3D11RenderTargetView> _renderTargetView = nullptr;

		ComPtr<ID3D11RasterizerState> _rasterizerState = nullptr;

		FLOAT _clearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f };

		std::unordered_map<std::string, std::unique_ptr<Mesh>> _meshes;

		ComPtr<ID3D11Buffer> _passConstantBuffer = nullptr;
		ComPtr<ID3D11Buffer> _objectConstantBuffer = nullptr;
	};
}

#endif
