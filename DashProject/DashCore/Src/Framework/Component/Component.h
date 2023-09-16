#pragma once

namespace Dash
{
	class TActor;

	class TComponent
	{
	public:
		TComponent(const std::string& name, TActor* owner);
		virtual ~TComponent();

		virtual void TickComponent(float deltaTime);

		void SetWorldPosition(const FVector3f& p);
		void SetWorldRotation(const FQuaternion& q);
		void SetWorldScale(const FVector3f& s);

		FVector3f GetWorldPosition() const;
		FQuaternion GetWorldRotation() const;
		FVector3f GetWorldScale() const;

		void SetWorldTransform(const FTransform& transform);
		const FTransform& GetWorldTransform() const;

		void SetRelativePosition(const FVector3f& p);
		void SetRelativeRotation(const FQuaternion& q);
		void SetRelativeScale(const FVector3f& s);

		FVector3f GetRelativePosition() const;
		FQuaternion GetRelativeRotation() const;
		FVector3f GetRelativeScale() const;

		void SetRelativeTransform(const FTransform& transform);
		const FTransform& GetRelativeTransform() const;

		void AttachToComponent(TComponent* parent, const FTransform& relativeTransform = FIdentity{});

		const std::vector<TComponent*>& GetAttachChildren() const { return mAttachChildren; }
		TActor* GetOwner() const { return mOwner; }
		TComponent* GetAttachParent() const { return mAttachParent; }

	protected:

		void UpdateChildTransforms();
		void UpdateComponentToWorldWithParent(TComponent* parent);

	protected:
		std::string mName;

		FTransform mComponentToWorld;
		FTransform mRelativeTransform;

		TComponent* mAttachParent;
		TActor* mOwner;
		std::vector<TComponent*> mAttachChildren;
	};
}