#include "PCH.h"
#include "SystemTimer.h"


namespace Dash
{
	double FSystemTimer::mSecondPerTick = 0.0;

	void FSystemTimer::Initialize()
	{
		LARGE_INTEGER frequency{};
		ASSERT_MSG(TRUE == QueryPerformanceFrequency(&frequency), "Unable to query performance frequency");
		mSecondPerTick = 1.0 / static_cast<double>(frequency.QuadPart);
	}

	int64_t FSystemTimer::GetCurrentTick()
	{
		LARGE_INTEGER currentTick{};
		ASSERT_MSG(TRUE == QueryPerformanceCounter(&currentTick), "Unable to query performance frequency");
		return static_cast<uint64_t>(currentTick.QuadPart);
	}

	double FSystemTimer::TicksToSecond(int64_t tickCount)
	{
		return tickCount * mSecondPerTick;
	}

	double FSystemTimer::TicksToMillisecond(int64_t tickCount)
	{
		return tickCount * mSecondPerTick * 1000.0;
	}

	double FSystemTimer::TimeBetweenTick(int64_t start, int64_t end)
	{
		return TicksToSecond(end - start);
	}


	//FCpuTimer

	void FCpuTimer::Tick()
	{
		if (mStopped)
		{
			mDeltaTime = 0.0;
			return;
		}

		mCurrentTick = FSystemTimer::GetCurrentTick();

		mDeltaTime = FSystemTimer::TimeBetweenTick(mPrevTick, mCurrentTick);

		mPrevTick = mCurrentTick;

		if (mDeltaTime < 0.0)
		{
			mDeltaTime = 0.0;
		}
	}

	void FCpuTimer::Start()
	{
		if (mStopped)
		{
			uint64_t currentTick = FSystemTimer::GetCurrentTick();

			mPauseTicks += currentTick - mStopTick;
			mPrevTick = currentTick;
			mStopTick = 0;
			mStopped = false;
		}
	}

	void FCpuTimer::Stop()
	{
		if (!mStopped)
		{
			uint64_t currentTick = FSystemTimer::GetCurrentTick();
			mStopTick = currentTick;
			mStopped = true;
		}
	}

	void FCpuTimer::Reset()
	{
		uint64_t currentTick = FSystemTimer::GetCurrentTick();
		mBaseTick = currentTick;
		mPrevTick = currentTick;
		mStopTick = 0;
		mPauseTicks = 0;
		mStopped = false;
	}

	float FCpuTimer::GetTotalTime() const
	{
		if (mStopped)
		{
			return static_cast<float>(FSystemTimer::TicksToSecond((mStopTick - mPauseTicks) - mBaseTick));
		}

		return static_cast<float>(FSystemTimer::TicksToSecond((mCurrentTick - mPauseTicks) - mBaseTick));
	}

	float FCpuTimer::GetDeltaTime() const
	{
		return static_cast<float>(mDeltaTime);
	}
}

