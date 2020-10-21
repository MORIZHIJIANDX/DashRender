#pragma once

#include "Graphics/GameApp.h"

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
		
		virtual void OnWindowResize(const FResizeEventArgs& e) override
		{
			LOG_INFO << "Window Resized, Wdith : " << e.mWidth << ", Height : " << e.mHeight << " , Minimized : " << e.mMinimized;
		}
	};
}

