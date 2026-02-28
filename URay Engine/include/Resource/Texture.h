#ifndef TEXTURE_H
#define TEXTURE_H

#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace URay
{
	class Texture
	{
	public:
		Texture() = default;
		~Texture() = default;

	public:
		void Create(const wchar_t* texturePath);
		void Release();

		void Bind();

	private:
		ComPtr<ID3D11Texture2D> _texture;
		ComPtr<ID3D11ShaderResourceView> _srv;
		ComPtr<ID3D11SamplerState> _samplerState;
	};
}

#endif
