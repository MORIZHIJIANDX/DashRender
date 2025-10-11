#include "PCH.h"
#include "FileUtility.h"

namespace Dash
{
	enum class EPathType
	{
		File,
		Directory,
		Unknown,
	};

	static EPathType GetPathType(const std::string& str)
	{
		if (str.empty())
		{
			return EPathType::Unknown;
		}

		if (FFileUtility::IsPathExistent(str))
		{
			if (FFileUtility::IsFile(str))
			{
				return EPathType::File;
			}
			else if (FFileUtility::IsDirectory(str))
			{
				return EPathType::Directory;
			}
			else
			{
				return EPathType::Unknown;
			}
		}
		
		std::string path = str;
		FStringUtility::Trim(path);

		if (path.empty())
		{
			return EPathType::Unknown;
		}

		std::filesystem::path filePath{ path };
		if (filePath.extension().empty())
		{
			return EPathType::Directory;
		}

		return EPathType::File;
	}

	FFileUtility::ByteArray FFileUtility::NullFile{};

	FFileUtility::ByteArray ReadBinaryFileHelper(const std::string& fileName)
	{
		std::filesystem::path filePath{ fileName };
		if (!(std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath)))
		{
			return FFileUtility::NullFile;
		}

		std::ifstream file(fileName, std::ios::in | std::ios::binary);
		if (!file)
		{
			return FFileUtility::NullFile;
		}

		long fileSizeInByte = static_cast<long>(file.seekg(0, std::ios::end).tellg());

		FFileUtility::ByteArray byteArray = std::make_shared<std::vector<unsigned char>>(fileSizeInByte);
		file.seekg(0, std::ios::beg).read((char*)byteArray->data(), byteArray->size());
		file.close();

		ASSERT(std::filesystem::file_size(filePath) == fileSizeInByte);

		return byteArray;
	}

	FFileUtility::ByteArray FFileUtility::ReadBinaryFileSync(const std::string& fileName)
	{
		return ReadBinaryFileHelper(fileName);
	}

	std::future<FFileUtility::ByteArray> FFileUtility::ReadBinaryFileAsync(const std::string& fileName)
	{
		std::future<ByteArray> readTask = std::async(std::launch::async, ReadBinaryFileHelper, fileName);
		return readTask;
	}

	bool WriteBinaryFileHelper(const std::string& fileName, unsigned char* data, size_t count)
	{
		if (data == nullptr)
		{
			return false;
		}

		if (!std::filesystem::exists(fileName))
		{
			std::filesystem::create_directory(std::filesystem::path(fileName).parent_path());
		}

		std::ofstream file(fileName, std::ios::out | std::ios::binary);

		if (!file)
		{
			return false;
		}

		file.write((char*)data, count);
		file.close();

		return true;
	}

	bool FFileUtility::WriteBinaryFileSync(const std::string& fileName, unsigned char* data, size_t count)
	{
		return WriteBinaryFileHelper(fileName, data, count);
	}

	std::future<bool> FFileUtility::WriteBinaryFileAsync(const std::string& fileName, unsigned char* data, size_t count)
	{
		std::future<bool> writeTask = std::async(std::launch::async, WriteBinaryFileHelper, fileName, data, count);
		return writeTask;
	}

	std::string FFileUtility::GetBasePath(const std::string& str)
	{
		size_t lastSlash;
		if ((lastSlash = str.rfind('/')) != std::string::npos)
			return str.substr(0, lastSlash + 1);
		else if ((lastSlash = str.rfind('\\')) != std::string::npos)
			return str.substr(0, lastSlash + 1);
		else
			return "";
	}

	std::string FFileUtility::RemoveBasePath(const std::string& str)
	{
		size_t lastSlash;
		if ((lastSlash = str.rfind('/')) != std::string::npos)
			return str.substr(lastSlash + 1, std::string::npos);
		else if ((lastSlash = str.rfind('\\')) != std::string::npos)
			return str.substr(lastSlash + 1, std::string::npos);
		else
			return str;
	}

	std::string FFileUtility::GetFileExtension(const std::string& str)
	{
		std::string fileName = RemoveBasePath(str);
		size_t extOffset = fileName.rfind('.');
		if (extOffset == std::wstring::npos)
			return "";

		return fileName.substr(extOffset + 1);
	}

	std::string FFileUtility::RemoveExtension(const std::string& str)
	{
		return str.substr(0, str.rfind("."));
	}

	std::string FFileUtility::GetAbsolutePath(const std::string& str)
	{
		return std::filesystem::absolute(std::filesystem::path(str)).string();
	}

	std::string FFileUtility::GetRelativePath(const std::string& str, const std::string& base)
	{
		return std::filesystem::relative(str, base).string();
	}

	std::string FFileUtility::GetParentPath(const std::string& str)
	{
		return std::filesystem::path(str).parent_path().string();
	}

	std::string FFileUtility::GetRootPath(const std::string& str)
	{
		return std::filesystem::path(str).root_path().string();
	}

	std::string FFileUtility::GetFileName(const std::string& str)
	{
		return std::filesystem::path(str).filename().string();
	}

	std::string FFileUtility::CombinePath(const std::string& lhs, const std::string& rhs)
	{
		return (std::filesystem::path(lhs) / std::filesystem::path(rhs)).string();
	}

	std::string FFileUtility::GetCurrentPath()
	{
		return std::filesystem::current_path().string();
	}

	std::string FFileUtility::GetEngineDir()
	{
		static std::string engineDir;

		if (engineDir.size() == 0)
		{
			std::string appPath = GetExecutableDir();

			std::pair<std::string, std::string> pair = FStringUtility::SplitFirst(appPath, "DashProject", true);
			engineDir = FFileUtility::CombinePath(pair.first, "DashCore");
		}
		return engineDir;
	}

	std::string FFileUtility::GetProjectDir()
	{
		static std::string engineDir;

		if (engineDir.size() == 0)
		{
			std::string appPath = GetExecutableDir();

			std::pair<std::string, std::string> pair = FStringUtility::SplitFirst(appPath, "DashProject", true);
			engineDir = FFileUtility::CombinePath(pair.first, "DashProject");
		}
		return engineDir;
	}

	std::string FFileUtility::GetExecutableDir()
	{
		static std::string executableDir;

		if (executableDir.size() == 0)
		{
			std::wstring buf;
			DWORD size = 0;
			for (DWORD cap = 260; ; cap *= 2)
			{
				buf.resize(cap);
				size = GetModuleFileNameW(nullptr, &buf[0], cap);
				if (size == 0)
				{
					throw std::runtime_error("GetModuleFileNameW failed");
				}
				if (size < cap)
				{
					buf.resize(size);
					break;
				}
			}

			std::string appPath = FStringUtility::WideStringToUTF8(buf);
			executableDir = FFileUtility::GetParentPath(appPath);
		}

		return executableDir;
	}

	std::string FFileUtility::GetEngineShaderDir(const std::string& shaderFileName)
	{
		std::string temp = CombinePath("Src\\Shaders\\", shaderFileName);
		std::string shaderPath = CombinePath(GetEngineDir(), temp);
		return CombinePath(GetEngineDir(), temp);
	}

	Dash::FFileUtility::FileTimeType FFileUtility::GetFileLastWriteTime(const std::string& str)
	{
		return std::filesystem::last_write_time(std::filesystem::path(str));
	}

	bool FFileUtility::IsPathExistent(const std::string& str)
	{
		return std::filesystem::exists(std::filesystem::path(str));
	}

	bool FFileUtility::IsDirectory(const std::string& str)
	{
		return GetPathType(str) == EPathType::Directory;
	}

	bool FFileUtility::IsFile(const std::string& str)
	{
		return GetPathType(str) == EPathType::File;
	}

	bool FFileUtility::EnsurePathExist(const std::string& str)
	{
		if (str.empty())
		{
			return false;
		}

		std::string path = str;

		if (!FFileUtility::IsPathExistent(path))
		{
			if (FFileUtility::IsFile(str))
			{
				path = FFileUtility::GetParentPath(str);
			}

			std::filesystem::create_directories(path);
		}

		return true;
	}

	bool FFileUtility::EnsureFileExist(const std::string& str)
	{
		if (str.empty())
		{
			return false;
		}

		if (!FFileUtility::IsPathExistent(str))
		{
			EnsurePathExist(str);
			std::ofstream ofs(str, std::ios::app | std::ios::binary);
		}

		return true;
	}

	void FFileUtility::CreatePath(const std::string& str)
	{
		if (!FFileUtility::IsPathExistent(str))
		{
			std::filesystem::create_directories(str);
		}
	}

	void FFileUtility::DeletePath(const std::string& str)
	{
		std::filesystem::remove(std::filesystem::path(str));
	}
}