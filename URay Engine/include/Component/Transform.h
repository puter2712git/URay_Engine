#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Core/Component.h"
#include "Common/Math/Math.h"
#include <DirectXMath.h>

namespace URay
{
	class Transform : public Component
	{
	public:
		Transform() = default;
		~Transform() override = default;

	public:
		void Initialize() override;
		void Update() override;

		Vector3 GetPosition() const { return _position; }
		void SetPosition(Vector3 position) { _position = position; _isDirty = true; }
		Vector3 GetRotation() const { return _rotation; }
		void SetRotation(Vector3 rotation) { _rotation = rotation; _isDirty = true; }
		Vector3 GetScale() const { return _scale; }
		void SetScale(Vector3 scale) { _scale = scale; _isDirty = true; }

		void UpdateWorldMatrix();
		DirectX::XMMATRIX GetWorldMatrix() const { return DirectX::XMLoadFloat4x4(&_worldMatrix); }

	private:
		Vector3 _position = { 0.0f, 0.0f, 0.0f };
		Vector3 _rotation = { 0.0f, 0.0f, 0.0f };
		Vector3 _scale = { 1.0f, 1.0f, 1.0f };

		bool _isDirty = true;
		DirectX::XMFLOAT4X4 _worldMatrix;
	};
}

#endif
