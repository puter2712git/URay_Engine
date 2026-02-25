#include "Core/Unit.h"
#include "Core/Component.h"

namespace URay
{
	void Unit::Update()
	{
		for (const auto& component : _components)
		{
			component->Update();
		}
	}

	void Unit::Render()
	{
		for (const auto& component : _components)
		{
			component->Render();
		}
	}
}