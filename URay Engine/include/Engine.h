#ifndef ENGINE_H
#define ENGINE_H

#include <memory>
#include "Window.h"
#include "Renderer.h"
#include "Timer.h"
#include "InputManager.h"
#include "Core/Scene.h"
#include "Core/ResourceManager.h"

namespace URay
{
	class Engine
	{
	public:
		Engine();
		~Engine();

		static Engine* GetInstance() { return _instance; }

	public:
		bool Initialize(HINSTANCE hInstance);
		void Run();
		void Finalize();

		void SetFPS(double fps) { _targetFPS = fps; _targetFrameTime = 1.0 / _targetFPS; }
		double GetFPS() const { return _targetFPS; }
		double GetElapsedTime() const { return _timer->GetElapsedTime(); }

		Renderer* GetRenderer() const { return _renderer.get(); }
		Timer* GetTimer() const { return _timer.get(); }
		ResourceManager* GetResourceManager() const { return _resourceManager.get(); }

		Scene* GetCurrentScene() const { return _currScene; }
		void SetCurrentScene(Scene* scene) { _currScene = scene; }

	private:
		void Update();
		void Render();

	private:
		static Engine* _instance;

		std::unique_ptr<Window> _window = nullptr;
		std::unique_ptr<Renderer> _renderer = nullptr;
		std::unique_ptr<Timer> _timer = nullptr;
		std::unique_ptr<InputManager> _input = nullptr;
		std::unique_ptr<ResourceManager> _resourceManager = nullptr;

		double _targetFPS = 60.0;
		double _targetFrameTime = 1.0 / _targetFPS;
		
		Scene* _currScene = nullptr;
	};
}

#endif
