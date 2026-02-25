#ifndef SCENE_H
#define SCENE_H

#include "Core/Unit.h"
#include <memory>
#include <vector>

namespace URay
{
	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

	public:
		void Update();
		void Render();

		Unit* AddUnit();

	private:
		std::vector<std::unique_ptr<Unit>> _units;
	};
}

#endif
