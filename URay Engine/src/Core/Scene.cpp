#include "Core/Scene.h"

namespace URay
{
	void Scene::Update()
	{
		for (const auto& unit : _units)
		{
			unit->Update();
		}
	}

	void Scene::Render()
	{
		for (const auto& unit : _units)
		{
			unit->Render();
		}
	}

	Unit* Scene::AddUnit()
	{
		auto newUnit = std::make_unique<Unit>();
		Unit* pUnit = newUnit.get();
		_units.push_back(std::move(newUnit));
		return pUnit;
	}
}