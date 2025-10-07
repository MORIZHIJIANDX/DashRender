#pragma once

#include "Utility/Assert.h"

namespace Dash
{
	template<typename ReferencedType>
	class TRefCountPtr
	{
		typedef ReferencedType* ReferencePtr;
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

		void SafeRelease()
		{
			*this = nullptr;
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
}