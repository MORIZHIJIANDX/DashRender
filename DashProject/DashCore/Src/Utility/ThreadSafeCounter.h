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
			return mCounter.fetch_add(1) + 1;
		}

		int32 Add(int32 Amount)
		{
			return mCounter.fetch_add(Amount) + Amount;
		}

		int32 Decrement()
		{
			return mCounter.fetch_sub(1) - 1;
		}

		int32 Subtract(int32 Amount)
		{
			return mCounter.fetch_sub(Amount) - Amount;
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

	class FRefCount
	{
	public:
		FRefCount() {}
		virtual ~FRefCount()
		{
			ASSERT(mNumRefs.GetValue() == 0);
		}

		uint32 AddRef() const
		{
			int32 newValue = mNumRefs.Increment();
			ASSERT(newValue > 0);
			return uint32(newValue);
		}

		uint32 Release() const
		{
			int32 newValue = mNumRefs.Decrement();
			if (newValue == 0)
			{
				delete this;
			}
			ASSERT(newValue >= 0);
			return uint32(newValue);
		}

		uint32 RefCount() const
		{
			int32 currentValue = mNumRefs.GetValue();
			ASSERT(currentValue >= 0);
			return uint32(currentValue);
		}

	private:
		mutable FThreadSafeCounter mNumRefs;
	};
}