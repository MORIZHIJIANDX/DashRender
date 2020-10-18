#pragma once

#include <queue>
#include <mutex>

namespace Dash
{
	template<typename T>
	class TThreadSafeQueue
	{
	public:
		TThreadSafeQueue() noexcept;
		TThreadSafeQueue(const TThreadSafeQueue& copy) noexcept;

		/**
		 * Push a value into the back of the queue.
		 */
		void Push(T value) noexcept;

		/**
		 * Try to pop a value from the front of the queue.
		 * @returns false if the queue is empty.
		 */
		bool TryPop(T& value) noexcept;

		/**
		 * Check to see if there are any items in the queue.
		 */
		bool Empty() const noexcept;

		/**
		 * Retrieve the number of items in the queue.
		 */
		size_t Size() const noexcept;

	private:
		std::queue<T> mQueue;
		mutable std::mutex mMutex;
	};

	template<typename T>
	TThreadSafeQueue<T>::TThreadSafeQueue() noexcept
	{}

	template<typename T>
	TThreadSafeQueue<T>::TThreadSafeQueue(const TThreadSafeQueue<T>& copy) noexcept
	{
		std::lock_guard<std::mutex> lock(copy.mMutex);
		mQueue = copy.mQueue;
	}

	template<typename T>
	void TThreadSafeQueue<T>::Push(T value) noexcept
	{
		std::lock_guard<std::mutex> lock(mMutex);
		mQueue.push(std::move(value));
	}

	template<typename T>
	bool TThreadSafeQueue<T>::TryPop(T& value) noexcept
	{
		std::lock_guard<std::mutex> lock(mMutex);
		if (mQueue.empty())
			return false;

		value = mQueue.front();
		mQueue.pop();

		return true;
	}

	template<typename T>
	bool TThreadSafeQueue<T>::Empty() const noexcept
	{
		std::lock_guard<std::mutex> lock(mMutex);
		return mQueue.empty();
	}

	template<typename T>
	size_t TThreadSafeQueue<T>::Size() const noexcept
	{
		std::lock_guard<std::mutex> lock(mMutex);
		return mQueue.size();
	}
}