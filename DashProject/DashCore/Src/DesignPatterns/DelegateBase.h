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

namespace Dash
{
	template<typename T>
	class TDelegateBase;

	template<typename RET, typename ...PARAMS>
	class TDelegateBase<RET(PARAMS...)> {

	protected:

		using stub_type = RET(*)(void* this_ptr, PARAMS...);

		struct InvocationElement {
			InvocationElement() = default;
			InvocationElement(void* this_ptr, stub_type aStub) : mObject(this_ptr), mStub(aStub) {}
			void Clone(InvocationElement& target) const {
				target.mStub = mStub;
				target.mObject = mObject;
			} //Clone
			bool operator ==(const InvocationElement& another) const {
				return another.mStub == mStub && another.mObject == mObject;
			} //==
			bool operator !=(const InvocationElement& another) const {
				return another.mStub != mStub || another.mObject != mObject;
			} //!=
			void* mObject = nullptr;
			stub_type mStub = nullptr;
		}; //InvocationElement

	}; //class TDelegateBase

} /* namespace Core */
