#include "Resource/Texture.h"
#include "Common/DirectXHelper.h"
#include "Engine.h"
#include <stb/stb_image.h>

namespace URay
{
	void Texture::Create(const wchar_t* texturePath)
	{
		Engine* engine = Engine::GetInstance();
		Renderer* renderer = engine->GetRenderer();
		ID3D11Device* device = renderer->GetDevice();

		FILE* file = _wfopen(texturePath, L"rb");
		if (!file) return;

		int width, height, channels;
		unsigned char* data = stbi_load_from_file(file, &width, &height, &channels, 4);

		fclose(file);

		if (!data) return;

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA srd = {};
		srd.pSysMem = data;
		srd.SysMemPitch = width * 4;

		ThrowIfFailed(device->CreateTexture2D(&textureDesc, &srd, _texture.GetAddressOf()));
		ThrowIfFailed(device->CreateShaderResourceView(_texture.Get(), nullptr, _srv.GetAddressOf()));

		stbi_image_free(data);
	}

	void Texture::Release()
	{
		if (_samplerState)
		{
			_samplerState.Reset();
		}
		if (_srv)
		{
			_srv.Reset();
		}
		if (_texture)
		{
			_texture.Reset();
		}
	}

	void Texture::Bind()
	{
		Engine* engine = Engine::GetInstance();
		Renderer* renderer = engine->GetRenderer();
		ID3D11DeviceContext* deviceContext = renderer->GetDeviceContext();

		deviceContext->PSSetShaderResources(0, 1, _srv.GetAddressOf());
		deviceContext->PSSetSamplers(0, 1, _samplerState.GetAddressOf());
	}
}
