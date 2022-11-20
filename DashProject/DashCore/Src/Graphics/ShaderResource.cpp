#include "PCH.h"
#include "ShaderResource.h"
#include "Utility/StringUtility.h"

namespace Dash
{
	void FShaderCreationInfo::Finalize()
	{
		std::string hasedName = FileUtility::RemoveExtension(FileName) + "_" + EntryPoint;

		std::sort(Defines.begin(), Defines.end());
		for (const std::string& define : Defines)
		{
			hasedName = hasedName + "_" + define;
		}

		ShaderHash = std::hash<std::string>{}(hasedName);
		HashedFileName = hasedName + "_" + FStringUtility::ToString(ShaderHash) + ".cso";

		ComputeShaderTargetFromEntryPoint();
	}

	bool FShaderCreationInfo::IsOutOfDate() const
	{
		return !FileUtility::IsPathExistent(GetHashedFileName()) || FileUtility::GetFileLastWriteTime(FileName) > FileUtility::GetFileLastWriteTime(GetHashedFileName());
	}

	void FShaderCreationInfo::ComputeShaderTargetFromEntryPoint()
	{
		std::vector<std::string> splitStrs = FStringUtility::Split(EntryPoint, "_");

		ASSERT(splitStrs.size() == 2);

		static const std::string targetLevel{ "_6_4" };

		ShaderTarget = FStringUtility::ToLower(splitStrs[0]) + targetLevel;
	}
}