#include "Component/Camera.h"
#include "Component/Transform.h"
#include "Core/Unit.h"
#include "Config.h"
#include <iostream>

namespace URay
{
	void Camera::Update()
	{
		UpdateViewMatrix();
		UpdateProjMatrix();

		DirectX::XMMATRIX viewProjMatrix = DirectX::XMMatrixMultiply(GetViewMatrix(), GetProjMatrix());
		DirectX::XMStoreFloat4x4(&_viewProjMatrix, viewProjMatrix);
	}

	void Camera::UpdateViewMatrix()
	{
		Transform* transform = GetOwner()->GetComponent<Transform>();
		if (!transform)
		{
			std::cout << "[Camera] ERROR: No Transform component" << std::endl;
			return;
		}
		
		Vector3 position = transform->GetPosition();
		Vector3 rotation = transform->GetRotation();

		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
		DirectX::XMVECTOR forward = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix);
		DirectX::XMVECTOR up = DirectX::XMVector3TransformCoord(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotationMatrix);

		DirectX::XMFLOAT3 pos = { position.x, position.y, position.z };
		DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&pos), forward, up);
		DirectX::XMStoreFloat4x4(&_viewMatrix, viewMatrix);
		
		std::cout << "[Camera] Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
	}

	void Camera::UpdateProjMatrix()
	{
		if (_isOrthographic)
		{
			DirectX::XMMATRIX orthoProjMatrix = DirectX::XMMatrixOrthographicLH(Config::windowSize.x, Config::windowSize.y, _nearZ, _farZ);
			DirectX::XMStoreFloat4x4(&_projMatrix, orthoProjMatrix);
			std::cout << "[Camera] Orthographic Projection: " << Config::windowSize.x << "x" << Config::windowSize.y << std::endl;
		}
		else
		{
			DirectX::XMMATRIX perspectiveProjMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, Config::windowSize.x / Config::windowSize.y, _nearZ, _farZ);
			DirectX::XMStoreFloat4x4(&_projMatrix, perspectiveProjMatrix);
			std::cout << "[Camera] Perspective Projection" << std::endl;
		}
	}
}
