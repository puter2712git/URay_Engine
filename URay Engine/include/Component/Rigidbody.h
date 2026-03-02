#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "Core/Component.h"
#include "Common/Math/Math.h"

namespace URay
{
	class Rigidbody : public Component
	{
	public:
		Rigidbody() = default;
		~Rigidbody() override = default;

	public:
		void Update() override;

		Vector3 GetVelocity() const { return _velocity; }
		void SetVelocity(const Vector3& velocity) { _velocity = velocity; }

	private:
		Vector3 _velocity = Vector3(0.0f, 0.0f, 0.0f);
	};
}

#endif
