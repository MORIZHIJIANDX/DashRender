#pragma once

namespace Dash
{
	class FileUtility
	{
	public:
		using ByteArray = std::shared_ptr<std::vector<unsigned char>>;
		static ByteArray NullFile;

		using FileTimeType = std::filesystem::file_time_type;

		static ByteArray ReadBinaryFileSync(const std::string& fileName);
		static std::future<ByteArray> ReadBinaryFileAsync(const std::string& fileName);

		static bool WriteBinaryFileSync(const std::string& fileName, unsigned char* data, size_t count);
		static std::future<bool> WriteBinaryFileAsync(const std::string& fileName, unsigned char* data, size_t count);

		static std::string GetBasePath(const std::string& str);

		static std::string RemoveBasePath(const std::string& str);

		static std::string GetFileExtension(const std::string& str);

		static std::string RemoveExtension(const std::string& str);

		static std::string GetAbsolutePath(const std::string& str);

		static std::string GetRelativePath(const std::string& str, const std::string& base = std::filesystem::current_path().string());

		static std::string GetParentPath(const std::string& str);

		static std::string GetRootPath(const std::string& str);

		static std::string GetFileName(const std::string& str);

		static std::string GetCurrentPath();

		static FileTimeType GetFileLastWriteTime(const std::string& str);

		static bool IsPathExistent(const std::string& str);
		static bool IsPath(const std::string& str);
		static bool IsFile(const std::string& str);

		static void DeletePath(const std::string& str);

	};
}

