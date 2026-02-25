#include "TestComponent.h"
#include <Engine.h>
#include <iostream>

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
		URay::PrimitiveType type = _primitiveRenderer->GetPrimitiveType();

		if (type == URay::PrimitiveType::Triangle)
		{
			_primitiveRenderer->SetPrimitiveType(URay::PrimitiveType::Square);
			std::cout << "Switched to Square" << std::endl;
		}
		else
		{
			_primitiveRenderer->SetPrimitiveType(URay::PrimitiveType::Triangle);
			std::cout << "Switched to Triangle" << std::endl;
		}
	}
}

void TestComponent::Finalize()
{
}
