#include "PCH.h"
#include "ShaderPreprocesser.h"
#include "Utility/FileUtility.h"

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
			int32 BindPoint = INDEX_NONE;			// register(b0) 中的 0
			int32 RegisterSpace = INDEX_NONE;		// register(b0, space1) 中的 1
			std::string SourceFile;			// CBuffer 声明的文件
			uint64 LineNumber;
		};

		struct FConfig
		{
			std::vector<std::string> IncludePaths;			// Include搜索路径, 若为空则在处理文件所处目录进行搜索
			bool RecursiveInclude = true;					// 是否递归处理include
			bool Verbose = false;							// 是否输出详细处理信息
			bool GenerateComments = true;					// 生成注释
			std::string CBufferName = "BindlessCBuffer";	// 生成的 CBuffer 名称
			int32 PreferredBindPoint = 0;					// 优先使用的 BindPoint
			int32 PreferredRegisterSpace = INDEX_NONE;				// 优先使用的space，-1表示自动
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
			int32 AssignedBindPoint = 0;							// 分配的 BindPoint
			int32 AssignedRegisterSpace = 0;						// 分配的 Space
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

			FProcessedResult result = GenerateResult();

			if (!result.CBufferCode.empty())
			{
				result.Succeed = true;
			}

			return result;
		}

	private:
		void Reset()
		{
			mMainFileLines.clear();
			mBindlessResources.clear();
			mExistingCBuffers.clear();
			mProcessdFiles.clear();
			mFirstBindlessLine = INDEX_NONE;
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

			// 解析BINDLESS声明
			ParseBindlessDeclarations(filePath, lines);

			// 解析cbuffer声明
			ParseCBufferDeclarations(filePath, lines);

			if (mConfig.RecursiveInclude)
			{
				for (const std::string& includeFile : includes)
				{
					std::string includeFilePath = ResolveIncludePath(includeFile, FFileUtility::GetParentPath(filePath));
					if (!includeFilePath.empty())
					{
						ProcessFileRecursive(includeFilePath);
					}
				}
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
		}

		void ParseBindlessDeclarations(const std::string& filePath, std::vector<std::string>& lines)
		{
			std::regex bindlessRegex(R"(\b(?:BINDLESS_)?(SRV|SAMPLER|UAV)\s*\(\s*((?:[^(),<]+|<(?:[^<>]|<[^<>]*>)*>)+)\s*,\s*([A-Za-z_]\w*)\s*\)\s*;?)");

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
						DASH_LOG(LogTemp, Fatal, "[BindlessCBufferGenerator] Found duplicate resource {} in file {} : {}, first find in {} : {}",
							resource.ResourceName, resource.SourceFile, resource.LineNumber, iter->SourceFile, iter->LineNumber);
					}
					else
					{
						mBindlessResources.push_back(resource);

						if (filePath == mMainFile && mFirstBindlessLine == INDEX_NONE)
						{
							mFirstBindlessLine = index;
						}
					}
				}
			}
		}

		void ParseCBufferDeclarations(const std::string& filePath, std::vector<std::string>& lines)
		{
			// 匹配 cbuffer Name : register(bN) 或 register(bN, spaceM)
			std::regex cbufferStartRegex(R"(^\s*cbuffer\s+([A-Za-z_]\w*)\s*(?:\:\s*register\s*\(\s*b(\d+)(?:\s*,\s*space\s*(\d+))?\s*\))?\s*$)");
			std::regex registerRegex(R"(\bregister\s*\(\s*b(\d+)(?:\s*,\s*space\s*(\d+))?\s*\))");

			// 也匹配 ConstantBuffer<T> 的形式
			std::regex constantBufferRegex(R"(ConstantBuffer\s*<[^>]+>\s+(\w+)\s*:\s*register\s*\(\s*b(\d+)(?:\s*,\s*space(\d+))?\s*\))");

			for (uint64 index = 0; index < lines.size(); index++)
			{
				const std::string line = lines[index];

				std::smatch cbufferMatch;
				std::smatch constantBufferMatch;

				bool cbufferMatchSucceed = std::regex_match(line, cbufferMatch, cbufferStartRegex);
				bool constantBufferMatchSucceed = std::regex_match(line, constantBufferMatch, constantBufferRegex);
				if (cbufferMatchSucceed || constantBufferMatchSucceed)
				{
					FCBufferInfo cbufferInfo;
					cbufferInfo.Name = cbufferMatchSucceed ? cbufferMatch[1].str() : constantBufferMatch[1].str();
					cbufferInfo.SourceFile = filePath;
					cbufferInfo.LineNumber = index;

					std::smatch registerMatch;
					if (std::regex_search(line, registerMatch, registerRegex))
					{
						cbufferInfo.BindPoint = std::stoi(registerMatch[1].str());
						if (registerMatch[2].matched)
						{
							cbufferInfo.RegisterSpace = std::stoi(registerMatch[2].str());
						}
					}
					else
					{
						DASH_LOG(LogTemp, Fatal, "[BindlessCBufferGenerator] CBuffer {} BindPoint Not Defined", cbufferInfo.Name);
					}

					if (cbufferInfo.Name != mConfig.CBufferName)
					{
						mExistingCBuffers.push_back(cbufferInfo);

						DASH_LOG(LogTemp, Info, "[BindlessCBufferGenerator] Found CBuffer {} BindPoint {} Register Space {}, File {}", cbufferInfo.Name, cbufferInfo.BindPoint, cbufferInfo.RegisterSpace, cbufferInfo.SourceFile);
					}
				}
			}
		}

		std::string ResolveIncludePath(const std::string& includeFile, std::string currentDir)
		{
			std::vector<std::string> searchPaths;
			searchPaths.push_back(FFileUtility::CombinePath(currentDir, includeFile));

			for (const std::string& includePath : mConfig.IncludePaths)
			{
				searchPaths.push_back(FFileUtility::CombinePath(includePath, includeFile));
			}

			searchPaths.push_back(includeFile);

			for (const std::string& path : searchPaths)
			{
				if (FFileUtility::IsPathExistent(path))
				{
					return path;
				}
			}

			DASH_LOG(LogTemp, Error, "[BindlessCBufferGenerator] Not Found Include File {}", includeFile);

			return "";
		}

		FProcessedResult GenerateResult()
		{
			FProcessedResult result;

			result.BindlessResources = mBindlessResources;
			result.ExistingCBuffers = mExistingCBuffers;
			result.ProcessedFiles.reserve(mProcessdFiles.size());

			for (const std::string& file : mProcessdFiles)
			{
				result.ProcessedFiles.push_back(file);
			}

			auto [bindPoint, resigerSpace] = DetermineRegisterAllocation();

			ASSERT(bindPoint != INDEX_NONE);

			result.AssignedBindPoint = bindPoint;
			result.AssignedRegisterSpace = resigerSpace;

			if (!mBindlessResources.empty())
			{
				std::ostringstream cbufferStream;
				GenerateBindlessCBuffer(cbufferStream, bindPoint, resigerSpace);
				result.CBufferCode = cbufferStream.str();
			}

			// 确定插入位置
			result.InsertPosition = DetermineInsertPosition();

			// 生成完整的代码
			std::ostringstream fullCode;

			// 插入点之前的内容
			for (size_t i = 0; i < result.InsertPosition && i < mMainFileLines.size(); ++i) {
				fullCode << mMainFileLines[i] << "\n";
			}

			// 插入cbuffer
			if (!result.CBufferCode.empty()) {
				fullCode << result.CBufferCode;
			}

			// 插入点之后的内容
			for (size_t i = result.InsertPosition; i < mMainFileLines.size(); ++i) {
				fullCode << mMainFileLines[i] << "\n";
			}

			result.GeneratedCode = fullCode.str();

			return result;
		}

		std::pair<int32, int32> DetermineRegisterAllocation()
		{
			const int32 MaxBindPoint = 16;		// D3D12_COMMONSHADER_CONSTANT_BUFFER_HW_SLOT_COUNT
			const int32 MaxRegisterSpace = 16;	// D3D12_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT

			int32 bindPoint = FMath::Min(mConfig.PreferredBindPoint, MaxBindPoint);
			int32 resigerSpace = FMath::Min(mConfig.PreferredRegisterSpace, MaxRegisterSpace);

			std::map<int32, std::set<int32>> usedBindPointPerSpace;

			std::vector<uint32> spaceUndeclaredCBufferIndexs;
			for (uint32 i = 0; i < mExistingCBuffers.size(); i++)
			{
				if (mExistingCBuffers[i].RegisterSpace == INDEX_NONE)
				{
					spaceUndeclaredCBufferIndexs.push_back(i);
				}
				else
				{
					usedBindPointPerSpace[mExistingCBuffers[i].RegisterSpace].emplace(mExistingCBuffers[i].BindPoint);
				}
			}

			if (!spaceUndeclaredCBufferIndexs.empty())
			{
				while (!spaceUndeclaredCBufferIndexs.empty())
				{
					uint32 cbufferIndex = spaceUndeclaredCBufferIndexs.back();
					FCBufferInfo& cbuffer = mExistingCBuffers[cbufferIndex];
					
					for (int32 space = 0; space < MaxRegisterSpace; space++)
					{
						if (usedBindPointPerSpace[space].size() < MaxBindPoint)
						{
							cbuffer.RegisterSpace = space;
							usedBindPointPerSpace[space].emplace(cbuffer.BindPoint);
							break;
						}
					}

					spaceUndeclaredCBufferIndexs.pop_back();
				}
			}

			// 策略1：如果指定了 register space，在该space中找可用的slot
			if (resigerSpace != INDEX_NONE)
			{
				if (usedBindPointPerSpace.contains(resigerSpace))
				{
					const std::set<int32>& bindPoints = usedBindPointPerSpace[resigerSpace];
					if (bindPoints.size() < MaxBindPoint)
					{
						if (bindPoint != INDEX_NONE &&
							bindPoint >= 0 &&
							bindPoint < MaxBindPoint)
						{
							if (bindPoints.find(bindPoint) == bindPoints.end())
							{
								return { bindPoint , resigerSpace };
							}
						}

						for (int32 testBindPoint = 0; testBindPoint < MaxBindPoint; testBindPoint++)
						{
							if (!bindPoints.contains(testBindPoint))
							{
								DASH_LOG(LogTemp, Info, "[BindlessCBufferGenerator] Auto detect bind point {} register space {}", testBindPoint, resigerSpace);
								return { testBindPoint , resigerSpace };
							}
						}
					}
				}
			}

			// 策略2：在现有space中找可用的slot
			for (const auto& registerSpaceMap : usedBindPointPerSpace)
			{
				int32 testRegisterSpace = registerSpaceMap.first;
				const std::set<int32>& bindPoints = registerSpaceMap.second;
				if (bindPoints.size() < MaxBindPoint)
				{
					for (int32 testBindPoint = 0; testBindPoint < MaxBindPoint; testBindPoint++)
					{
						if (!bindPoints.contains(testBindPoint))
						{
							DASH_LOG(LogTemp, Info, "[BindlessCBufferGenerator] Auto detect bind point {} register space {}", testBindPoint, testRegisterSpace);
							return { testBindPoint , testRegisterSpace };
						}
					}
				}
			}

			// 策略3：使用没有任何cbuffer的space
			{
				for (int32 testRegisterSpace = 0; testRegisterSpace < MaxBindPoint; testRegisterSpace++)
				{
					if (!usedBindPointPerSpace.contains(testRegisterSpace))
					{
						DASH_LOG(LogTemp, Info, "[BindlessCBufferGenerator] Auto detect bind point {} register space {}", bindPoint, testRegisterSpace);
						return { bindPoint , testRegisterSpace };
					}
				}
			}

			return { INDEX_NONE, INDEX_NONE };
		}

		void GenerateBindlessCBuffer(std::ostringstream& cbufferStream, int32 bindPoint, int32 registerSpace)
		{
			cbufferStream << "// ===== Auto-generated Bindless CBuffer =====\n";

			if (mConfig.GenerateComments) {
				cbufferStream << "// Generated from BINDLESS_* macro declarations\n";
				cbufferStream << "// Found " << mBindlessResources.size() << " resources across "
					<< mProcessdFiles.size() << " files\n";

				// 显示检测到的其他cbuffer
				if (!mExistingCBuffers.empty()) {
					cbufferStream << "// Detected " << mExistingCBuffers.size() << " existing cbuffers:\n";

					// 按space分组显示
					std::map<int, std::vector<const FCBufferInfo*>> cbuffersBySpace;
					for (const auto& cb : mExistingCBuffers) {
						cbuffersBySpace[cb.RegisterSpace].push_back(&cb);
					}

					for (const auto& [cbSpace, cbs] : cbuffersBySpace) {
						cbufferStream << "//   space" << cbSpace << ": ";
						bool first = true;
						for (const auto* cb : cbs) {
							if (!first) cbufferStream << ", ";
							cbufferStream << cb->Name << "(b" << cb->BindPoint << ", " << "space" << cb->RegisterSpace << ")";
							first = false;
						}
						cbufferStream << "\n";
					}
				}

				cbufferStream << "// Assigned register: b" << bindPoint << ", space" << registerSpace << "\n";
				cbufferStream << "// Last generated: " << std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::system_clock::now()) << "\n";

				// 列出所有处理的文件
				if (mProcessdFiles.size() > 1) {
					cbufferStream << "//\n";
					cbufferStream << "// Processed files:\n";
					for (const auto& file : mProcessdFiles) {
						cbufferStream << "//   - " << FFileUtility::GetFileName(file) << "\n";
					}
				}
			}

			cbufferStream << "\n";

			// 生成cbuffer声明
			cbufferStream << "cbuffer " << mConfig.CBufferName << " : register(b" << bindPoint << ", space" << registerSpace << ")\n";
			cbufferStream << "{\n";

			// 按类型分组输出
			OutputResourcesByType(cbufferStream, "SRV", "SRV Resources");
			OutputResourcesByType(cbufferStream, "UAV", "UAV Resources");
			OutputResourcesByType(cbufferStream, "SAMPLER", "Sampler Resources");

			cbufferStream << "};\n";
			cbufferStream << "\n";
			cbufferStream << "// ===== End of Auto-generated Section =====\n";
			cbufferStream << "\n";
		}

		void OutputResourcesByType(std::ostream& out, const std::string& type, const std::string& comment) {
			std::vector<const FBindlessResource*> resources;
			for (const auto& res : mBindlessResources) {
				if (res.ResourceType == type) {
					resources.push_back(&res);
				}
			}

			if (!resources.empty()) {
				if (mConfig.GenerateComments) {
					out << "    // " << comment << "\n";
				}

				for (const auto* res : resources) {
					out << "    uint Bindless" << res->ResourceType << "_" << res->ResourceName << ";";

					if (mConfig.GenerateComments && mProcessdFiles.size() > 1) {
						out << " // " << res->DataType << " from " << res->SourceFile;
					}

					out << "\n";
				}
			}
		}

		size_t DetermineInsertPosition() {
			// 策略1：如果主文件有BINDLESS声明，插入到第一个声明之前
			if (mFirstBindlessLine != INDEX_NONE) {
				return mFirstBindlessLine;
			}

			// 策略2：插入到最后一个include之后
			uint64 lastIncludeLine = INDEX_NONE;
			std::regex includeRegex(R"(#\s*include)");

			for (uint64 i = 0; i < mMainFileLines.size(); ++i) {
				if (std::regex_search(mMainFileLines[i], includeRegex)) {
					lastIncludeLine = i;
				}
			}

			if (lastIncludeLine != INDEX_NONE) {
				// 跳过include后的空行
				uint64 insertLine = lastIncludeLine + 1;
				while (insertLine < mMainFileLines.size() && FStringUtility::TrimCopy(mMainFileLines[insertLine]).empty()) {
					insertLine++;
				}
				return insertLine;
			}

			// 策略3：插入到文件开头（跳过注释）
			for (size_t i = 0; i < mMainFileLines.size(); ++i) {
				const std::string& trimmed = FStringUtility::TrimCopy(mMainFileLines[i]);
				if (!trimmed.empty() && !trimmed.starts_with("//")) {
					return i;
				}
			}

			return 0;
		}
	};

	std::string FShaderPreprocesser::Process(const std::string& filePath)
	{
        std::ifstream file(filePath);
        if (!file.is_open()) {
            DASH_LOG(LogTemp, Fatal, "Failed to read file {}", filePath);
        }

		FBindlessCBufferGenerator::FConfig bindlessGeneratorConfig;
		FBindlessCBufferGenerator bindlessCBufferGenerator;
		bindlessCBufferGenerator.SetConfig(bindlessGeneratorConfig);
		FBindlessCBufferGenerator::FProcessedResult result = bindlessCBufferGenerator.Process(filePath);

		return result.GeneratedCode;
	}
}