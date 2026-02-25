#ifndef CAMERA_H
#define CAMERA_H

#include "Core/Component.h"
#include <DirectXMath.h>

namespace URay
{
	class Camera : public Component
	{
	public:
		Camera() = default;
		~Camera() override = default;

	public:
		void Update() override;

		DirectX::XMMATRIX GetViewMatrix() const { return DirectX::XMLoadFloat4x4(&_viewMatrix); }
		DirectX::XMMATRIX GetProjMatrix() const { return DirectX::XMLoadFloat4x4(&_projMatrix); }
		DirectX::XMMATRIX GetViewProjMatrix() const { return DirectX::XMLoadFloat4x4(&_viewProjMatrix); }

		void UpdateViewMatrix();
		void UpdateProjMatrix();

	private:
		bool _isOrthographic = true;

		DirectX::XMFLOAT4X4 _viewMatrix;
		DirectX::XMFLOAT4X4 _projMatrix;
		DirectX::XMFLOAT4X4 _viewProjMatrix;

		float _nearZ = 0.1f;
		float _farZ = 1000.0f;
	};
}

#endif
