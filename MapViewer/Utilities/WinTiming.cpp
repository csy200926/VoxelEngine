#include "WinTiming.h"
#include <Windows.h>
namespace Timing
{
	static double LastFrameTime_ms = 0;
    double WinTiming::getCurenntFrameTime_ms()
	{
		LARGE_INTEGER LI;
		LONGLONG ticks;
		double ticksPerSec,currentFrameTimeMS = 0;

		QueryPerformanceFrequency(&LI);
		ticksPerSec = (double)LI.QuadPart;
		QueryPerformanceCounter(&LI);
		ticks = LI.QuadPart;

		currentFrameTimeMS = ticks / ticksPerSec;

		return currentFrameTimeMS;
	}

	double WinTiming::getLastFrameTime_ms()
	{
		return LastFrameTime_ms;
	}

	double WinTiming::DeltaTime()
	{
		return WinTiming::getCurenntFrameTime_ms() - LastFrameTime_ms;
	}

	void WinTiming::setLastFrameTime_ms(double i_time_ms)
	{
		LastFrameTime_ms = i_time_ms;
	}
}