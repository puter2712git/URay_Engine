#include "Resource/Shader.h"
#include "Common/DirectXHelper.h"
#include <d3dcompiler.h>
#include <iostream>

namespace URay
{
	void Shader::Create(const wchar_t* shaderPath, ID3D11Device* device)
	{
		ComPtr<ID3DBlob> vsBlob = nullptr;
		ComPtr<ID3DBlob> psBlob = nullptr;

		ThrowIfFailed(D3DCompileFromFile(shaderPath, nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, vsBlob.GetAddressOf(), nullptr));
		ThrowIfFailed(device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, _vertexShader.GetAddressOf()));

		ThrowIfFailed(D3DCompileFromFile(shaderPath, nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, psBlob.GetAddressOf(), nullptr));
		ThrowIfFailed(device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, _pixelShader.GetAddressOf()));

		D3D11_INPUT_ELEMENT_DESC inputLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		ThrowIfFailed(device->CreateInputLayout(inputLayout, ARRAYSIZE(inputLayout),
			vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), _inputLayout.GetAddressOf()));
	}

	void Shader::Release()
	{
		if (_inputLayout)
		{
			_inputLayout.Reset();
		}

		if (_pixelShader)
		{
			_pixelShader.Reset();
		}

		if (_vertexShader)
		{
			_vertexShader.Reset();
		}
	}
}