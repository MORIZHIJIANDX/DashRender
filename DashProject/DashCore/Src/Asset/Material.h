#pragma once
#include "Graphics/ShaderTechnique.h"

namespace Dash
{
	class FMaterial
	{
	public:
		FMaterial(const std::string& name, FShaderTechniqueRef shaderTechnique) 
			: mName(name)
			, mShaderTechnique(shaderTechnique)
		{};

		~FMaterial() {};

		bool SetTextureParameter() {};
		bool SetConstantParameter() {};

	private:
		std::string mName;
		FShaderTechniqueRef mShaderTechnique;
	};
}