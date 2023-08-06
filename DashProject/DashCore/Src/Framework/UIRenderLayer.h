#pragma once

#include "RenderLayer.h"

namespace Dash
{
	class FUIRenderLayer : public IRenderLayer
	{
	public:
		FUIRenderLayer();
		virtual ~FUIRenderLayer();
		
		virtual void Init() override;
		virtual void Shutdown() override;

		virtual void OnBeginFrame() override;
		virtual void OnEndFrame() override;
		virtual void OnUpdate(const FUpdateEventArgs& e) override;
		virtual void OnRender(const FRenderEventArgs& e) override;

		virtual void OnWindowResize(const FResizeEventArgs& e) override;
	};

}