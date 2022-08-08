#pragma once

#include "Graphics/GameApp.h"
#include <Graphics/Camera.h>

namespace Dash
{
	class TestApplication : public IGameApp
	{
	public:
		TestApplication ();
		virtual ~TestApplication ();

		virtual void Startup(void) override;
		virtual void Cleanup(void) override;

		virtual void OnUpdate(const FUpdateEventArgs& e) override;

		virtual void OnRenderScene(const FRenderEventArgs& e) override;
		
		virtual void OnWindowResize(const FResizeEventArgs& e) override;

		void OnMouseWheelDown(FMouseWheelEventArgs& e);
		void OnMouseWheelUp(FMouseWheelEventArgs& e);

		void OnMouseMove(FMouseMotionEventArgs& e);

	private: 
		std::shared_ptr<FPerspectiveCamera> mCamera;

		FMouseWheelEventDelegate OnMouseWheelDownDelegate;
		FMouseWheelEventDelegate OnMouseWheelUpDelegate;
		FMouseMotionEventDelegate OnMouseMoveDelegate;
	};
}

