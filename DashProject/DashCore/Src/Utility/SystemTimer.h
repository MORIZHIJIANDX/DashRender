#pragma once

namespace Dash
{
	using FHighResolutionClock = std::chrono::high_resolution_clock;

	class FCpuTimer
	{
	public:
		void Tick();
		void Start();
		void Stop();
		void Reset();

		float GetTotalTime() const;
		float GetDeltaTime() const;

	private:
		FHighResolutionClock::time_point mBaseTick;
		FHighResolutionClock::time_point mPauseTicks;
		FHighResolutionClock::time_point mStopTick;
		FHighResolutionClock::time_point mPrevTick;
		FHighResolutionClock::time_point mCurrentTick;

		double mDeltaTime = 0.0;
		double mTotalTime = 0.0;

		bool mStopped;
	};
}