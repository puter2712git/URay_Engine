#ifndef TIMER_H
#define TIMER_H

#include <windows.h>

namespace URay
{
	class Timer
	{
	public:
		Timer() = default;
		~Timer() = default;

	public:
		void Initialize();
		void Tick();

		float GetDeltaTime() const { return _deltaTime; }
		float GetElapsedTime() const { return _elapsedTime; }

	private:
		float _deltaTime = 0.0f;
		float _elapsedTime = 0.0f;

		LARGE_INTEGER _frequency;

		LARGE_INTEGER _startCounter;
		LARGE_INTEGER _prevCounter;
		LARGE_INTEGER _currCounter;
	};
}

#endif
