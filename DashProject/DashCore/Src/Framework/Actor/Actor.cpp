#include "PCH.h"
#include "Actor.h"

namespace Dash
{
	TActor::TActor(const std::string& name)
		: mName(name)
	{
	}

	TActor::~TActor()
	{
	}

	std::vector<TComponent*> TActor::GetComponents() const
	{
		std::vector<TComponent*> result;

		for (auto& component : mComponents)
		{
			result.emplace_back(component.get());
		}

		return result;
	}

	TComponent* TActor::GetRootComponent() const
	{
		return mRootComponent;
	}

	bool TActor::RemoveComponent(TComponent* component)
	{
		if (component)
		{
			if (component == mRootComponent)
			{
				mRootComponent = nullptr;
			}

			for (auto iter = mComponents.begin(); iter != mComponents.end(); iter++)
			{
				if (iter->get() == component)
				{
					mComponents.erase(iter);
					return true;
				}
			}
		}

		return false;
	}

	void TActor::Tick(float deltaTime)
	{
		for (auto& component : mComponents)
		{
			component->TickComponent(deltaTime);
		}
	}

	void TActor::SetActorWorldPosition(const FVector3f& p)
	{
		if (mRootComponent)
		{
			mRootComponent->SetWorldPosition(p);
		}
	}

	void TActor::SetActorWorldRotation(const FQuaternion& q)
	{
		if (mRootComponent)
		{
			mRootComponent->SetWorldRotation(q);
		}
	}

	void TActor::SetActorWorldScale(const FVector3f& s)
	{
		if (mRootComponent)
		{
			mRootComponent->SetWorldScale(s);
		}
	}

	FVector3f TActor::GetActorWorldPosition() const
	{
		if (mRootComponent)
		{
			return mRootComponent->GetWorldPosition();
		}

		return FVector3f{FIdentity{}};
	}

	FQuaternion TActor::GetActorWorldRotation() const
	{
		if (mRootComponent)
		{
			return mRootComponent->GetWorldRotation();
		}

		return FQuaternion{ FIdentity{} };
	}

	FVector3f TActor::GetActorWorldScale() const
	{
		if (mRootComponent)
		{
			return mRootComponent->GetWorldScale();
		}

		return FVector3f{ FIdentity{} };
	}

	void TActor::SetActorWorldTransform(const FTransform& transform)
	{
		if (mRootComponent)
		{
			mRootComponent->SetWorldTransform(transform);
		}
	}

	const FTransform& TActor::GetActorWorldTransform() const
	{
		if (mRootComponent)
		{
			return mRootComponent->GetWorldTransform();
		}

		return FTransform::Identity;
	}

	void TActor::SetActorRelativePosition(const FVector3f& p)
	{
		if (mRootComponent)
		{
			mRootComponent->SetRelativePosition(p);
		}
	}

	void TActor::SetActorRelativeRotation(const FQuaternion& q)
	{
		if (mRootComponent)
		{
			mRootComponent->SetRelativeRotation(q);
		}
	}

	void TActor::SetActorRelativeScale(const FVector3f& s)
	{
		if (mRootComponent)
		{
			mRootComponent->SetRelativeScale(s);
		}
	}

	FVector3f TActor::GetActorRelativePosition() const
	{
		if (mRootComponent)
		{
			return mRootComponent->GetRelativePosition();
		}

		return FVector3f{ FIdentity{} };
	}

	FQuaternion TActor::GetActorRelativeRotation() const
	{
		if (mRootComponent)
		{
			return mRootComponent->GetRelativeRotation();
		}

		return FQuaternion{ FIdentity{} };
	}

	FVector3f TActor::GetActorRelativeScale() const
	{
		if (mRootComponent)
		{
			return mRootComponent->GetRelativeScale();
		}

		return FVector3f{ FIdentity{} };
	}

	void TActor::SetActorRelativeTransform(const FTransform& transform)
	{
		if (mRootComponent)
		{
			mRootComponent->SetRelativeTransform(transform);
		}
	}

	const FTransform& TActor::GetActorRelativeTransform() const
	{
		if (mRootComponent)
		{
			return mRootComponent->GetRelativeTransform();
		}

		return FTransform::Identity;
	}

	void TActor::AttachToActor(TActor* parent, const FTransform& relativeTransform)
	{
		if (mRootComponent && parent && parent->GetRootComponent())
		{
			mRootComponent->AttachToComponent(parent->GetRootComponent(), relativeTransform);
		}
	}

	TActor* TActor::GetAttachParentActor() const
	{
		if (mRootComponent && mRootComponent->GetAttachParent())
		{
			return mRootComponent->GetAttachParent()->GetOwner();
		}

		return nullptr;
	}
}


