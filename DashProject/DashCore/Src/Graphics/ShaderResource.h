#pragma once

#include "Utility/FileUtility.h"

namespace Dash
{
	struct FShaderCreationInfo
	{
	public:
		FShaderCreationInfo(){}

		FShaderCreationInfo(const std::string& fileName, const std::string& entryPoint, const std::vector<std::string>& defines = {})
			: FileName(fileName)
			, EntryPoint(entryPoint)
			, Defines(defines)
		{}

		void Finalize();
		size_t GetShaderHash() const { return ShaderHash; }
		std::string GetShaderTarget() const { return ShaderTarget; }
		std::string GetHashedFileName() const { return HashedFileName; }
		bool IsOutOfDate() const;

		std::string FileName;
		std::string EntryPoint;
		std::vector<std::string> Defines;

	protected:
		void ComputeShaderTargetFromEntryPoint();

	protected:
		std::string ShaderTarget;
		std::string HashedFileName;
		size_t ShaderHash = 0;
	};

	struct FShaderResource
	{
		FShaderCreationInfo CreationInfo;
		FileUtility::ByteArray ShaderCode = FileUtility::NullFile;
	};
}