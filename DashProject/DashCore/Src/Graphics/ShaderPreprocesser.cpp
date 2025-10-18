#include "PCH.h"
#include "ShaderPreprocesser.h"

namespace Dash
{
	class FBindlessCBufferGenerator
	{
	public:
		struct FBindlessResource
		{
			std::string ResourceType;	// SRV, UAV, SAMPLER
			std::string DataType;		// Texture2D, RWTexture2D<float>, etc.
			std::string ResourceName;	// DepthTexture, etc.
			std::string SourceFile;		// 来源文件，用于调试
			uint64 LineNumber;			// 在源文件中的行号
		};

		struct FCBufferInfo
		{
			std::string Name;
			int32 Slot;					// register(b0) 中的 0
			int32 RegisterSpace;		// register(b0, space1) 中的 1
			std::string SourceFile;		// CBuffer 声明的文件
			uint64 LineNumber;
		};

		struct FConfig
		{
			std::vector<std::string> IncludePaths;			// Include搜索路径, 若为空则在处理文件所处目录进行搜索
			bool RecursiveInclude = true;					// 是否递归处理include
			bool Verbose = false;							// 是否输出详细处理信息
			bool GenerateComments = true;					// 生成注释
			std::string CBufferName = "BindlessCBuffer";	// 生成的 CBuffer 名称
			int32 PreferredSlot = 0;						// 优先使用的slot
			int32 PreferredSpace = -1;						// 优先使用的space，-1表示自动
			bool AutoDetectRegister = true;					// 自动检测可用的register
		};

		struct FProcessedResult
		{
			std::string GeneratedCode;								// 生成的完整代码
			std::string CBufferCode;								// 生成的 CBuffer 代码
			std::vector<FBindlessResource> BindlessResources;		// Bindless 资源列表
			std::vector<std::string> ProcessedFiles;				// 处理的文件列表
			std::vector<FCBufferInfo> ExistingCBuffers;				// 找到的现有 CBuffer 列表
			uint64 InsertPosition;									// 生成的 CBuffer 插入位置
			int32 AssignedSlot = 0;									// 分配的 Slot
			int32 AssignedSpace = 0;								// 分配的 Space
			bool Succeed = false;									// 是否生成 CBuffer
		};

	private:
		FConfig mConfig;
		std::string mMainFile;									// 处理的文件路径
		std::vector<std::string> mMainFileLines;				// 处理的文件内容
		std::vector<FBindlessResource> mBindlessResources;
		std::vector<FCBufferInfo> mExistingCBuffers;							
		std::unordered_set<std::string> mProcessdFiles;
		uint64 mFirstBindlessLine;								// 主文件中第一个 Bindless Resource 声明的行数

	public:
		void SetConfig(const FConfig& config)
		{
			mConfig = config;
		}

		FProcessedResult Process(std::string filePath)
		{
			Reset();

			mMainFile = filePath;

			ProcessFileRecursive(mMainFile);
		}

	private:
		void Reset()
		{
			mMainFileLines.clear();
			mBindlessResources.clear();
			mExistingCBuffers.clear();
			mProcessdFiles.clear();
			mFirstBindlessLine = TScalarTraits<uint64>::Max();
		}

		void ProcessFileRecursive(const std::string& filePath)
		{
			if (mProcessdFiles.contains(filePath))
			{
				return;
			}

			mProcessdFiles.insert(filePath);

			if (mConfig.Verbose)
			{
				DASH_LOG(LogTemp, Info, "[BindlessCBufferGenerator] Parsing {}", filePath);
			}

			std::vector<std::string> lines;
			std::vector<std::string> includes;

			ReadFile(filePath, lines, includes);

			if (filePath == mMainFile)
			{
				mMainFileLines = lines;
			}
		}

		void ReadFile(const std::string& filePath, std::vector<std::string>& lines, std::vector<std::string>& includes)
		{
			std::ifstream file(filePath);
			if (!file.is_open())
			{
				DASH_LOG(LogTemp, Fatal, "[BindlessCBufferGenerator] Failed to read file {}", filePath);
			}

			std::string line;

			std::regex includeRegex(R"(#\s*include\s*[<"]([^>"]+)[>"])");

			while (std::getline(file, line))
			{
				lines.push_back(line);

				std::smatch match;
				if (std::regex_match(line, match, includeRegex))
				{
					includes.push_back(match[1].str());
				}
			}

			ParseBindlessDeclarations(filePath, lines);
		}

		void ParseBindlessDeclarations(const std::string& filePath, std::vector<std::string>& lines)
		{
			std::regex bindlessRegex(R"(BINDLESS_(SRV|UAV|SAMPLER)\s*\(\s*([^,)]+)\s*,\s*(\w+)\s*\))");

			for (uint64 index = 0; index < lines.size(); index++)
			{
				const std::string line = lines[index];
				std::smatch match;

				if (std::regex_match(line, match, bindlessRegex))
				{
					FBindlessResource resource;
					resource.ResourceType = match[1].str();
					resource.DataType = FStringUtility::TrimCopy(match[2].str());
					resource.ResourceName = match[3].str();
					resource.SourceFile = filePath;
					resource.LineNumber = index + 1;

					// 检查是否重复
					auto iter = std::find_if(mBindlessResources.begin(), mBindlessResources.end(), [&resource](const FBindlessResource& r){
						return r.ResourceName == resource.ResourceName;
					});

					if (iter != mBindlessResources.end())
					{
						DASH_LOG(LogTemp, Error, "[BindlessCBufferGenerator] Found duplicate resource {} in file {} : {}, first find in {} : {}", 
							resource.ResourceName, resource.SourceFile, resource.LineNumber, iter->SourceFile, iter->LineNumber);
					}
					else
					{
						mBindlessResources.push_back(resource);

						if (filePath == mMainFile && mFirstBindlessLine == TScalarTraits<uint64>::Max())
						{
							mFirstBindlessLine = index;
						}
					}
				}
			}
		}

		void ParseCBufferDeclarations()
		{
			// 匹配 cbuffer Name : register(bN) 或 register(bN, spaceM)
			std::regex cbufferStartRegex(R"(cbuffer\s+(\w+)\s*:\s*register\s*\()");
			std::regex registerRegex(R"(register\s*\(\s*b(\d+)(?:\s*,\s*space(\d+))?\s*\))");

			// 也匹配 ConstantBuffer<T> 的形式
			std::regex constantBufferRegex(R"(ConstantBuffer\s*<[^>]+>\s+(\w+)\s*:\s*register\s*\(\s*b(\d+)(?:\s*,\s*space(\d+))?\s*\))");
		}
	};

	std::string FShaderPreprocesser::Process(const std::string& filePath)
	{
        std::ifstream file(filePath);
        if (!file.is_open()) {
            DASH_LOG(LogTemp, Fatal, "Failed to read file {}", filePath);
        }


	}
}