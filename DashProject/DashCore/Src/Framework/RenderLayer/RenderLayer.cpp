#include "PCH.h"
#include "RenderLayer.h"

namespace Dash
{
	IRenderLayer::IRenderLayer(const std::string& layerName, uint16 layerId)
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

	uint16 IRenderLayer::GetLayerId() const
	{
		return mLayerId;
	}
}
