#pragma once

#include "RenderLayer.h"
#include "Framework/Actor/CameraActor.h"
#include "Framework/Actor/StaticMeshActor.h"

namespace Dash
{
	class FSceneRenderLayer : public IRenderLayer
	{
	public:
		FSceneRenderLayer();
		virtual ~FSceneRenderLayer();

		virtual void Init() override;
		virtual void Shutdown() override;

		virtual void OnBeginFrame() override;
		virtual void OnEndFrame() override;
		virtual void OnUpdate(const FUpdateEventArgs& e) override;
		virtual void OnRender(const FRenderEventArgs& e) override;

		virtual void OnWindowResize(const FResizeEventArgs& e) override;

		void OnMouseWheelDown(FMouseWheelEventArgs& e);
		void OnMouseWheelUp(FMouseWheelEventArgs& e);
		void OnMouseMove(FMouseMotionEventArgs& e);

	protected:
		void UpdateCamera(Scalar translate);

	private:
		std::shared_ptr<TCameraActor> mCameraActor;
		std::shared_ptr<TStaticMeshActor> mStaticMeshActor;
		TPerspectiveCameraComponent* mPerspectiveCamera;

		FStructuredBufferRef mInstanceBuffer;

		FMouseWheelEventDelegate OnMouseWheelDownDelegate;
		FMouseWheelEventDelegate OnMouseWheelUpDelegate;
		FMouseMotionEventDelegate OnMouseMoveDelegate;
	};
}