#pragma once

#include <chrono>

namespace Dash
{
	class FHighResolutionTimer
	{
	public:
		FHighResolutionTimer();
		~FHighResolutionTimer() = default;

		void Update();

		double ElapsedSeconds() const;

		double ElapsedMilliSeconds() const;

		double ElapsedMicroSeconds() const;

		double DeltaSeconds() const;

		double DeltaMilliSeconds() const;

		double DeltaMicroSeconds() const;

	private:
		std::chrono::steady_clock::time_point mStart;
		double mElapsedTime, mDeltaTime;
	};
}