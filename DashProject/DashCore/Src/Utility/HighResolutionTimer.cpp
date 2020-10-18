#include "PCH.h"

#include "HighResolutionTimer.h"

namespace Dash
{
	FHighResolutionTimer::FHighResolutionTimer()
		: mElapsedTime(0), mDeltaTime(0)
	{
		mStart = std::chrono::steady_clock::now();
	}

	void FHighResolutionTimer::Update()
	{
		//double currCount = (std::chrono::duration<double, std::micro>(std::chrono::steady_clock::now() - mStart)).count();
		double currCount = (std::chrono::duration<double>(std::chrono::steady_clock::now() - mStart)).count();
		mDeltaTime = currCount - mElapsedTime;
		mElapsedTime = currCount;
	}

	double FHighResolutionTimer::ElapsedSeconds() const
	{
		//return mElapsedTime * 0.000001;
		return mElapsedTime;
	}

	double FHighResolutionTimer::ElapsedMilliSeconds() const
	{
		return mElapsedTime * 0.001;
	}

	double FHighResolutionTimer::ElapsedMicroSeconds() const
	{
		return mElapsedTime;
	}

	double FHighResolutionTimer::DeltaSeconds() const
	{
		//return mDeltaTime * 0.000001;
		return mDeltaTime;
	}

	double FHighResolutionTimer::DeltaMilliSeconds() const
	{
		return mDeltaTime * 0.001;
	}

	double FHighResolutionTimer::DeltaMicroSeconds() const
	{
		return mDeltaTime;
	}
}