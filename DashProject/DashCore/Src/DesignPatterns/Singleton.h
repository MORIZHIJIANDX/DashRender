#pragma once

#include <memory>
#include <mutex>

namespace Dash
{
	template<typename T>
	class TSingleton
	{
	private:
		TSingleton<T>(const TSingleton<T>&) = delete;
		TSingleton<T>& operator = (const TSingleton<T>&) = delete;

	protected:
		TSingleton<T>() = default;

		static std::unique_ptr<T> mInstance;
		static std::once_flag mOnce;

	public:
		virtual ~TSingleton<T>() = default;

		template<typename... Args>
		static T* Get(Args&& ...args)
		{
			std::call_once(mOnce, [&]() {
				mInstance.reset(new T(std::forward<Args>(args)...));
			});
			return mInstance.get();
		}
	};

	template<typename T> std::unique_ptr<T> TSingleton<T>::mInstance;
	template<typename T> std::once_flag TSingleton<T>::mOnce;

}