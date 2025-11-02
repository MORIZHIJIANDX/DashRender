#include "PCH.h"
#include "GraphicsDefines.h"

namespace Dash
{
	const char* ShaderStageToString(EShaderStage stage)
	{
		switch (stage)
		{
		case EShaderStage::Vertex:   return "Vertex";
		case EShaderStage::Hull:     return "Hull";
		case EShaderStage::Domain:   return "Domain";
		case EShaderStage::Geometry: return "Geometry";
		case EShaderStage::Pixel:    return "Pixel";
		case EShaderStage::Compute:  return "Compute";
		default:                     return "Unknown";
		}
	}
}

