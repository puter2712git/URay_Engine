#ifndef SHADER_H
#define SHADER_H

#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace URay
{
	class Shader
	{
	public:
		Shader() = default;
		~Shader() = default;

	public:
		void Create(const wchar_t* shaderPath);
		void Release();

		void Bind();

	private:
		ComPtr<ID3D11VertexShader> _vertexShader = nullptr;
		ComPtr<ID3D11PixelShader> _pixelShader = nullptr;
		ComPtr<ID3D11InputLayout> _inputLayout = nullptr;
	};
}

#endif
