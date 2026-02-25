#include "Component/PrimitiveRenderer.h"
#include "Engine.h"
#include "Component/Transform.h"
#include <iostream>

namespace URay
{
	void PrimitiveRenderer::Initialize()
	{
		_shaderName = "default";
		SetPrimitiveType(PrimitiveType::Triangle);
		std::cout << "[PrimitiveRenderer] Initialize - Shader: " << _shaderName << ", VB: " << _vertexBufferName << ", IB: " << _indexBufferName << std::endl;
	}

	void PrimitiveRenderer::Render()
	{
		Renderer* renderer = Engine::GetInstance()->GetRenderer();
		ID3D11DeviceContext* deviceContext = renderer->GetDeviceContext();
		ResourceManager* resourceManager = Engine::GetInstance()->GetResourceManager();

		if (!_cachedShader)
		{
			_cachedShader = resourceManager->GetShader(_shaderName);
			if (!_cachedShader)
			{
				std::cout << "[PrimitiveRenderer] ERROR: Shader not found: " << _shaderName << std::endl;
				return;
			}
		}
		if (!_cachedVertexBuffer)
		{
			_cachedVertexBuffer = renderer->GetVertexBuffer(_vertexBufferName);
			if (!_cachedVertexBuffer)
			{
				std::cout << "[PrimitiveRenderer] ERROR: Vertex buffer not found: " << _vertexBufferName << std::endl;
				return;
			}
		}
		if (!_cachedIndexBuffer)
		{
			_cachedIndexBuffer = renderer->GetIndexBuffer(_indexBufferName);
			if (!_cachedIndexBuffer)
			{
				std::cout << "[PrimitiveRenderer] ERROR: Index buffer not found: " << _indexBufferName << std::endl;
				return;
			}
		}

		Transform* transform = GetOwner()->GetComponent<Transform>();
		if (!transform)
		{
			std::cout << "[PrimitiveRenderer] ERROR: No Transform component" << std::endl;
			return;
		}

		ObjectConstants objConstants = {};
		DirectX::XMStoreFloat4x4(&objConstants.world, transform->GetWorldMatrix());
		renderer->UpdateObjectConstants(objConstants);

		_cachedShader->Bind(deviceContext);

		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, &_cachedVertexBuffer, &stride, &offset);
		deviceContext->IASetIndexBuffer(_cachedIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		deviceContext->DrawIndexed(6, 0, 0);

		std::cout << "[PrimitiveRenderer] Rendered triangle" << std::endl;
	}

	void PrimitiveRenderer::SetPrimitiveType(PrimitiveType primitiveType)
	{
		_primitiveType = primitiveType;
		switch (_primitiveType)
		{
		case PrimitiveType::Triangle:
			_vertexBufferName = "triangle";
			_indexBufferName = "triangle";
			break;
		case PrimitiveType::Square:
			_vertexBufferName = "square";
			_indexBufferName = "square";
			break;
		default:
			break;
		}
	}
}
