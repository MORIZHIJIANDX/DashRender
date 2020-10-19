#pragma once

namespace Dash
{
	class FSystemTimer
	{
	public:
		static void Initialize();
		
		static int64_t GetCurrentTick();

		static double TicksToSecond(int64_t tickCount);

		static double TicksToMillisecond(int64_t tickCount);

		static double TimeBetweenTick(int64_t start, int64_t end);

	private:
		static double mSecondPerTick;
	};

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
		int64_t mBaseTick;
		int64_t mPauseTicks;
		int64_t mStopTick;
		int64_t mPrevTick;
		int64_t mCurrentTick;

		double mSecondPerTick;
		double mDeltaTime;

		bool mStopped;
	};
}