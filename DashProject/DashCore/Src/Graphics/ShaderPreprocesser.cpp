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
			std::string SourceFile;		// ��Դ�ļ������ڵ���
			uint64 LineNumber;			// ��Դ�ļ��е��к�
		};

		struct FCBufferInfo
		{
			std::string Name;
			int32 BindPoint = INDEX_NONE;			// register(b0) �е� 0
			int32 RegisterSpace = INDEX_NONE;		// register(b0, space1) �е� 1
			std::string SourceFile;			// CBuffer �������ļ�
			uint64 LineNumber;
		};

		struct FConfig
		{
			std::vector<std::string> IncludePaths;			// Include����·��, ��Ϊ�����ڴ����ļ�����Ŀ¼��������
			bool RecursiveInclude = true;					// �Ƿ�ݹ鴦��include
			bool Verbose = false;							// �Ƿ������ϸ������Ϣ
			bool GenerateComments = true;					// ����ע��
			std::string CBufferName = "BindlessCBuffer";	// ���ɵ� CBuffer ����
			int32 PreferredBindPoint = 0;					// ����ʹ�õ� BindPoint
			int32 PreferredRegisterSpace = INDEX_NONE;				// ����ʹ�õ�space��-1��ʾ�Զ�
			bool AutoDetectRegister = true;					// �Զ������õ�register
		};

		struct FProcessedResult
		{
			std::string GeneratedCode;								// ���ɵ���������
			std::string CBufferCode;								// ���ɵ� CBuffer ����
			std::vector<FBindlessResource> BindlessResources;		// Bindless ��Դ�б�
			std::vector<std::string> ProcessedFiles;				// ������ļ��б�
			std::vector<FCBufferInfo> ExistingCBuffers;				// �ҵ������� CBuffer �б�
			uint64 InsertPosition;									// ���ɵ� CBuffer ����λ��
			int32 AssignedBindPoint = 0;							// ����� BindPoint
			int32 AssignedRegisterSpace = 0;						// ����� Space
			bool Succeed = false;									// �Ƿ����� CBuffer
		};

	private:
		FConfig mConfig;
		std::string mMainFile;									// ������ļ�·��
		std::vector<std::string> mMainFileLines;				// ������ļ�����
		std::vector<FBindlessResource> mBindlessResources;
		std::vector<FCBufferInfo> mExistingCBuffers;							
		std::unordered_set<std::string> mProcessdFiles;
		uint64 mFirstBindlessLine;								// ���ļ��е�һ�� Bindless Resource ����������

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

			// ����BINDLESS����
			ParseBindlessDeclarations(filePath, lines);

			// ����cbuffer����
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

					// ����Ƿ��ظ�
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
			// ƥ�� cbuffer Name : register(bN) �� register(bN, spaceM)
			std::regex cbufferStartRegex(R"(^\s*cbuffer\s+([A-Za-z_]\w*)\s*(?:\:\s*register\s*\(\s*b(\d+)(?:\s*,\s*space\s*(\d+))?\s*\))?\s*$)");
			std::regex registerRegex(R"(\bregister\s*\(\s*b(\d+)(?:\s*,\s*space\s*(\d+))?\s*\))");

			// Ҳƥ�� ConstantBuffer<T> ����ʽ
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

			// ȷ������λ��
			result.InsertPosition = DetermineInsertPosition();

			// ���������Ĵ���
			std::ostringstream fullCode;

			// �����֮ǰ������
			for (size_t i = 0; i < result.InsertPosition && i < mMainFileLines.size(); ++i) {
				fullCode << mMainFileLines[i] << "\n";
			}

			// ����cbuffer
			if (!result.CBufferCode.empty()) {
				fullCode << result.CBufferCode;
			}

			// �����֮�������
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

			// ����1�����ָ���� register space���ڸ�space���ҿ��õ�slot
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

			// ����2��������space���ҿ��õ�slot
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

			// ����3��ʹ��û���κ�cbuffer��space
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

				// ��ʾ��⵽������cbuffer
				if (!mExistingCBuffers.empty()) {
					cbufferStream << "// Detected " << mExistingCBuffers.size() << " existing cbuffers:\n";

					// ��space������ʾ
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

				// �г����д�����ļ�
				if (mProcessdFiles.size() > 1) {
					cbufferStream << "//\n";
					cbufferStream << "// Processed files:\n";
					for (const auto& file : mProcessdFiles) {
						cbufferStream << "//   - " << FFileUtility::GetFileName(file) << "\n";
					}
				}
			}

			cbufferStream << "\n";

			// ����cbuffer����
			cbufferStream << "cbuffer " << mConfig.CBufferName << " : register(b" << bindPoint << ", space" << registerSpace << ")\n";
			cbufferStream << "{\n";

			// �����ͷ������
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
			// ����1��������ļ���BINDLESS���������뵽��һ������֮ǰ
			if (mFirstBindlessLine != INDEX_NONE) {
				return mFirstBindlessLine;
			}

			// ����2�����뵽���һ��include֮��
			uint64 lastIncludeLine = INDEX_NONE;
			std::regex includeRegex(R"(#\s*include)");

			for (uint64 i = 0; i < mMainFileLines.size(); ++i) {
				if (std::regex_search(mMainFileLines[i], includeRegex)) {
					lastIncludeLine = i;
				}
			}

			if (lastIncludeLine != INDEX_NONE) {
				// ����include��Ŀ���
				uint64 insertLine = lastIncludeLine + 1;
				while (insertLine < mMainFileLines.size() && FStringUtility::TrimCopy(mMainFileLines[insertLine]).empty()) {
					insertLine++;
				}
				return insertLine;
			}

			// ����3�����뵽�ļ���ͷ������ע�ͣ�
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