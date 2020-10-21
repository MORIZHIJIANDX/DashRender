#include "PCH.h"
#include "FileUtility.h"

namespace Dash
{
	FileUtility::ByteArray ReadBinaryFileHelper(const std::wstring& fileName)
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

	FileUtility::ByteArray FileUtility::ReadBinaryFileSync(const std::wstring& fileName)
	{
		return ReadBinaryFileHelper(fileName);
	}

	std::future<FileUtility::ByteArray> FileUtility::ReadBinaryFileAsync(const std::wstring& fileName)
	{
		std::future<ByteArray> readTask = std::async(std::launch::async, ReadBinaryFileHelper, fileName);
		return readTask;
	}


	bool WriteBinaryFileHelper(const std::wstring& fileName, unsigned char* data, size_t count)
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

	bool FileUtility::WriteBinaryFileSync(const std::wstring& fileName, unsigned char* data, size_t count)
	{
		return WriteBinaryFileHelper(fileName, data, count);
	}

	std::future<bool> FileUtility::WriteBinaryFileAsync(const std::wstring& fileName, unsigned char* data, size_t count)
	{
		std::future<bool> writeTask = std::async(std::launch::async, WriteBinaryFileHelper, fileName, data, count);
		return writeTask;
	}
}