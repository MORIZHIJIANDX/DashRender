#pragma once

namespace Dash
{
	class FThreadSafeCounter
	{
	public:
		FThreadSafeCounter()
		{
			mCounter = 0;
		}

		FThreadSafeCounter(const FThreadSafeCounter& Other)
		{
			mCounter = Other.GetValue();
		}

		FThreadSafeCounter(int32 Value)
		{
			mCounter = Value;
		}

		int32 Increment()
		{
			return mCounter.fetch_add(1);
		}

		int32 Add(int32 Amount)
		{
			return mCounter.fetch_add(Amount);
		}

		int32 Decrement()
		{
			return mCounter.fetch_sub(1);
		}

		int32 Subtract(int32 Amount)
		{
			return mCounter.fetch_sub(Amount);
		}

		int32 Set(int32 Value)
		{
			return mCounter.exchange(Value);
		}

		int32 Reset()
		{
			return mCounter.exchange(0);
		}

		int32 GetValue() const
		{
			return mCounter.load();
		}

	private:
		void operator=(const FThreadSafeCounter& Other) {}

		std::atomic<int32> mCounter;
	};
}