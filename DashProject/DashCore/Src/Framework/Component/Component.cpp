#include "PCH.h"
#include "Component.h"

namespace Dash
{
	TComponent::TComponent(const std::string& name, TActor* owner)
		: mName(name)
		, mComponentToWorld(FIdentity{})
		, mRelativeTransform(FIdentity{})
		, mAttachParent(nullptr)
		, mOwner(owner)
	{
	}

	TComponent::~TComponent()
	{
	}

	void TComponent::TickComponent(float deltaTime)
	{
		
	}

	void TComponent::SetWorldPosition(const FVector3f& p)
	{
		mComponentToWorld.SetPosition(p);
	}

	void TComponent::SetWorldRotation(const FQuaternion& q)
	{
		mComponentToWorld.SetRotation(q);
	}

	void TComponent::SetWorldScale(const FVector3f& s)
	{
		mComponentToWorld.SetScale(s);
	}

	FVector3f TComponent::GetWorldPosition() const
	{
		return mComponentToWorld.GetPosition();
	}

	FQuaternion TComponent::GetWorldRotation() const
	{
		return mComponentToWorld.GetRotation();
	}

	FVector3f TComponent::GetWorldScale() const
	{
		return mComponentToWorld.GetScale();
	}

	void TComponent::SetWorldTransform(const FTransform& transform)
	{
		mComponentToWorld = transform;
	}

	const FTransform& TComponent::GetWorldTransform() const
	{
		return mComponentToWorld;
	}

	void TComponent::SetRelativePosition(const FVector3f& p)
	{
		mRelativeTransform.SetPosition(p);
	}

	void TComponent::SetRelativeRotation(const FQuaternion& q)
	{
		mRelativeTransform.SetRotation(q);
	}

	void TComponent::SetRelativeScale(const FVector3f& s)
	{
		mRelativeTransform.SetScale(s);
	}

	FVector3f TComponent::GetRelativePosition() const
	{
		return mRelativeTransform.GetPosition();
	}

	FQuaternion TComponent::GetRelativeRotation() const
	{
		return mRelativeTransform.GetRotation();
	}

	FVector3f TComponent::GetRelativeScale() const
	{
		return mRelativeTransform.GetScale();
	}

	void TComponent::SetRelativeTransform(const FTransform& transform)
	{
		mRelativeTransform = transform;
	}

	const FTransform& TComponent::GetRelativeTransform() const
	{
		return mRelativeTransform;
	}

	void TComponent::AttachToComponent(TComponent* parent, const FTransform& relativeTransform)
	{
		if (parent)
		{
			mAttachParent = parent;
			mRelativeTransform = relativeTransform;
		}
	}

	void TComponent::UpdateChildTransforms()
	{
		for (TComponent* child : mAttachChildren)
		{
			child->UpdateComponentToWorldWithParent(this);
		}
	}

	void TComponent::UpdateComponentToWorldWithParent(TComponent* parent)
	{
		if (parent)
		{
			mComponentToWorld = mRelativeTransform * parent->GetWorldTransform();

			UpdateChildTransforms();
		}
	}
}