#include "PCH.h"
#include "SystemTimer.h"


namespace Dash
{
	//FCpuTimer

	void FCpuTimer::Tick()
	{
		if (mStopped)
		{
			mDeltaTime = 0.0;
			return;
		}

		mCurrentTick = FHighResolutionClock::now();

		mDeltaTime = std::chrono::duration_cast<std::chrono::duration<double>>(mCurrentTick - mPrevTick).count();
		
		mPrevTick = mCurrentTick;

		if (mDeltaTime < 0.0)
		{
			mDeltaTime = 0.0;
		}

		mTotalTime += mDeltaTime;
	}

	void FCpuTimer::Start()
	{
		if (mStopped)
		{
			auto currentTick = FHighResolutionClock::now();

			mPauseTicks += currentTick - mStopTick;
			mPrevTick = currentTick;
			mStopTick = FHighResolutionClock::time_point{};
			mStopped = false;
		}
	}

	void FCpuTimer::Stop()
	{
		if (!mStopped)
		{
			auto currentTick = FHighResolutionClock::now();
			mStopTick = currentTick;
			mStopped = true;
		}
	}

	void FCpuTimer::Reset()
	{
		auto currentTick = FHighResolutionClock::now();
		mBaseTick = currentTick;
		mPrevTick = currentTick;
		mStopTick = FHighResolutionClock::time_point{};;
		mPauseTicks = FHighResolutionClock::time_point{};;
		mStopped = false;
	}

	float FCpuTimer::GetTotalTime() const
	{
		return static_cast<float>(mTotalTime);
	}

	float FCpuTimer::GetDeltaTime() const
	{
		return static_cast<float>(mDeltaTime);
	}
}

