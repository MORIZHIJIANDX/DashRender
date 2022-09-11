/*

	Copyright (C) 2017 by Sergey A Kryukov: derived work
	http://www.SAKryukov.org
	http://www.codeproject.com/Members/SAKryukov

	Based on original work by Sergey Ryazanov:
	"The Impossibly Fast C++ Delegates", 18 Jul 2005
	https://www.codeproject.com/articles/11015/the-impossibly-fast-c-delegates

	MIT license:
	http://en.wikipedia.org/wiki/MIT_License

	Original publication: https://www.codeproject.com/Articles/1170503/The-Impossibly-Fast-Cplusplus-Delegates-Fixed

*/

#pragma once
#include "Delegate.h"
#include <list>
#include <functional>

namespace Dash
{
	template<typename RET, typename ...PARAMS>
	class TMulticastDelegate<RET(PARAMS...)> final : private TDelegateBase<RET(PARAMS...)> {
	public:

		TMulticastDelegate() = default;
		~TMulticastDelegate() {
			Clear();
		} //~TMulticastDelegate

		bool IsNull() const { return mInvocation.size() < 1; }
		bool operator ==(void* ptr) const {
			return (ptr == nullptr) && this->IsNull();
		} //operator ==
		bool operator !=(void* ptr) const {
			return (ptr != nullptr) || (!this->IsNull());
		} //operator !=

		size_t Num() const { return mInvocation.size(); }

		void Clear() 
		{
			for (auto& element : mInvocation) delete element;
			mInvocation.clear();
		};

		TMulticastDelegate& operator =(const TMulticastDelegate&) = delete;
		TMulticastDelegate(const TMulticastDelegate&) = delete;

		bool operator ==(const TMulticastDelegate& another) const {
			if (mInvocation.size() != another.mInvocation.size()) return false;
			auto anotherIt = another.mInvocation.begin();
			for (auto it = mInvocation.begin(); it != mInvocation.end(); ++it)
				if (**it != **anotherIt) return false;
			return true;
		} //==
		bool operator !=(const TMulticastDelegate& another) const { return !(*this == another); }

		bool operator ==(const TDelegate<RET(PARAMS...)>& another) const {
			if (IsNull() && another.IsNull()) return true;
			if (another.IsNull() || (Num() != 1)) return false;
			return (another.mInvocation == **mInvocation.begin());
		} //==
		bool operator !=(const TDelegate<RET(PARAMS...)>& another) const { return !(*this == another); }

		TMulticastDelegate& operator +=(const TMulticastDelegate& another) {
			for (auto& item : another.mInvocation) // clone, not copy; flattens hierarchy:
				this->mInvocation.push_back(new typename TDelegateBase<RET(PARAMS...)>::InvocationElement(item->mObject, item->mStub));
			return *this;
		} //operator +=

		template <typename LAMBDA> // template instantiation is not neededm, will be deduced/inferred:
		TMulticastDelegate& operator +=(const LAMBDA& lambda) {
			TDelegate<RET(PARAMS...)> d = TDelegate<RET(PARAMS...)>::template Create<LAMBDA>(lambda);
			return *this += d;
		} //operator +=

		TMulticastDelegate& operator +=(const TDelegate<RET(PARAMS...)>& another) {
			if (another.IsNull()) return *this;
			this->mInvocation.push_back(new typename TDelegateBase<RET(PARAMS...)>::InvocationElement(another.mInvocation.mObject, another.mInvocation.mStub));
			return *this;
		} //operator +=


		template <typename LAMBDA> // template instantiation is not neededm, will be deduced/inferred:
		TMulticastDelegate& operator -=(const LAMBDA& lambda) {
			TDelegate<RET(PARAMS...)> d = TDelegate<RET(PARAMS...)>::template Create<LAMBDA>(lambda);
			return *this -= d;
		} //operator +=

		TMulticastDelegate& operator -=(const TDelegate<RET(PARAMS...)>& another) {
			if (another.IsNull() || this->IsNull()) return *this;

			auto it = mInvocation.begin();
			for (; it != mInvocation.end(); ++it)
			{
				if ((**it) == another.mInvocation)
				{
					break;
				}
			}
			delete* it;
			mInvocation.erase(it);

			return *this;
		} //operator -=


		// will work even if RET is void, return values are ignored:
		// (for handling return values, see operator(..., handler))
		void operator()(PARAMS... arg) const {
			for (auto& item : mInvocation)
				(*(item->mStub))(item->mObject, arg...);
		} //operator()

		template<typename HANDLER>
		void operator()(PARAMS... arg, HANDLER handler) const {
			size_t index = 0;
			for (auto& item : mInvocation) {
				RET value = (*(item->mStub))(item->mObject, arg...);
				handler(index, &value);
				++index;
			} //loop
		} //operator()

		void operator()(PARAMS... arg, TDelegate<void(size_t, RET*)> handler) const {
			operator() < decltype(handler) > (arg..., handler);
		} //operator()
		void operator()(PARAMS... arg, std::function<void(size_t, RET*)> handler) const {
			operator() < decltype(handler) > (arg..., handler);
		} //operator()

	private:

		std::list<typename TDelegateBase<RET(PARAMS...)>::InvocationElement*> mInvocation;

	}; //class TMulticastDelegate

} /* namespace Core */

