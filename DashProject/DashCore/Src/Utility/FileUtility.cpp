#include "PCH.h"
#include "FileUtility.h"

namespace Dash
{
	FileUtility::ByteArray FileUtility::NullFile{};

	FileUtility::ByteArray ReadBinaryFileHelper(const std::string& fileName)
	{
		std::filesystem::path filePath{ fileName };
		if (!(std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath)))
		{
			return FileUtility::NullFile;
		}

		std::ifstream file(fileName, std::ios::in | std::ios::binary);
		if (!file)
		{
			return FileUtility::NullFile;
		}

		long fileSizeInByte = static_cast<long>(file.seekg(0, std::ios::end).tellg());

		FileUtility::ByteArray byteArray = std::make_shared<std::vector<unsigned char>>(fileSizeInByte);
		file.seekg(0, std::ios::beg).read((char*)byteArray->data(), byteArray->size());
		file.close();

		ASSERT(std::filesystem::file_size(filePath) == fileSizeInByte);

		return byteArray;
	}

	FileUtility::ByteArray FileUtility::ReadBinaryFileSync(const std::string& fileName)
	{
		return ReadBinaryFileHelper(fileName);
	}

	std::future<FileUtility::ByteArray> FileUtility::ReadBinaryFileAsync(const std::string& fileName)
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

		std::ofstream file(fileName, std::ios::out | std::ios::binary);

		if (!file)
		{
			return false;
		}

		file.write((char*)data, count);
		file.close();

		return true;
	}

	bool FileUtility::WriteBinaryFileSync(const std::string& fileName, unsigned char* data, size_t count)
	{
		return WriteBinaryFileHelper(fileName, data, count);
	}

	std::future<bool> FileUtility::WriteBinaryFileAsync(const std::string& fileName, unsigned char* data, size_t count)
	{
		std::future<bool> writeTask = std::async(std::launch::async, WriteBinaryFileHelper, fileName, data, count);
		return writeTask;
	}

	std::string FileUtility::GetBasePath(const std::string& str)
	{
		size_t lastSlash;
		if ((lastSlash = str.rfind('/')) != std::string::npos)
			return str.substr(0, lastSlash + 1);
		else if ((lastSlash = str.rfind('\\')) != std::string::npos)
			return str.substr(0, lastSlash + 1);
		else
			return "";
	}

	std::string FileUtility::RemoveBasePath(const std::string& str)
	{
		size_t lastSlash;
		if ((lastSlash = str.rfind('/')) != std::string::npos)
			return str.substr(lastSlash + 1, std::string::npos);
		else if ((lastSlash = str.rfind('\\')) != std::string::npos)
			return str.substr(lastSlash + 1, std::string::npos);
		else
			return str;
	}

	std::string FileUtility::GetFileExtension(const std::string& str)
	{
		std::string fileName = RemoveBasePath(str);
		size_t extOffset = fileName.rfind('.');
		if (extOffset == std::wstring::npos)
			return "";

		return fileName.substr(extOffset + 1);
	}

	std::string FileUtility::RemoveExtension(const std::string& str)
	{
		return str.substr(0, str.rfind("."));
	}

	std::string FileUtility::GetAbsolutePath(const std::string& str)
	{
		return std::filesystem::absolute(std::filesystem::path(str)).string();
	}

	std::string FileUtility::GetRelativePath(const std::string& str, const std::string& base)
	{
		return std::filesystem::relative(str, base).string();
	}

	std::string FileUtility::GetParentPath(const std::string& str)
	{
		return std::filesystem::path(str).parent_path().string();
	}

	std::string FileUtility::GetRootPath(const std::string& str)
	{
		return std::filesystem::path(str).root_path().string();
	}

	std::string FileUtility::GetFileName(const std::string& str)
	{
		return std::filesystem::path(str).filename().string();
	}

	std::string FileUtility::GetCurrentPath()
	{
		return std::filesystem::current_path().string();
	}

	Dash::FileUtility::FileTimeType FileUtility::GetFileLastWriteTime(const std::string& str)
	{
		return std::filesystem::last_write_time(std::filesystem::path(str));
	}

	bool FileUtility::IsPathExistent(const std::string& str)
	{
		return std::filesystem::exists(std::filesystem::path(str));
	}

	bool FileUtility::IsPath(const std::string& str)
	{
		return std::filesystem::is_directory(std::filesystem::path(str));
	}

	bool FileUtility::IsFile(const std::string& str)
	{
		return std::filesystem::is_regular_file(std::filesystem::path(str));
	}

	void FileUtility::DeletePath(const std::string& str)
	{
		std::filesystem::remove(std::filesystem::path(str));
	}
}