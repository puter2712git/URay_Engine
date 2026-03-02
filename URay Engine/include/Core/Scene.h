#ifndef SCENE_H
#define SCENE_H

#include "Core/Unit.h"
#include <memory>
#include <vector>

namespace URay
{
	class Camera;

	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

	public:
		void Update();
		void Render();

		Unit* AddUnit();
		void AddUnit(Unit* unit);

		void SetMainCamera(Camera* camera) { _mainCamera = camera; }
		Camera* GetMainCamera() const { return _mainCamera; }

	private:
		std::vector<std::unique_ptr<Unit>> _units;
		Camera* _mainCamera = nullptr;
	};
}

#endif
