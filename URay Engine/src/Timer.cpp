#include "Timer.h"

namespace URay
{
	void Timer::Initialize()
	{
		_deltaTime = 0.0f;
		_elapsedTime = 0.0f;

		QueryPerformanceFrequency(&_frequency);
		QueryPerformanceCounter(&_startCounter);
		_prevCounter = _startCounter;
	}

	void Timer::Tick()
	{
		QueryPerformanceCounter(&_currCounter);

		_deltaTime = static_cast<float>(_currCounter.QuadPart - _prevCounter.QuadPart) /
			static_cast<float>(_frequency.QuadPart);

		_elapsedTime = static_cast<float>(_currCounter.QuadPart - _startCounter.QuadPart) /
			static_cast<float>(_frequency.QuadPart);

		_prevCounter = _currCounter;
	}
}