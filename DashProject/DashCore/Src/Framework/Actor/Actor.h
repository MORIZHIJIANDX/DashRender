#pragma once

#include "Framework/Component/Component.h"

namespace Dash
{
	class TActor
	{
	public:
		TActor(const std::string& name);
		virtual ~TActor();

		template<typename Type>
		Type* AddComponent(const std::string& name)
		{
			std::unique_ptr<Type> newComponent = std::make_unique<Type>(name, this);
			Type* result = newComponent.get();
			mComponents.emplace_back(std::move(newComponent));

			if (mRootComponent == nullptr)
			{
				mRootComponent = result;
			}

			return result;
		}

		template<typename Type>
		std::vector<Type*> GetComponentsByClass() const
		{
			std::vector<Type*> result;

			for (auto& component : mComponents)
			{
				Type* componentOfClass = dynamic_cast<Type*>(component.get());
				if (componentOfClass)
				{
					result.emplace_back(componentOfClass);
				}
			}

			return result;
		}

		std::vector<TComponent*> GetComponents() const;

		TComponent* GetRootComponent() const;

		bool RemoveComponent(TComponent* component);

		virtual void Tick(float deltaTime);

		void SetActorWorldPosition(const FVector3f& p);
		void SetActorWorldRotation(const FQuaternion& q);
		void SetActorWorldScale(const FVector3f& s);

		FVector3f GetActorWorldPosition() const;
		FQuaternion GetActorWorldRotation() const;
		FVector3f GetActorWorldScale() const;

		void SetActorWorldTransform(const FTransform& transform);
		const FTransform& GetActorWorldTransform() const;

		void SetActorRelativePosition(const FVector3f& p);
		void SetActorRelativeRotation(const FQuaternion& q);
		void SetActorRelativeScale(const FVector3f& s);

		FVector3f GetActorRelativePosition() const;
		FQuaternion GetActorRelativeRotation() const;
		FVector3f GetActorRelativeScale() const;

		void SetActorRelativeTransform(const FTransform& relativeTransform);
		const FTransform& GetActorRelativeTransform() const;

		void AttachToActor(TActor* parent, const FTransform& relativeTransform = FIdentity{});
		TActor* GetAttachParentActor() const;

	private:
		std::string mName;

		std::vector<std::unique_ptr<TComponent>> mComponents;
		TComponent* mRootComponent = nullptr;
	};
}