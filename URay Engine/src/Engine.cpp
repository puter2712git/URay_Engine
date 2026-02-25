#include "Engine.h"
#include <iostream>
#include <chrono>
#include <thread>

namespace URay
{
	Engine::Engine() = default;

	Engine::~Engine() = default;

	bool Engine::Initialize(HINSTANCE hInstance)
	{
		if (AllocConsole())
		{
			FILE* fp;
			freopen_s(&fp, "CONOUT$", "w", stdout);
			std::ios::sync_with_stdio();
		}

		_window = std::make_unique<Window>();
		_window->Create(L"URay Engine", 1024, 1024, hInstance);

		_renderer = std::make_unique<Renderer>();
		if (!_renderer->Initialize(_window->GetHandle()))
		{
			return false;
		}

		_timer = std::make_unique<Timer>();
		_timer->Initialize();

		_input = std::make_unique<InputManager>();

		SetFPS(60.0);

		return true;
	}

	void Engine::Run()
	{
		double accumulatedTime = 0.0;
		int frameCount = 0;

		bool isExit = false;
		while (!isExit)
		{
			MSG msg = { 0 };
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					isExit = true;
					break;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			_timer->Tick();

			accumulatedTime += _timer->GetDeltaTime();
			if (accumulatedTime >= _targetFrameTime)
			{
				_input->Update();

				if (_currScene)
				{
					_currScene->Update();
					_currScene->Render();
				}

				_renderer->SwapBuffer();

				accumulatedTime -= _targetFrameTime;
				frameCount++;
			}

			double sleepTime = _targetFrameTime - accumulatedTime;
			if (sleepTime > 0.0)
			{
				std::chrono::duration<double> sleepDuration(sleepTime);
				std::this_thread::sleep_for(sleepDuration);
			}
		}
	}

	void Engine::Finalize()
	{
		if (_renderer)
		{
			_renderer->Finalize();
		}
	}
}