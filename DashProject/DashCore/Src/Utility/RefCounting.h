#pragma once

#include "Utility/Assert.h"

namespace Dash
{
	template<typename ReferencedType>
	class TRefCountPtr
	{
		typedef ReferencedType* ReferencePtr;
		// 允许不同实例互相访问私有成员（以便移动时直接窃取指针）
		template<typename> friend class TRefCountPtr;
	public:
		TRefCountPtr()
			: mReference(nullptr)
		{}

		TRefCountPtr(ReferencedType* InReference, bool bAddRef = true)
		{
			mReference = InReference;
			if (mReference && bAddRef)
			{
				mReference->AddRef();
			}
		}

		TRefCountPtr(const TRefCountPtr& Copy)
		{
			mReference = Copy.mReference;
			if (mReference)
			{
				mReference->AddRef();
			}
		}

		TRefCountPtr(TRefCountPtr&& Copy)
		{
			mReference = Copy.mReference;
			Copy.mReference = nullptr;
		}

		// 模板化的转换构造（允许从 TRefCountPtr<Other> 到 TRefCountPtr<ReferencedType> 的转换）
		template<typename OtherType, typename = typename std::enable_if<std::is_convertible<OtherType*, ReferencedType*>::value>::type>
		TRefCountPtr(const TRefCountPtr<OtherType>& Other)
		{
			mReference = Other.mReference;
			if (mReference)
			{
				mReference->AddRef();
			}
		}

		// 模板化的移动构造（窃取指针）
		template<typename OtherType, typename = typename std::enable_if<std::is_convertible<OtherType*, ReferencedType*>::value>::type>
		TRefCountPtr(TRefCountPtr<OtherType>&& Other)
		{
			mReference = Other.mReference;
			Other.mReference = nullptr;
		}

		~TRefCountPtr()
		{
			if (mReference)
			{
				mReference->Release();
			}
		}

		TRefCountPtr& operator=(ReferencedType* InReference)
		{
			if (mReference != InReference)
			{
				// Call AddRef before Release, in case the new reference is the same as the old reference.
				ReferencedType* OldReference = mReference;
				mReference = InReference;
				if (mReference)
				{
					mReference->AddRef();
				}
				if (OldReference)
				{
					OldReference->Release();
				}
			}
			return *this;
		}

		TRefCountPtr& operator=(const TRefCountPtr& InPtr)
		{
			return *this = InPtr.mReference;
		}

		TRefCountPtr& operator=(TRefCountPtr&& InPtr)
		{
			if (this != &InPtr)
			{
				ReferencedType* OldReference = mReference;
				mReference = InPtr.mReference;
				InPtr.mReference = nullptr;
				if (OldReference)
				{
					OldReference->Release();
				}
			}
			return *this;
		}

		// 模板化的转换赋值（copy）
		template<typename OtherType, typename = typename std::enable_if<std::is_convertible<OtherType*, ReferencedType*>::value>::type>
		TRefCountPtr& operator=(const TRefCountPtr<OtherType>& Other)
		{
			return *this = Other.mReference;
		}

		// 模板化的转换赋值（move）
		template<typename OtherType, typename = typename std::enable_if<std::is_convertible<OtherType*, ReferencedType*>::value>::type>
		TRefCountPtr& operator=(TRefCountPtr<OtherType>&& Other)
		{
			if (mReference != Other.mReference)
			{
				ReferencedType* OldReference = mReference;
				mReference = Other.mReference;
				Other.mReference = nullptr;
				if (OldReference)
				{
					OldReference->Release();
				}
			}
			return *this;
		}

		ReferencedType* operator->() const
		{
			return mReference;
		}

		operator ReferencePtr() const
		{
			return mReference;
		}

		ReferencedType** GetInitReference()
		{
			*this = nullptr;
			return &mReference;
		}

		ReferencedType* GetReference() const
		{
			return mReference;
		}

		friend bool IsValidRef(const TRefCountPtr& InReference)
		{
			return InReference.mReference != nullptr;
		}

		bool IsValid() const
		{
			return mReference != nullptr;
		}

		uint32 SafeRelease()
		{
			uint32 refCount = 0;
			if (mReference != nullptr)
			{
				ReferencedType* OldReference = mReference;
				mReference = nullptr;
				if (OldReference)
				{
					refCount = OldReference->Release();
				}
			}
			return refCount;
		}

		uint32 GetRefCount()
		{
			uint32 Result = 0;
			if (mReference)
			{
				Result = mReference->GetRefCount();
				ASSERT(Result > 0); // you should never have a zero ref count if there is a live ref counted pointer (*this is live)
			}
			return Result;
		}

		void Swap(TRefCountPtr& InPtr) // this does not change the reference count, and so is faster
		{
			ReferencedType* OldReference = mReference;
			mReference = InPtr.mReference;
			InPtr.mReference = OldReference;
		}

	private:
		ReferencedType* mReference;
	};

	template<typename ReferencedType>
	bool operator==(const TRefCountPtr<ReferencedType>& A, const TRefCountPtr<ReferencedType>& B)
	{
		return A.GetReference() == B.GetReference();
	}

	template<typename ReferencedType>
	bool operator==(const TRefCountPtr<ReferencedType>& A, ReferencedType* B)
	{
		return A.GetReference() == B;
	}

	template<typename ReferencedType>
	bool operator==(ReferencedType* A, const TRefCountPtr<ReferencedType>& B)
	{
		return A == B.GetReference();
	}

	template<typename T, typename... Args>
	TRefCountPtr<T> MakeRefCounted(Args&&... args)
	{
		return TRefCountPtr<T>(new T(std::forward<Args>(args)...), true);
	}
}