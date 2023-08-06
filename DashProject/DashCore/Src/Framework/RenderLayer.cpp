#include "PCH.h"
#include "RenderLayer.h"

namespace Dash
{
	IRenderLayer::IRenderLayer(const std::string& layerName, uint16_t layerId)
		: mLayerName (layerName)
		, mLayerId (layerId)
	{
		
	}

	IRenderLayer::~IRenderLayer()
	{

	}

	const std::string& IRenderLayer::GetLayerName() const
	{
		return mLayerName;
	}

	uint16_t IRenderLayer::GetLayerId() const
	{
		return mLayerId;
	}
}
