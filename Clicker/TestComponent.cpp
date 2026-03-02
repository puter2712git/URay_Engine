#include "TestComponent.h"
#include <Engine.h>
#include <iostream>
#include <Component/Transform.h>

void TestComponent::Initialize()
{
	_primitiveRenderer = GetOwner()->GetComponent<URay::PrimitiveRenderer>();
}

void TestComponent::Update()
{
	URay::Engine* engine = URay::Engine::GetInstance();
	URay::InputManager* input = engine->GetInput();

	if (input->GetMouseButtonDown(0))
	{
		URay::Transform* transform = GetOwner()->GetComponent<URay::Transform>();
		transform->SetPosition(transform->GetPosition() + URay::Vector3(1.0f, 0.0f, 0.0f));
	}
}

void TestComponent::Finalize()
{
}
