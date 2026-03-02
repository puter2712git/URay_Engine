#include "Component/SpriteRenderer.h"
#include "Resource/Texture.h"
#include "Core/Unit.h"
#include "Component/Transform.h"
#include "Engine.h"

namespace URay
{
	void SpriteRenderer::Render()
	{
		if (!_texture) return;

		auto transform = GetOwner()->GetComponent<Transform>();
		if (!transform) return;

		Engine* engine = Engine::GetInstance();
		Renderer* renderer = engine->GetRenderer();
		ID3D11DeviceContext* deviceContext = renderer->GetDeviceContext();
		ResourceManager* resourceManager = engine->GetResourceManager();

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

		ObjectConstants objConstants;
		DirectX::XMStoreFloat4x4(&objConstants.world, transform->GetWorldMatrix());
		renderer->UpdateObjectConstants(objConstants);

		_cachedShader->Bind();
		_texture->Bind();

		deviceContext->IASetVertexBuffers(0, 1, _cachedMesh->vertexBuffer.GetAddressOf(), &_cachedMesh->stride, &_cachedMesh->offset);
		deviceContext->IASetIndexBuffer(_cachedMesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetPrimitiveTopology(_cachedMesh->topology);

		deviceContext->DrawIndexed(_cachedMesh->indexCount, 0, 0);
	}
}
