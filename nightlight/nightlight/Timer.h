#pragma once
#include <windows.h>
#include <stdexcept>

class Timer
{
private:

	INT64 frequency;
	double ticksPerMs;
	INT64 startTime;
	double frameTime;
	double gameTime;

public:

	Timer();
	~Timer();

	void Update();

	double GetFrameTime();
	double GetGameTime();

	void Reset();
};

