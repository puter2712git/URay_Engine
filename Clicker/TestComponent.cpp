#include "TestComponent.h"
#include <iostream>

void TestComponent::Initialize()
{
	std::cout << "Initialize TestComponent" << std::endl;
}

void TestComponent::Update()
{
	std::cout << "Update TestComponent" << std::endl;
}

void TestComponent::Render()
{
	std::cout << "Render TestComponent" << std::endl;
}

void TestComponent::Finalize()
{
	std::cout << "Finalize TestComponent" << std::endl;
}
