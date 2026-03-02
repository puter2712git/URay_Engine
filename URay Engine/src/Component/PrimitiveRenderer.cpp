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
				return;
			}
		}
		if (!_cachedMesh)
		{
			_cachedMesh = renderer->GetMesh(_meshName);
			if (!_cachedMesh)
			{
				return;
			}
		}

		Transform* transform = GetOwner()->GetComponent<Transform>();

		ObjectConstants objConstants = {};
		DirectX::XMStoreFloat4x4(&objConstants.world, transform->GetWorldMatrix());
		renderer->UpdateObjectConstants(objConstants);

		_cachedShader->Bind();

		deviceContext->IASetPrimitiveTopology(_cachedMesh->topology);

		deviceContext->IASetVertexBuffers(0, 1, _cachedMesh->vertexBuffer.GetAddressOf(), &_cachedMesh->stride, &_cachedMesh->offset);
		deviceContext->IASetIndexBuffer(_cachedMesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		deviceContext->DrawIndexed(_cachedMesh->indexCount, 0, 0);
	}

	void PrimitiveRenderer::SetPrimitiveType(PrimitiveType primitiveType)
	{
		_primitiveType = primitiveType;
		switch (_primitiveType)
		{
		case PrimitiveType::Triangle:
			_meshName = "triangle";
			_cachedMesh = nullptr;
			break;
		case PrimitiveType::Square:
			_meshName = "square";
			_cachedMesh = nullptr;
			break;
		default:
			break;
		}
	}
}
