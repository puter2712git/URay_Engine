#include "Component/Transform.h"

namespace URay
{
	void Transform::Initialize()
	{
		UpdateWorldMatrix();
	}

	void Transform::Update()
	{
		if (_isDirty)
		{
			UpdateWorldMatrix();
			_isDirty = false;
		}
	}

	void Transform::UpdateWorldMatrix()
	{
		DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(_scale.x, _scale.y, _scale.z);
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z);
		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(_position.x, _position.y, _position.z);

		DirectX::XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
		DirectX::XMStoreFloat4x4(&_worldMatrix, DirectX::XMMatrixTranspose(worldMatrix));
	}
}
