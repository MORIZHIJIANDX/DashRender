#pragma once

#include "Utility/Events.h"
#include "Graphics/CommandContext.h"

namespace Dash
{
	class IRenderLayer
	{
	public:
		IRenderLayer(const std::string& layerName, uint16_t layerId);
		virtual ~IRenderLayer();

		const std::string& GetLayerName() const;
		uint16_t GetLayerId() const;

		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		virtual void OnBeginFrame() = 0;
		virtual void OnEndFrame() = 0;

		virtual void OnUpdate(const FUpdateEventArgs& e) = 0;
		virtual void OnRender(const FRenderEventArgs& e) = 0;

		virtual void OnWindowResize(const FWindowResizeEventArgs& e) = 0;

	protected:
		
		std::string mLayerName;
		uint16_t mLayerId;
	};
}