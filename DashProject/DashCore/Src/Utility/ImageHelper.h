#pragma once

#include "Image.h"
#include "DDS.h"
#include <wincodec.h>
#include <wrl.h>

namespace Dash
{
	enum class EWIC_LOADER_FLAGS : uint32_t
	{
		WIC_LOADER_DEFAULT = 0,
		WIC_LOADER_FORCE_SRGB = 0x1,
		WIC_LOADER_IGNORE_SRGB = 0x2,
		WIC_LOADER_SRGB_DEFAULT = 0x4,
		WIC_LOADER_MIP_AUTOGEN = 0x8,
		WIC_LOADER_MIP_RESERVE = 0x10,
		WIC_LOADER_FIT_POW2 = 0x20,
		WIC_LOADER_MAKE_SQUARE = 0x40,
		WIC_LOADER_FORCE_RGBA32 = 0x80,
	};

	enum EDDS_LOADER_FLAGS : uint32_t
	{
		DDS_LOADER_DEFAULT = 0,
		DDS_LOADER_FORCE_SRGB = 0x1,
		DDS_LOADER_MIP_AUTOGEN = 0x8,
		DDS_LOADER_MIP_RESERVE = 0x10,
	};

	FORCEINLINE EWIC_LOADER_FLAGS operator&(const EWIC_LOADER_FLAGS& a, const EWIC_LOADER_FLAGS& b)
	{
		return static_cast<EWIC_LOADER_FLAGS>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
	}

	FORCEINLINE EWIC_LOADER_FLAGS operator|(const EWIC_LOADER_FLAGS& a, const EWIC_LOADER_FLAGS& b)
	{
		return static_cast<EWIC_LOADER_FLAGS>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	FORCEINLINE bool Valid(const EWIC_LOADER_FLAGS& a)
	{
		return a != EWIC_LOADER_FLAGS::WIC_LOADER_DEFAULT;
	}



	FORCEINLINE static void GetImageColor(FColor& color, const FVector2i& index, const FTexture* image, bool repeat = false);

	FORCEINLINE EDASH_FORMAT DXGIFormatToDashFormat(DXGI_FORMAT format);

	FORCEINLINE DXGI_FORMAT DashFormatToDXGIFormat(EDASH_FORMAT format);

	FORCEINLINE void SavePPMImage(const FTexture* image, const std::string& name);

	FORCEINLINE FTexture LoadPPMImage(const std::string& name);

	/*
	//--------------------------------------------------------------------------------------
	// Return the BPP for a particular format
	//--------------------------------------------------------------------------------------
	FORCEINLINE size_t BitsPerPixel(DXGI_FORMAT fmt) noexcept
	{
		switch (fmt)
		{
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
		case DXGI_FORMAT_R32G32B32A32_UINT:
		case DXGI_FORMAT_R32G32B32A32_SINT:
			return 128;

		case DXGI_FORMAT_R32G32B32_TYPELESS:
		case DXGI_FORMAT_R32G32B32_FLOAT:
		case DXGI_FORMAT_R32G32B32_UINT:
		case DXGI_FORMAT_R32G32B32_SINT:
			return 96;

		case DXGI_FORMAT_R16G16B16A16_TYPELESS:
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
		case DXGI_FORMAT_R16G16B16A16_UNORM:
		case DXGI_FORMAT_R16G16B16A16_UINT:
		case DXGI_FORMAT_R16G16B16A16_SNORM:
		case DXGI_FORMAT_R16G16B16A16_SINT:
		case DXGI_FORMAT_R32G32_TYPELESS:
		case DXGI_FORMAT_R32G32_FLOAT:
		case DXGI_FORMAT_R32G32_UINT:
		case DXGI_FORMAT_R32G32_SINT:
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		case DXGI_FORMAT_Y416:
		case DXGI_FORMAT_Y210:
		case DXGI_FORMAT_Y216:
			return 64;

		case DXGI_FORMAT_R10G10B10A2_TYPELESS:
		case DXGI_FORMAT_R10G10B10A2_UNORM:
		case DXGI_FORMAT_R10G10B10A2_UINT:
		case DXGI_FORMAT_R11G11B10_FLOAT:
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_R8G8B8A8_UINT:
		case DXGI_FORMAT_R8G8B8A8_SNORM:
		case DXGI_FORMAT_R8G8B8A8_SINT:
		case DXGI_FORMAT_R16G16_TYPELESS:
		case DXGI_FORMAT_R16G16_FLOAT:
		case DXGI_FORMAT_R16G16_UNORM:
		case DXGI_FORMAT_R16G16_UINT:
		case DXGI_FORMAT_R16G16_SNORM:
		case DXGI_FORMAT_R16G16_SINT:
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
		case DXGI_FORMAT_R32_UINT:
		case DXGI_FORMAT_R32_SINT:
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		case DXGI_FORMAT_R8G8_B8G8_UNORM:
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		case DXGI_FORMAT_AYUV:
		case DXGI_FORMAT_Y410:
		case DXGI_FORMAT_YUY2:
#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
		case DXGI_FORMAT_R10G10B10_7E3_A2_FLOAT:
		case DXGI_FORMAT_R10G10B10_6E4_A2_FLOAT:
		case DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM:
#endif
			return 32;

		case DXGI_FORMAT_P010:
		case DXGI_FORMAT_P016:
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
		case DXGI_FORMAT_V408:
#endif
#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
		case DXGI_FORMAT_D16_UNORM_S8_UINT:
		case DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X16_TYPELESS_G8_UINT:
#endif
			return 24;

		case DXGI_FORMAT_R8G8_TYPELESS:
		case DXGI_FORMAT_R8G8_UNORM:
		case DXGI_FORMAT_R8G8_UINT:
		case DXGI_FORMAT_R8G8_SNORM:
		case DXGI_FORMAT_R8G8_SINT:
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_R16_FLOAT:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM:
		case DXGI_FORMAT_R16_UINT:
		case DXGI_FORMAT_R16_SNORM:
		case DXGI_FORMAT_R16_SINT:
		case DXGI_FORMAT_B5G6R5_UNORM:
		case DXGI_FORMAT_B5G5R5A1_UNORM:
		case DXGI_FORMAT_A8P8:
		case DXGI_FORMAT_B4G4R4A4_UNORM:
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
		case DXGI_FORMAT_P208:
		case DXGI_FORMAT_V208:
#endif
			return 16;

		case DXGI_FORMAT_NV12:
		case DXGI_FORMAT_420_OPAQUE:
		case DXGI_FORMAT_NV11:
			return 12;

		case DXGI_FORMAT_R8_TYPELESS:
		case DXGI_FORMAT_R8_UNORM:
		case DXGI_FORMAT_R8_UINT:
		case DXGI_FORMAT_R8_SNORM:
		case DXGI_FORMAT_R8_SINT:
		case DXGI_FORMAT_A8_UNORM:
		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC3_TYPELESS:
		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_BC5_TYPELESS:
		case DXGI_FORMAT_BC5_UNORM:
		case DXGI_FORMAT_BC5_SNORM:
		case DXGI_FORMAT_BC6H_TYPELESS:
		case DXGI_FORMAT_BC6H_UF16:
		case DXGI_FORMAT_BC6H_SF16:
		case DXGI_FORMAT_BC7_TYPELESS:
		case DXGI_FORMAT_BC7_UNORM:
		case DXGI_FORMAT_BC7_UNORM_SRGB:
		case DXGI_FORMAT_AI44:
		case DXGI_FORMAT_IA44:
		case DXGI_FORMAT_P8:
#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
		case DXGI_FORMAT_R4G4_UNORM:
#endif
			return 8;

		case DXGI_FORMAT_R1_UNORM:
			return 1;

		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC4_TYPELESS:
		case DXGI_FORMAT_BC4_UNORM:
		case DXGI_FORMAT_BC4_SNORM:
			return 4;

		case DXGI_FORMAT_UNKNOWN:
		case DXGI_FORMAT_FORCE_UINT:
		default:
			return 0;
		}
	}
	*/

	FORCEINLINE DXGI_FORMAT MakeSRGB(DXGI_FORMAT format) noexcept;

	FORCEINLINE bool IsDepthStencil(DXGI_FORMAT fmt) noexcept
	{
		switch (fmt)
		{
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		case DXGI_FORMAT_D16_UNORM:

#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
		case DXGI_FORMAT_D16_UNORM_S8_UINT:
		case DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X16_TYPELESS_G8_UINT:
#endif
			return true;

		default:
			return false;
		}
	}

	//--------------------------------------------------------------------------------------
	FORCEINLINE DXGI_FORMAT EnsureNotTypeless(DXGI_FORMAT fmt) noexcept
	{
		// Assumes UNORM or FLOAT; doesn't use UINT or SINT
		switch (fmt)
		{
		case DXGI_FORMAT_R32G32B32A32_TYPELESS: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case DXGI_FORMAT_R32G32B32_TYPELESS:    return DXGI_FORMAT_R32G32B32_FLOAT;
		case DXGI_FORMAT_R16G16B16A16_TYPELESS: return DXGI_FORMAT_R16G16B16A16_UNORM;
		case DXGI_FORMAT_R32G32_TYPELESS:       return DXGI_FORMAT_R32G32_FLOAT;
		case DXGI_FORMAT_R10G10B10A2_TYPELESS:  return DXGI_FORMAT_R10G10B10A2_UNORM;
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:     return DXGI_FORMAT_R8G8B8A8_UNORM;
		case DXGI_FORMAT_R16G16_TYPELESS:       return DXGI_FORMAT_R16G16_UNORM;
		case DXGI_FORMAT_R32_TYPELESS:          return DXGI_FORMAT_R32_FLOAT;
		case DXGI_FORMAT_R8G8_TYPELESS:         return DXGI_FORMAT_R8G8_UNORM;
		case DXGI_FORMAT_R16_TYPELESS:          return DXGI_FORMAT_R16_UNORM;
		case DXGI_FORMAT_R8_TYPELESS:           return DXGI_FORMAT_R8_UNORM;
		case DXGI_FORMAT_BC1_TYPELESS:          return DXGI_FORMAT_BC1_UNORM;
		case DXGI_FORMAT_BC2_TYPELESS:          return DXGI_FORMAT_BC2_UNORM;
		case DXGI_FORMAT_BC3_TYPELESS:          return DXGI_FORMAT_BC3_UNORM;
		case DXGI_FORMAT_BC4_TYPELESS:          return DXGI_FORMAT_BC4_UNORM;
		case DXGI_FORMAT_BC5_TYPELESS:          return DXGI_FORMAT_BC5_UNORM;
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:     return DXGI_FORMAT_B8G8R8A8_UNORM;
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:     return DXGI_FORMAT_B8G8R8X8_UNORM;
		case DXGI_FORMAT_BC7_TYPELESS:          return DXGI_FORMAT_BC7_UNORM;
		default:                                return fmt;
		}
	}


	//--------------------------------------------------------------------------------------
	FORCEINLINE bool IsCompressed(_In_ DXGI_FORMAT fmt) noexcept
	{
		switch (fmt)
		{
		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC3_TYPELESS:
		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_BC4_TYPELESS:
		case DXGI_FORMAT_BC4_UNORM:
		case DXGI_FORMAT_BC4_SNORM:
		case DXGI_FORMAT_BC5_TYPELESS:
		case DXGI_FORMAT_BC5_UNORM:
		case DXGI_FORMAT_BC5_SNORM:
		case DXGI_FORMAT_BC6H_TYPELESS:
		case DXGI_FORMAT_BC6H_UF16:
		case DXGI_FORMAT_BC6H_SF16:
		case DXGI_FORMAT_BC7_TYPELESS:
		case DXGI_FORMAT_BC7_UNORM:
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			return true;

		default:
			return false;
		}
	}

	FORCEINLINE uint32_t CountMips(uint32_t width, uint32_t height) noexcept;

	//--------------------------------------------------------------------------------------
	// Get surface information for a particular format
	//--------------------------------------------------------------------------------------
	HRESULT GetSurfaceInfo(
		size_t width,
		size_t height,
		DXGI_FORMAT fmt,
		size_t* outNumBytes,
		size_t* outRowBytes,
		size_t* outNumRows) noexcept;

	FTexture LoadWICTexture(const std::wstring& fileName, EWIC_LOADER_FLAGS loadFlags = EWIC_LOADER_FLAGS::WIC_LOADER_DEFAULT);

	void ExportWICTexture(const std::wstring& fileName, const FTexture& texture, REFGUID guidContainerFormat = GUID_ContainerFormatPng, const GUID* targetFormat = nullptr, bool forceSRGB = false);

    //--------------------------------------------------------------------------------------
	HRESULT LoadDDSTextureDataFromFile(
		const std::wstring& fileName,
		std::unique_ptr<uint8_t[]>& ddsData,
		const DirectX::DDS_HEADER** header,
		const uint8_t** bitData,
		size_t* bitSize) noexcept;

	//--------------------------------------------------------------------------------------
	class AutoDeleteFile
	{
	public:
		AutoDeleteFile(HANDLE hFile) noexcept : m_handle(hFile) {}

		AutoDeleteFile(const AutoDeleteFile&) = delete;
		AutoDeleteFile& operator=(const AutoDeleteFile&) = delete;

		AutoDeleteFile(const AutoDeleteFile&&) = delete;
		AutoDeleteFile& operator=(const AutoDeleteFile&&) = delete;

		~AutoDeleteFile()
		{
			if (m_handle)
			{
				FILE_DISPOSITION_INFO info = {};
				info.DeleteFile = TRUE;
				(void)SetFileInformationByHandle(m_handle, FileDispositionInfo, &info, sizeof(info));
			}
		}

		void clear() noexcept { m_handle = nullptr; }

	private:
		HANDLE m_handle;
	};

	class AutoDeleteFileWic
	{
	public:
		AutoDeleteFileWic(Microsoft::WRL::ComPtr<IWICStream>& hFile, LPCWSTR szFile) noexcept : m_filename(szFile), m_handle(hFile) {}

		AutoDeleteFileWic(const AutoDeleteFileWic&) = delete;
		AutoDeleteFileWic& operator=(const AutoDeleteFileWic&) = delete;

		AutoDeleteFileWic(const AutoDeleteFileWic&&) = delete;
		AutoDeleteFileWic& operator=(const AutoDeleteFileWic&&) = delete;

		~AutoDeleteFileWic()
		{
			if (m_filename)
			{
				m_handle.Reset();
				DeleteFileW(m_filename);
			}
		}

		void clear() noexcept { m_filename = nullptr; }

	private:
		LPCWSTR m_filename;
		Microsoft::WRL::ComPtr<IWICStream>& m_handle;
	};

	IWICImagingFactory2* _GetWIC() noexcept;
}