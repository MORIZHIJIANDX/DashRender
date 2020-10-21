#pragma once

namespace Dash
{
	class FileUtility
	{
	public:
		using ByteArray = std::shared_ptr<std::vector<unsigned char>>;
		static ByteArray NullFile;

		static ByteArray ReadBinaryFileSync(const std::wstring& fileName);
		static std::future<ByteArray> ReadBinaryFileAsync(const std::wstring& fileName);

		static bool WriteBinaryFileSync(const std::wstring& fileName, unsigned char* data, size_t count);
		static std::future<bool> WriteBinaryFileAsync(const std::wstring& fileName, unsigned char* data, size_t count);
	};
}

