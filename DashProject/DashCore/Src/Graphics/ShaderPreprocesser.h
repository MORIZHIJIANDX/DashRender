#pragma once

namespace Dash
{
	struct FShaderPreprocessdResult
	{
		std::string ShaderCode;
		std::map<std::string, std::string> BindlessResourceMap;
	};

	class FShaderPreprocesser
	{
	public:
		static FShaderPreprocessdResult Process(const std::string& fileName);
	};
}