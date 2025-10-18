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
			std::string SourceFile;		// ��Դ�ļ������ڵ���
			uint64 LineNumber;			// ��Դ�ļ��е��к�
		};

		struct FCBufferInfo
		{
			std::string Name;
			int32 Slot;					// register(b0) �е� 0
			int32 RegisterSpace;		// register(b0, space1) �е� 1
			std::string SourceFile;		// CBuffer �������ļ�
			uint64 LineNumber;
		};

		struct FConfig
		{
			std::vector<std::string> IncludePaths;			// Include����·��, ��Ϊ�����ڴ����ļ�����Ŀ¼��������
			bool RecursiveInclude = true;					// �Ƿ�ݹ鴦��include
			bool Verbose = false;							// �Ƿ������ϸ������Ϣ
			bool GenerateComments = true;					// ����ע��
			std::string CBufferName = "BindlessCBuffer";	// ���ɵ� CBuffer ����
			int32 PreferredSlot = 0;						// ����ʹ�õ�slot
			int32 PreferredSpace = -1;						// ����ʹ�õ�space��-1��ʾ�Զ�
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
			int32 AssignedSlot = 0;									// ����� Slot
			int32 AssignedSpace = 0;								// ����� Space
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

					// ����Ƿ��ظ�
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
			// ƥ�� cbuffer Name : register(bN) �� register(bN, spaceM)
			std::regex cbufferStartRegex(R"(cbuffer\s+(\w+)\s*:\s*register\s*\()");
			std::regex registerRegex(R"(register\s*\(\s*b(\d+)(?:\s*,\s*space(\d+))?\s*\))");

			// Ҳƥ�� ConstantBuffer<T> ����ʽ
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