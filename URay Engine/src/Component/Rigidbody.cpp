#include "Component/Rigidbody.h"
#include "Component/Transform.h"
#include "Engine.h"

namespace URay
{
	void Rigidbody::Update()
	{
		Transform* transform = GetOwner()->GetComponent<Transform>();
		if (!transform) return;

		Engine* engine = Engine::GetInstance();
		Timer* timer = engine->GetTimer();

		Vector3 position = transform->GetPosition();
		transform->SetPosition(position + _velocity * timer->GetDeltaTime());
	}
}
