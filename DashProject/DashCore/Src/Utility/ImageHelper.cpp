#include "PCH.h"

#include "ImageHelper.h"
#include "Graphics/DX12Helper.h"
#include "Graphics/ResourceFormat.h"

namespace Dash
{
	FORCEINLINE static uint8_t GetSRGBFromLinear(float value)
	{
		float FloatR = FMath::Clamp(value, 0.0f, 1.0f);

		FloatR = FloatR <= 0.0031308f ? FloatR * 12.92f : FMath::Pow(FloatR, 1.0f / 2.4f) * 1.055f - 0.055f;

		return FMath::FloorToInt(FloatR * 255.999f);
	}

	void GetImageColor(FColor& color, const FVector2i& index, const FTexture* image, bool repeat)
	{
		ASSERT(image != nullptr);

		EDASH_FORMAT format = EDASH_FORMAT::UnKwon;

		switch (format)
		{
		case Dash::EDASH_FORMAT::R8G8B8A8_UINT:
		{
			color = image->GetPixel<FColor>(index.X, index.Y);
			break;
		}
		case Dash::EDASH_FORMAT::R32_FLOAT:
		{
			float fvalue = image->GetPixel<float>(index.X, index.Y);
			const uint8_t ivalue = GetSRGBFromLinear(fvalue);
			color = repeat ? FColor{ ivalue, ivalue, ivalue, 255 } : FColor{ ivalue, 0, 0, 255 };
			break;
		}
		case Dash::EDASH_FORMAT::R32G32_FLOAT:
		{
			FVector2f fvalue = image->GetPixel<FVector2f>(index.X, index.Y);
			const uint8_t ir = GetSRGBFromLinear(fvalue.X);
			const uint8_t ig = GetSRGBFromLinear(fvalue.Y);
			color = FColor{ ir, ig, 0, 255 };
			break;
		}
		case Dash::EDASH_FORMAT::R32G32B32_FLOAT:
		{
			FVector3f fvalue = image->GetPixel<FVector3f>(index.X, index.Y);
			const uint8_t ir = GetSRGBFromLinear(fvalue.X);
			const uint8_t ig = GetSRGBFromLinear(fvalue.Y);
			const uint8_t ib = GetSRGBFromLinear(fvalue.Z);
			color = FColor{ ir, ig, ib, 255 };
			break;
		}
		case Dash::EDASH_FORMAT::R32G32B32A32_FLOAT:
		{
			FVector4f fvalue = image->GetPixel<FVector4f>(index.X, index.Y);
			const uint8_t ir = GetSRGBFromLinear(fvalue.X);
			const uint8_t ig = GetSRGBFromLinear(fvalue.Y);
			const uint8_t ib = GetSRGBFromLinear(fvalue.Z);
			const uint8_t ia = FMath::FloorToInt(FMath::Clamp(fvalue.W, 0.0f, 1.0f) * 255.999f);
			color = FColor{ ir, ig, ib, ia };
			break;
		}
		default:
			ASSERT_FAIL("Unsupported Type!");
			break;
		}
	}

	FORCEINLINE void SavePPMImage(const FTexture* image, const std::string& name)
	{
		std::ofstream output;
		output.open(name, std::ios::binary);

		if (output.fail())
		{
			output.close();
			return;
		}

		std::size_t imageWidth = image->GetWidth();
		std::size_t imageHeight = image->GetHeight();

		output << "P6\n" << imageWidth << " " << imageHeight << "\n";

		float maxValue = 255.0f;
		output << maxValue << "\n";

		for (std::size_t i = 0; i < imageHeight; ++i)
		{
			for (std::size_t j = 0; j < imageWidth; ++j)
			{
				FColor color;

				GetImageColor(color, FVector2i{ j, i }, image);

				std::uint8_t r = color.r;
				std::uint8_t g = color.g;
				std::uint8_t b = color.b;

				output << r << g << b;
			}
		}

		output.close();
	}

	FORCEINLINE FTexture LoadPPMImage(const std::string& name)
	{
		std::ifstream input;
		input.open(name, std::ios::binary);
		FTexture image;

		if (input.fail())
		{
			input.close();
			return image;
		}

		std::string fileType;
		input >> fileType;

		if (fileType != "P6")
		{
			input.close();
			return image;
		}

		std::size_t imageWidth;
		std::size_t imageHeight;
		float maxValue;
		input >> imageWidth >> imageHeight;
		input >> maxValue;

		float invMaxValue = 1 / maxValue;

		image.Resize(imageWidth, imageHeight);

		input.ignore(256, '\n');

		FColor currentPixel{ 0, 0, 0, 255 };
		for (std::size_t i = 0; i < imageHeight; i++)
		{
			for (std::size_t j = 0; j < imageWidth; j++)
			{
				input.read(reinterpret_cast<char*>(&(currentPixel.AlignmentDummy)), 3);

				FLinearColor color{ currentPixel };

				image.SetPixel(color, j, i);
			}
		}

		input.close();
		return image;
	}

	//-------------------------------------------------------------------------------------
	// WIC Pixel Format Translation Data
	//-------------------------------------------------------------------------------------
	struct WICTranslate
	{
		GUID                wic;
		DXGI_FORMAT         format;
	};

	const WICTranslate g_WICFormats[] =
	{
		{ GUID_WICPixelFormat128bppRGBAFloat,       DXGI_FORMAT_R32G32B32A32_FLOAT },

		{ GUID_WICPixelFormat64bppRGBAHalf,         DXGI_FORMAT_R16G16B16A16_FLOAT },
		{ GUID_WICPixelFormat64bppRGBA,             DXGI_FORMAT_R16G16B16A16_UNORM },

		{ GUID_WICPixelFormat32bppRGBA,             DXGI_FORMAT_R8G8B8A8_UNORM },
		{ GUID_WICPixelFormat32bppBGRA,             DXGI_FORMAT_B8G8R8A8_UNORM },
		{ GUID_WICPixelFormat32bppBGR,              DXGI_FORMAT_B8G8R8X8_UNORM },

		{ GUID_WICPixelFormat32bppRGBA1010102XR,    DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM },
		{ GUID_WICPixelFormat32bppRGBA1010102,      DXGI_FORMAT_R10G10B10A2_UNORM },

		{ GUID_WICPixelFormat16bppBGRA5551,         DXGI_FORMAT_B5G5R5A1_UNORM },
		{ GUID_WICPixelFormat16bppBGR565,           DXGI_FORMAT_B5G6R5_UNORM },

		{ GUID_WICPixelFormat32bppGrayFloat,        DXGI_FORMAT_R32_FLOAT },
		{ GUID_WICPixelFormat16bppGrayHalf,         DXGI_FORMAT_R16_FLOAT },
		{ GUID_WICPixelFormat16bppGray,             DXGI_FORMAT_R16_UNORM },
		{ GUID_WICPixelFormat8bppGray,              DXGI_FORMAT_R8_UNORM },

		{ GUID_WICPixelFormat8bppAlpha,             DXGI_FORMAT_A8_UNORM },

		{ GUID_WICPixelFormat96bppRGBFloat,         DXGI_FORMAT_R32G32B32_FLOAT },
	};

	//-------------------------------------------------------------------------------------
	// WIC Pixel Format nearest conversion table
	//-------------------------------------------------------------------------------------

	struct WICConvert
	{
		GUID        source;
		GUID        target;
	};

	const WICConvert g_WICConvert[] =
	{
		// Note target GUID in this conversion table must be one of those directly supported formats (above).

		{ GUID_WICPixelFormatBlackWhite,            GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM

		{ GUID_WICPixelFormat1bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat2bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat4bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat8bppIndexed,           GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 

		{ GUID_WICPixelFormat2bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM 
		{ GUID_WICPixelFormat4bppGray,              GUID_WICPixelFormat8bppGray }, // DXGI_FORMAT_R8_UNORM 

		{ GUID_WICPixelFormat16bppGrayFixedPoint,   GUID_WICPixelFormat16bppGrayHalf }, // DXGI_FORMAT_R16_FLOAT 
		{ GUID_WICPixelFormat32bppGrayFixedPoint,   GUID_WICPixelFormat32bppGrayFloat }, // DXGI_FORMAT_R32_FLOAT 

		{ GUID_WICPixelFormat16bppBGR555,           GUID_WICPixelFormat16bppBGRA5551 }, // DXGI_FORMAT_B5G5R5A1_UNORM

		{ GUID_WICPixelFormat32bppBGR101010,        GUID_WICPixelFormat32bppRGBA1010102 }, // DXGI_FORMAT_R10G10B10A2_UNORM

		{ GUID_WICPixelFormat24bppBGR,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat24bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat32bppPBGRA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 
		{ GUID_WICPixelFormat32bppPRGBA,            GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM 

		{ GUID_WICPixelFormat48bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat48bppBGR,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat64bppBGRA,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat64bppPRGBA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat64bppPBGRA,            GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM

		{ GUID_WICPixelFormat48bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
		{ GUID_WICPixelFormat48bppBGRFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
		{ GUID_WICPixelFormat64bppRGBAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
		{ GUID_WICPixelFormat64bppBGRAFixedPoint,   GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
		{ GUID_WICPixelFormat64bppRGBFixedPoint,    GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
		{ GUID_WICPixelFormat64bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 
		{ GUID_WICPixelFormat48bppRGBHalf,          GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT 

		{ GUID_WICPixelFormat128bppPRGBAFloat,      GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
		{ GUID_WICPixelFormat128bppRGBFloat,        GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
		{ GUID_WICPixelFormat128bppRGBAFixedPoint,  GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
		{ GUID_WICPixelFormat128bppRGBFixedPoint,   GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 
		{ GUID_WICPixelFormat32bppRGBE,             GUID_WICPixelFormat128bppRGBAFloat }, // DXGI_FORMAT_R32G32B32A32_FLOAT 

		{ GUID_WICPixelFormat32bppCMYK,             GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
		{ GUID_WICPixelFormat64bppCMYK,             GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat40bppCMYKAlpha,        GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
		{ GUID_WICPixelFormat80bppCMYKAlpha,        GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM

		{ GUID_WICPixelFormat32bppRGB,              GUID_WICPixelFormat32bppRGBA }, // DXGI_FORMAT_R8G8B8A8_UNORM
		{ GUID_WICPixelFormat64bppRGB,              GUID_WICPixelFormat64bppRGBA }, // DXGI_FORMAT_R16G16B16A16_UNORM
		{ GUID_WICPixelFormat64bppPRGBAHalf,        GUID_WICPixelFormat64bppRGBAHalf }, // DXGI_FORMAT_R16G16B16A16_FLOAT

		{ GUID_WICPixelFormat96bppRGBFixedPoint,   GUID_WICPixelFormat96bppRGBFloat }, // DXGI_FORMAT_R32G32B32_FLOAT

		// We don't support n-channel formats
	};

	struct DXGIFormatTranslate
	{
		DXGI_FORMAT         DxgiFormat;
		EDASH_FORMAT        DashFormat;
	};

	const DXGIFormatTranslate g_DXGIFormatTranslator[] =
	{
		{ DXGI_FORMAT_R32G32B32A32_FLOAT,		  EDASH_FORMAT::R32G32B32A32_FLOAT },

		{ DXGI_FORMAT_R16G16B16A16_FLOAT,         EDASH_FORMAT::R16G16B16A16_FLOAT },
		{ DXGI_FORMAT_R16G16B16A16_UNORM,         EDASH_FORMAT::R16G16B16A16_UNORM},

		{ DXGI_FORMAT_R8G8B8A8_UNORM,             EDASH_FORMAT::R8G8B8A8_UNORM },
		{ DXGI_FORMAT_B8G8R8A8_UNORM,             EDASH_FORMAT::B8G8R8A8_UNORM },
		{ DXGI_FORMAT_B8G8R8X8_UNORM,             EDASH_FORMAT::B8G8R8X8_UNORM },

		{ DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM, EDASH_FORMAT::R10G10B10_XR_BIAS_A2_UNORM },
		{ DXGI_FORMAT_R10G10B10A2_UNORM,		  EDASH_FORMAT::R10G10B10A2_UNORM },

		{ DXGI_FORMAT_B5G5R5A1_UNORM,			  EDASH_FORMAT::B5G5R5A1_UNORM },
		{ DXGI_FORMAT_B5G6R5_UNORM,				  EDASH_FORMAT::B5G6R5_UNORM },

		{ DXGI_FORMAT_R32_FLOAT,				  EDASH_FORMAT::R32_FLOAT },
		{ DXGI_FORMAT_R16_FLOAT,				  EDASH_FORMAT::R16_FLOAT },
		{ DXGI_FORMAT_R16_UNORM,				  EDASH_FORMAT::R16_UNORM },
		{ DXGI_FORMAT_R8_UNORM,                   EDASH_FORMAT::R8_UNORM },

		{ DXGI_FORMAT_A8_UNORM,					  EDASH_FORMAT::A8_UNORM },

		{ DXGI_FORMAT_R32G32B32_FLOAT,			  EDASH_FORMAT::R32G32B32_FLOAT },
	};

	FORCEINLINE EDASH_FORMAT DXGIFormatToDashFormat(DXGI_FORMAT format)
	{
		for (size_t i = 0; i < _countof(g_DXGIFormatTranslator); ++i)
		{
			if (format == g_DXGIFormatTranslator[i].DxgiFormat)
			{
				return g_DXGIFormatTranslator[i].DashFormat;
			}
		}

		return EDASH_FORMAT::UnKwon;
	}

	FORCEINLINE DXGI_FORMAT DashFormatToDXGIFormat(EDASH_FORMAT format)
	{
		for (size_t i = 0; i < _countof(g_DXGIFormatTranslator); ++i)
		{
			if (format == g_DXGIFormatTranslator[i].DashFormat)
			{
				return g_DXGIFormatTranslator[i].DxgiFormat;
			}
		}

		return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
	}

	BOOL WINAPI InitializeWICFactory(PINIT_ONCE, PVOID, PVOID* ifactory) noexcept
	{
		::CoInitialize(nullptr);

		return SUCCEEDED(CoCreateInstance(
			CLSID_WICImagingFactory2,
			nullptr,
			CLSCTX_INPROC_SERVER,
			__uuidof(IWICImagingFactory2),
			ifactory)) ? TRUE : FALSE;
	}

	// Also used by ScreenGrab
	IWICImagingFactory2* _GetWIC() noexcept
	{
		static INIT_ONCE s_initOnce = INIT_ONCE_STATIC_INIT;

		IWICImagingFactory2* factory = nullptr;
		if (!InitOnceExecuteOnce(
			&s_initOnce,
			InitializeWICFactory,
			nullptr,
			reinterpret_cast<LPVOID*>(&factory)))
		{
			return nullptr;
		}

		return factory;
	}


	DXGI_FORMAT _WICToDXGI(const GUID& guid) noexcept
	{
		for (size_t i = 0; i < _countof(g_WICFormats); ++i)
		{
			if (memcmp(&g_WICFormats[i].wic, &guid, sizeof(GUID)) == 0)
				return g_WICFormats[i].format;
		}

		return DXGI_FORMAT_UNKNOWN;
	}

	size_t _WICBitsPerPixel(REFGUID targetGuid) noexcept
	{
		auto pWIC = _GetWIC();
		if (!pWIC)
			return 0;

		Microsoft::WRL::ComPtr<IWICComponentInfo> cinfo;
		if (FAILED(pWIC->CreateComponentInfo(targetGuid, cinfo.GetAddressOf())))
			return 0;

		WICComponentType type;
		if (FAILED(cinfo->GetComponentType(&type)))
			return 0;

		if (type != WICPixelFormat)
			return 0;

		Microsoft::WRL::ComPtr<IWICPixelFormatInfo> pfinfo;
		if (FAILED(cinfo.As(&pfinfo)))
			return 0;

		UINT bpp;
		if (FAILED(pfinfo->GetBitsPerPixel(&bpp)))
			return 0;

		return bpp;
	}

	inline void FitPowerOf2(UINT origx, UINT origy, UINT& targetx, UINT& targety, size_t maxsize)
	{
		float origAR = float(origx) / float(origy);

		if (origx > origy)
		{
			size_t x;
			for (x = maxsize; x > 1; x >>= 1) { if (x <= targetx) break; }
			targetx = UINT(x);

			float bestScore = FLT_MAX;
			for (size_t y = maxsize; y > 0; y >>= 1)
			{
				float score = fabsf((float(x) / float(y)) - origAR);
				if (score < bestScore)
				{
					bestScore = score;
					targety = UINT(y);
				}
			}
		}
		else
		{
			size_t y;
			for (y = maxsize; y > 1; y >>= 1) { if (y <= targety) break; }
			targety = UINT(y);

			float bestScore = FLT_MAX;
			for (size_t x = maxsize; x > 0; x >>= 1)
			{
				float score = fabsf((float(x) / float(y)) - origAR);
				if (score < bestScore)
				{
					bestScore = score;
					targetx = UINT(x);
				}
			}
		}
	}

	//--------------------------------------------------------------------------------------
	DXGI_FORMAT MakeSRGB(DXGI_FORMAT format) noexcept
	{
		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

		case DXGI_FORMAT_BC1_UNORM:
			return DXGI_FORMAT_BC1_UNORM_SRGB;

		case DXGI_FORMAT_BC2_UNORM:
			return DXGI_FORMAT_BC2_UNORM_SRGB;

		case DXGI_FORMAT_BC3_UNORM:
			return DXGI_FORMAT_BC3_UNORM_SRGB;

		case DXGI_FORMAT_B8G8R8A8_UNORM:
			return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

		case DXGI_FORMAT_B8G8R8X8_UNORM:
			return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

		case DXGI_FORMAT_BC7_UNORM:
			return DXGI_FORMAT_BC7_UNORM_SRGB;

		default:
			return format;
		}
	}

	FORCEINLINE uint32_t CountMips(uint32_t width, uint32_t height) noexcept
	{
		if (width == 0 || height == 0)
			return 0;

		uint32_t count = 1;
		while (width > 1 || height > 1)
		{
			width >>= 1;
			height >>= 1;
			count++;
		}
		return count;
	}

	HRESULT GetSurfaceInfo(size_t width, size_t height, DXGI_FORMAT fmt, size_t* outNumBytes, size_t* outRowBytes, size_t* outNumRows) noexcept
	{
		uint64_t numBytes = 0;
		uint64_t rowBytes = 0;
		uint64_t numRows = 0;

		bool bc = false;
		bool packed = false;
		bool planar = false;
		size_t bpe = 0;
		switch (fmt)
		{
		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC4_TYPELESS:
		case DXGI_FORMAT_BC4_UNORM:
		case DXGI_FORMAT_BC4_SNORM:
			bc = true;
			bpe = 8;
			break;

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
			bc = true;
			bpe = 16;
			break;

		case DXGI_FORMAT_R8G8_B8G8_UNORM:
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
		case DXGI_FORMAT_YUY2:
			packed = true;
			bpe = 4;
			break;

		case DXGI_FORMAT_Y210:
		case DXGI_FORMAT_Y216:
			packed = true;
			bpe = 8;
			break;

		case DXGI_FORMAT_NV12:
		case DXGI_FORMAT_420_OPAQUE:
#if (_WIN32_WINNT >= _WIN32_WINNT_WIN10)
		case DXGI_FORMAT_P208:
#endif
			planar = true;
			bpe = 2;
			break;

		case DXGI_FORMAT_P010:
		case DXGI_FORMAT_P016:
			planar = true;
			bpe = 4;
			break;

#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)

		case DXGI_FORMAT_D16_UNORM_S8_UINT:
		case DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X16_TYPELESS_G8_UINT:
			planar = true;
			bpe = 4;
			break;

#endif

		default:
			break;
		}

		if (bc)
		{
			uint64_t numBlocksWide = 0;
			if (width > 0)
			{
				numBlocksWide = std::max<uint64_t>(1u, (uint64_t(width) + 3u) / 4u);
			}
			uint64_t numBlocksHigh = 0;
			if (height > 0)
			{
				numBlocksHigh = std::max<uint64_t>(1u, (uint64_t(height) + 3u) / 4u);
			}
			rowBytes = numBlocksWide * bpe;
			numRows = numBlocksHigh;
			numBytes = rowBytes * numBlocksHigh;
		}
		else if (packed)
		{
			rowBytes = ((uint64_t(width) + 1u) >> 1) * bpe;
			numRows = uint64_t(height);
			numBytes = rowBytes * height;
		}
		else if (fmt == DXGI_FORMAT_NV11)
		{
			rowBytes = ((uint64_t(width) + 3u) >> 2) * 4u;
			numRows = uint64_t(height) * 2u; // Direct3D makes this simplifying assumption, although it is larger than the 4:1:1 data
			numBytes = rowBytes * numRows;
		}
		else if (planar)
		{
			rowBytes = ((uint64_t(width) + 1u) >> 1) * bpe;
			numBytes = (rowBytes * uint64_t(height)) + ((rowBytes * uint64_t(height) + 1u) >> 1);
			numRows = height + ((uint64_t(height) + 1u) >> 1);
		}
		else
		{
			size_t bpp = BitsPerPixel(fmt);
			if (!bpp)
				return E_INVALIDARG;

			rowBytes = (uint64_t(width) * bpp + 7u) / 8u; // round up to nearest byte
			numRows = uint64_t(height);
			numBytes = rowBytes * height;
		}

#if defined(_M_IX86) || defined(_M_ARM) || defined(_M_HYBRID_X86_ARM64)
		static_assert(sizeof(size_t) == 4, "Not a 32-bit platform!");
		if (numBytes > UINT32_MAX || rowBytes > UINT32_MAX || numRows > UINT32_MAX)
			return HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW);
#else
		static_assert(sizeof(size_t) == 8, "Not a 64-bit platform!");
#endif

		if (outNumBytes)
		{
			*outNumBytes = static_cast<size_t>(numBytes);
		}
		if (outRowBytes)
		{
			*outRowBytes = static_cast<size_t>(rowBytes);
		}
		if (outNumRows)
		{
			*outNumRows = static_cast<size_t>(numRows);
		}

		return S_OK;
	}

	FTexture LoadWICTexture(const std::wstring& fileName, EWIC_LOADER_FLAGS loadFlags)
	{
		auto pWIC = _GetWIC();
		if (!pWIC)
		{
			ASSERT_FAIL("Get WIC Factory Failed!");
		}
			
		// Initialize WIC
		Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
		DX_CALL(pWIC->CreateDecoderFromFilename(fileName.c_str(),
			nullptr,
			GENERIC_READ,
			WICDecodeMetadataCacheOnDemand,
			decoder.GetAddressOf()));

		Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
		DX_CALL(decoder->GetFrame(0, frame.GetAddressOf()));

		UINT width, height;
		DX_CALL(frame->GetSize(&width, &height));

		assert(width > 0 && height > 0);

		size_t maxsize = 16384;

		UINT twidth = width;
		UINT theight = height;
		if (Valid(loadFlags & EWIC_LOADER_FLAGS::WIC_LOADER_FIT_POW2))
		{
			FitPowerOf2(width, height, twidth, theight, maxsize);
		}
		else if (width > maxsize || height > maxsize)
		{
			float ar = static_cast<float>(height) / static_cast<float>(width);
			if (width > height)
			{
				twidth = static_cast<UINT>(maxsize);
				theight = std::max<UINT>(1, static_cast<UINT>(static_cast<float>(maxsize) * ar));
			}
			else
			{
				theight = static_cast<UINT>(maxsize);
				twidth = std::max<UINT>(1, static_cast<UINT>(static_cast<float>(maxsize) / ar));
			}
			assert(twidth <= maxsize && theight <= maxsize);
		}

		if (Valid(loadFlags & EWIC_LOADER_FLAGS::WIC_LOADER_MAKE_SQUARE))
		{
			twidth = std::max<UINT>(twidth, theight);
			theight = twidth;
		}

		// Determine format
		WICPixelFormatGUID pixelFormat;
		DX_CALL(frame->GetPixelFormat(&pixelFormat));

		WICPixelFormatGUID convertGUID;
		memcpy_s(&convertGUID, sizeof(WICPixelFormatGUID), &pixelFormat, sizeof(GUID));

		size_t bpp = 0;

		DXGI_FORMAT format = _WICToDXGI(pixelFormat);
		if (format == DXGI_FORMAT_UNKNOWN)
		{
			for (size_t i = 0; i < _countof(g_WICConvert); ++i)
			{
				if (memcmp(&g_WICConvert[i].source, &pixelFormat, sizeof(WICPixelFormatGUID)) == 0)
				{
					memcpy_s(&convertGUID, sizeof(WICPixelFormatGUID), &g_WICConvert[i].target, sizeof(GUID));

					format = _WICToDXGI(g_WICConvert[i].target);
					ASSERT(format != DXGI_FORMAT_UNKNOWN);
					bpp = _WICBitsPerPixel(convertGUID);
					break;
				}
			}

			if (format == DXGI_FORMAT_UNKNOWN)
			{
				ASSERT_FAIL("ERROR: WICTextureLoader does not support all DXGI formats (WIC GUID {%8.8lX-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X}). Consider using DirectXTex.\n",
					pixelFormat.Data1, pixelFormat.Data2, pixelFormat.Data3,
					pixelFormat.Data4[0], pixelFormat.Data4[1], pixelFormat.Data4[2], pixelFormat.Data4[3],
					pixelFormat.Data4[4], pixelFormat.Data4[5], pixelFormat.Data4[6], pixelFormat.Data4[7]);
				//return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
			}
		}
		else
		{
			bpp = _WICBitsPerPixel(pixelFormat);
		}

		if (Valid(loadFlags & EWIC_LOADER_FLAGS::WIC_LOADER_FORCE_RGBA32))
		{
			memcpy_s(&convertGUID, sizeof(WICPixelFormatGUID), &GUID_WICPixelFormat32bppRGBA, sizeof(GUID));
			format = DXGI_FORMAT_R8G8B8A8_UNORM;
			bpp = 32;
		}

		if (!bpp)
		{
			ASSERT_FAIL("Failed to get bpp");
		}	

		// Handle sRGB formats
		if (Valid(loadFlags & EWIC_LOADER_FLAGS::WIC_LOADER_FORCE_SRGB))
		{
			format = MakeSRGB(format);
		}
		else if (!Valid(loadFlags & EWIC_LOADER_FLAGS::WIC_LOADER_IGNORE_SRGB))
		{
			Microsoft::WRL::ComPtr<IWICMetadataQueryReader> metareader;
			if (SUCCEEDED(frame->GetMetadataQueryReader(metareader.GetAddressOf())))
			{
				GUID containerFormat;
				if (SUCCEEDED(metareader->GetContainerFormat(&containerFormat)))
				{
					bool sRGB = false;

					PROPVARIANT value;
					PropVariantInit(&value);

					// Check for colorspace chunks
					if (memcmp(&containerFormat, &GUID_ContainerFormatPng, sizeof(GUID)) == 0)
					{
						// Check for sRGB chunk
						if (SUCCEEDED(metareader->GetMetadataByName(L"/sRGB/RenderingIntent", &value)) && value.vt == VT_UI1)
						{
							sRGB = true;
						}
						else if (SUCCEEDED(metareader->GetMetadataByName(L"/gAMA/ImageGamma", &value)) && value.vt == VT_UI4)
						{
							sRGB = (value.uintVal == 45455);
						}
						else
						{
							sRGB = Valid(loadFlags & EWIC_LOADER_FLAGS::WIC_LOADER_SRGB_DEFAULT);
						}
					}
#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
					else if (memcmp(&containerFormat, &GUID_ContainerFormatJpeg, sizeof(GUID)) == 0)
					{
						if (SUCCEEDED(metareader->GetMetadataByName(L"/app1/ifd/exif/{ushort=40961}", &value)) && value.vt == VT_UI2)
						{
							sRGB = (value.uiVal == 1);
						}
						else
						{
							sRGB = (loadFlags & WIC_LOADER_SRGB_DEFAULT) != 0;
						}
					}
					else if (memcmp(&containerFormat, &GUID_ContainerFormatTiff, sizeof(GUID)) == 0)
					{
						if (SUCCEEDED(metareader->GetMetadataByName(L"/ifd/exif/{ushort=40961}", &value)) && value.vt == VT_UI2)
						{
							sRGB = (value.uiVal == 1);
						}
						else
						{
							sRGB = (loadFlags & WIC_LOADER_SRGB_DEFAULT) != 0;
						}
					}
#else
					else if (SUCCEEDED(metareader->GetMetadataByName(L"System.Image.ColorSpace", &value)) && value.vt == VT_UI2)
					{
						sRGB = (value.uiVal == 1);
					}
					else
					{
						sRGB = Valid(loadFlags & EWIC_LOADER_FLAGS::WIC_LOADER_SRGB_DEFAULT);
					}
#endif

					(void)PropVariantClear(&value);

					if (sRGB)
						format = MakeSRGB(format);
				}
			}
		}

		// Allocate memory for decoded image
		uint64_t rowBytes = (uint64_t(twidth) * uint64_t(bpp) + 7u) / 8u;
		uint64_t numBytes = rowBytes * uint64_t(height);

		if (rowBytes > UINT32_MAX || numBytes > UINT32_MAX)
		{
			ASSERT_FAIL("Arithmetic Overflow!");
		}	

		auto rowPitch = static_cast<size_t>(rowBytes);
		auto imageSize = static_cast<size_t>(numBytes);


		EDASH_FORMAT dashFormat = DXGIFormatToDashFormat(format);
		ASSERT(dashFormat != EDASH_FORMAT::UnKwon);
		FTexture texture{twidth, theight, dashFormat};

		// Load image data
		if (memcmp(&convertGUID, &pixelFormat, sizeof(GUID)) == 0
			&& twidth == width
			&& theight == height)
		{
			// No format conversion or resize needed
			DX_CALL(frame->CopyPixels(nullptr, static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize), texture.GetRawData()));
		}
		else if (twidth != width || theight != height)
		{
			// Resize
			//auto pWIC = _GetWIC();
			if (!pWIC)
			{
				ASSERT_FAIL("Get WIC Factory Failed!");
			}	

			Microsoft::WRL::ComPtr<IWICBitmapScaler> scaler;
			DX_CALL(pWIC->CreateBitmapScaler(scaler.GetAddressOf()));

			DX_CALL(scaler->Initialize(frame.Get(), twidth, theight, WICBitmapInterpolationModeFant));

			WICPixelFormatGUID pfScaler;
			DX_CALL(scaler->GetPixelFormat(&pfScaler));

			if (memcmp(&convertGUID, &pfScaler, sizeof(GUID)) == 0)
			{
				// No format conversion needed
				DX_CALL(scaler->CopyPixels(nullptr, static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize), texture.GetRawData()));
			}
			else
			{
				Microsoft::WRL::ComPtr<IWICFormatConverter> FC;
				DX_CALL(pWIC->CreateFormatConverter(FC.GetAddressOf()));

				BOOL canConvert = FALSE;
				DX_CALL(FC->CanConvert(pfScaler, convertGUID, &canConvert));
				if (!canConvert)
				{
					ASSERT_FAIL("Can not convert format!");
				}

				DX_CALL(FC->Initialize(scaler.Get(), convertGUID, WICBitmapDitherTypeErrorDiffusion, nullptr, 0, WICBitmapPaletteTypeMedianCut));

				DX_CALL(FC->CopyPixels(nullptr, static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize), texture.GetRawData()));
			}
		}
		else
		{
			// Format conversion but no resize
			auto pWIC = _GetWIC();
			if (!pWIC)
			{
				ASSERT_FAIL("Get WIC Factory Failed!");
			}		

			Microsoft::WRL::ComPtr<IWICFormatConverter> FC;
			DX_CALL(pWIC->CreateFormatConverter(FC.GetAddressOf()));

			BOOL canConvert = FALSE;
			DX_CALL(FC->CanConvert(pixelFormat, convertGUID, &canConvert));
			if (!canConvert)
			{
				ASSERT_FAIL("Can not convert format!");
			}

			DX_CALL(FC->Initialize(frame.Get(), convertGUID, WICBitmapDitherTypeErrorDiffusion, nullptr, 0, WICBitmapPaletteTypeMedianCut));

			DX_CALL(FC->CopyPixels(nullptr, static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize), texture.GetRawData()));
		}

		return texture;
	}

	
	void ExportWICTexture(const std::wstring& fileName, const FTexture& texture, REFGUID guidContainerFormat, const GUID* targetFormat, bool forceSRGB)
	{
		if (fileName.empty())
		{
			ASSERT("Invalid Args");
		}			

		UINT64 fpRowPitch = texture.GetRowPitch();

#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
		// Round up the srcPitch to multiples of 1024
		UINT64 dstRowPitch = (fpRowPitch + static_cast<uint64_t>(D3D12XBOX_TEXTURE_DATA_PITCH_ALIGNMENT) - 1u) & ~(static_cast<uint64_t>(D3D12XBOX_TEXTURE_DATA_PITCH_ALIGNMENT) - 1u);
#else
		// Round up the srcPitch to multiples of 256
		//UINT64 dstRowPitch = (fpRowPitch + 255) & ~0xFFu;
		UINT64 dstRowPitch = fpRowPitch;
#endif

		if (dstRowPitch > UINT32_MAX)
		{
			ASSERT_FAIL("Arithmetic Overflow!");
		}			

		size_t textureWidth = texture.GetWidth();
		size_t textureHeight = texture.GetHeight();
		DXGI_FORMAT dxgiFormat = DashFormatToDXGIFormat(texture.GetFormat());

		// Determine source format's WIC equivalent
		WICPixelFormatGUID pfGuid = {};
		bool sRGB = forceSRGB;
		switch (dxgiFormat)
		{
		case DXGI_FORMAT_R32G32B32A32_FLOAT:            pfGuid = GUID_WICPixelFormat128bppRGBAFloat; break;
		case DXGI_FORMAT_R16G16B16A16_FLOAT:            pfGuid = GUID_WICPixelFormat64bppRGBAHalf; break;
		case DXGI_FORMAT_R16G16B16A16_UNORM:            pfGuid = GUID_WICPixelFormat64bppRGBA; break;
		case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:    pfGuid = GUID_WICPixelFormat32bppRGBA1010102XR; break;
		case DXGI_FORMAT_R10G10B10A2_UNORM:             pfGuid = GUID_WICPixelFormat32bppRGBA1010102; break;
		case DXGI_FORMAT_B5G5R5A1_UNORM:                pfGuid = GUID_WICPixelFormat16bppBGRA5551; break;
		case DXGI_FORMAT_B5G6R5_UNORM:                  pfGuid = GUID_WICPixelFormat16bppBGR565; break;
		case DXGI_FORMAT_R32_FLOAT:                     pfGuid = GUID_WICPixelFormat32bppGrayFloat; break;
		case DXGI_FORMAT_R16_FLOAT:                     pfGuid = GUID_WICPixelFormat16bppGrayHalf; break;
		case DXGI_FORMAT_R16_UNORM:                     pfGuid = GUID_WICPixelFormat16bppGray; break;
		case DXGI_FORMAT_R8_UNORM:                      pfGuid = GUID_WICPixelFormat8bppGray; break;
		case DXGI_FORMAT_A8_UNORM:                      pfGuid = GUID_WICPixelFormat8bppAlpha; break;

		case DXGI_FORMAT_R8G8B8A8_UNORM:
			pfGuid = GUID_WICPixelFormat32bppRGBA;
			break;

		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			pfGuid = GUID_WICPixelFormat32bppRGBA;
			sRGB = true;
			break;

		case DXGI_FORMAT_B8G8R8A8_UNORM:
			pfGuid = GUID_WICPixelFormat32bppBGRA;
			break;

		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			pfGuid = GUID_WICPixelFormat32bppBGRA;
			sRGB = true;
			break;

		case DXGI_FORMAT_B8G8R8X8_UNORM:
			pfGuid = GUID_WICPixelFormat32bppBGR;
			break;

		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			pfGuid = GUID_WICPixelFormat32bppBGR;
			sRGB = true;
			break;

		default:
			ASSERT_FAIL("ERROR: ScreenGrab does not support all DXGI formats (%u). Consider using DirectXTex.\n", static_cast<uint32_t>(dxgiFormat));
		}

		auto pWIC = _GetWIC();
		if (!pWIC)
		{
			ASSERT_FAIL("Get WIC Factory Failed!");
		}		

		Microsoft::WRL::ComPtr<IWICStream> stream;
		DX_CALL(pWIC->CreateStream(stream.GetAddressOf()));

		DX_CALL(stream->InitializeFromFilename(fileName.c_str(), GENERIC_WRITE));

		AutoDeleteFileWic delonfail(stream, fileName.c_str());

		Microsoft::WRL::ComPtr<IWICBitmapEncoder> encoder;
		DX_CALL(pWIC->CreateEncoder(guidContainerFormat, nullptr, encoder.GetAddressOf()));

		DX_CALL(encoder->Initialize(stream.Get(), WICBitmapEncoderNoCache));

		Microsoft::WRL::ComPtr<IWICBitmapFrameEncode> frame;
		Microsoft::WRL::ComPtr<IPropertyBag2> props;
		DX_CALL(encoder->CreateNewFrame(frame.GetAddressOf(), props.GetAddressOf()));

		if (targetFormat && memcmp(&guidContainerFormat, &GUID_ContainerFormatBmp, sizeof(WICPixelFormatGUID)) == 0)
		{
			// Opt-in to the WIC2 support for writing 32-bit Windows BMP files with an alpha channel
			PROPBAG2 option = {};
			option.pstrName = const_cast<wchar_t*>(L"EnableV5Header32bppBGRA");

			VARIANT varValue;
			varValue.vt = VT_BOOL;
			varValue.boolVal = VARIANT_TRUE;
			(void)props->Write(1, &option, &varValue);
		}

		DX_CALL(frame->Initialize(props.Get()));

		DX_CALL(frame->SetSize(static_cast<UINT>(textureWidth), static_cast<UINT>(textureHeight)));

		DX_CALL(frame->SetResolution(72, 72));

		// Pick a target format
		WICPixelFormatGUID targetGuid = {};
		if (targetFormat)
		{
			targetGuid = *targetFormat;
		}
		else
		{
			// Screenshots don't typically include the alpha channel of the render target
			switch (dxgiFormat)
			{
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
			case DXGI_FORMAT_R16G16B16A16_FLOAT:
				targetGuid = GUID_WICPixelFormat96bppRGBFloat; // WIC 2
				break;

			case DXGI_FORMAT_R16G16B16A16_UNORM: targetGuid = GUID_WICPixelFormat48bppBGR; break;
			case DXGI_FORMAT_B5G5R5A1_UNORM:     targetGuid = GUID_WICPixelFormat16bppBGR555; break;
			case DXGI_FORMAT_B5G6R5_UNORM:       targetGuid = GUID_WICPixelFormat16bppBGR565; break;

			case DXGI_FORMAT_R32_FLOAT:
			case DXGI_FORMAT_R16_FLOAT:
			case DXGI_FORMAT_R16_UNORM:
			case DXGI_FORMAT_R8_UNORM:
			case DXGI_FORMAT_A8_UNORM:
				targetGuid = GUID_WICPixelFormat8bppGray;
				break;

			default:
				targetGuid = GUID_WICPixelFormat24bppBGR;
				break;
			}
		}

		DX_CALL(frame->SetPixelFormat(&targetGuid));

		if (targetFormat && memcmp(targetFormat, &targetGuid, sizeof(WICPixelFormatGUID)) != 0)
		{
			// Requested output pixel format is not supported by the WIC codec
			ASSERT_FAIL("Not supported format");
		}

		// Encode WIC metadata
		Microsoft::WRL::ComPtr<IWICMetadataQueryWriter> metawriter;
		if (SUCCEEDED(frame->GetMetadataQueryWriter(metawriter.GetAddressOf())))
		{
			PROPVARIANT value;
			PropVariantInit(&value);

			value.vt = VT_LPSTR;
			value.pszVal = const_cast<char*>("DirectXTK");

			if (memcmp(&guidContainerFormat, &GUID_ContainerFormatPng, sizeof(GUID)) == 0)
			{
				// Set Software name
				(void)metawriter->SetMetadataByName(L"/tEXt/{str=Software}", &value);

				// Set sRGB chunk
				if (sRGB)
				{
					value.vt = VT_UI1;
					value.bVal = 0;
					(void)metawriter->SetMetadataByName(L"/sRGB/RenderingIntent", &value);
				}
				else
				{
					// add gAMA chunk with gamma 1.0
					value.vt = VT_UI4;
					value.uintVal = 100000; // gama value * 100,000 -- i.e. gamma 1.0
					(void)metawriter->SetMetadataByName(L"/gAMA/ImageGamma", &value);

					// remove sRGB chunk which is added by default.
					(void)metawriter->RemoveMetadataByName(L"/sRGB/RenderingIntent");
				}
			}
#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
			else if (memcmp(&guidContainerFormat, &GUID_ContainerFormatJpeg, sizeof(GUID)) == 0)
			{
				// Set Software name
				(void)metawriter->SetMetadataByName(L"/app1/ifd/{ushort=305}", &value);

				if (sRGB)
				{
					// Set EXIF Colorspace of sRGB
					value.vt = VT_UI2;
					value.uiVal = 1;
					(void)metawriter->SetMetadataByName(L"/app1/ifd/exif/{ushort=40961}", &value);
				}
			}
			else if (memcmp(&guidContainerFormat, &GUID_ContainerFormatTiff, sizeof(GUID)) == 0)
			{
				// Set Software name
				(void)metawriter->SetMetadataByName(L"/ifd/{ushort=305}", &value);

				if (sRGB)
				{
					// Set EXIF Colorspace of sRGB
					value.vt = VT_UI2;
					value.uiVal = 1;
					(void)metawriter->SetMetadataByName(L"/ifd/exif/{ushort=40961}", &value);
				}
			}
#else
			else
			{
				// Set Software name
				(void)metawriter->SetMetadataByName(L"System.ApplicationName", &value);

				if (sRGB)
				{
					// Set EXIF Colorspace of sRGB
					value.vt = VT_UI2;
					value.uiVal = 1;
					(void)metawriter->SetMetadataByName(L"System.Image.ColorSpace", &value);
				}
			}
#endif
		}

		UINT64 imageSize = dstRowPitch * UINT64(textureHeight);
		if (imageSize > UINT32_MAX)
		{
			ASSERT_FAIL("Arithmetic Overflow!");
		}		

		if (memcmp(&targetGuid, &pfGuid, sizeof(WICPixelFormatGUID)) != 0)
		{
			// Conversion required to write
			Microsoft::WRL::ComPtr<IWICBitmap> source;
			DX_CALL(pWIC->CreateBitmapFromMemory(static_cast<UINT>(textureWidth), static_cast<UINT>(textureHeight),
				pfGuid,
				static_cast<UINT>(dstRowPitch), static_cast<UINT>(imageSize),
				const_cast<BYTE*>(texture.GetRawData()), source.GetAddressOf()));

			Microsoft::WRL::ComPtr<IWICFormatConverter> FC;
			DX_CALL(pWIC->CreateFormatConverter(FC.GetAddressOf()));

			BOOL canConvert = FALSE;
			DX_CALL(FC->CanConvert(pfGuid, targetGuid, &canConvert));
			if (!canConvert)
			{
				ASSERT_FAIL("Can not convert format!");
			}

			DX_CALL(FC->Initialize(source.Get(), targetGuid, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeMedianCut));

			WICRect rect = { 0, 0, static_cast<INT>(textureWidth), static_cast<INT>(textureHeight) };
			DX_CALL(frame->WriteSource(FC.Get(), &rect));
		}
		else
		{
			// No conversion required
			DX_CALL(frame->WritePixels(static_cast<UINT>(textureHeight), static_cast<UINT>(dstRowPitch), static_cast<UINT>(imageSize), const_cast<BYTE*>(texture.GetRawData())));
		}

		DX_CALL(frame->Commit());

		DX_CALL(encoder->Commit());

		delonfail.clear();
	}

	HRESULT LoadDDSTextureDataFromFile(
		const std::wstring& fileName,
		std::unique_ptr<uint8_t[]>& ddsData, 
		const DirectX::DDS_HEADER** header, 
		const uint8_t** bitData, 
		size_t* bitSize) noexcept
	{
		if (!header || !bitData || !bitSize)
		{
			return E_POINTER;
		}

		// open the file
		ScopedHandle hFile(safe_handle(CreateFile2(fileName.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			OPEN_EXISTING,
			nullptr)));

		if (!hFile)
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}

		// Get the file size
		FILE_STANDARD_INFO fileInfo;
		if (!GetFileInformationByHandleEx(hFile.get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}

		// File is too big for 32-bit allocation, so reject read
		if (fileInfo.EndOfFile.HighPart > 0)
		{
			return E_FAIL;
		}

		// Need at least enough data to fill the header and magic number to be a valid DDS
		if (fileInfo.EndOfFile.LowPart < (sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER)))
		{
			return E_FAIL;
		}

		// create enough space for the file data
		ddsData.reset(new (std::nothrow) uint8_t[fileInfo.EndOfFile.LowPart]);
		if (!ddsData)
		{
			return E_OUTOFMEMORY;
		}

		// read the data in
		DWORD BytesRead = 0;
		if (!ReadFile(hFile.get(),
			ddsData.get(),
			fileInfo.EndOfFile.LowPart,
			&BytesRead,
			nullptr
		))
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}

		if (BytesRead < fileInfo.EndOfFile.LowPart)
		{
			return E_FAIL;
		}

		// DDS files always start with the same magic number ("DDS ")
		auto dwMagicNumber = *reinterpret_cast<const uint32_t*>(ddsData.get());
		if (dwMagicNumber != DirectX::DDS_MAGIC)
		{
			return E_FAIL;
		}

		auto hdr = reinterpret_cast<const DirectX::DDS_HEADER*>(ddsData.get() + sizeof(uint32_t));

		// Verify header to validate DDS file
		if (hdr->size != sizeof(DirectX::DDS_HEADER) ||
			hdr->ddspf.size != sizeof(DirectX::DDS_PIXELFORMAT))
		{
			return E_FAIL;
		}

		// Check for DX10 extension
		bool bDXT10Header = false;
		if ((hdr->ddspf.flags & DDS_FOURCC) &&
			(MAKEFOURCC('D', 'X', '1', '0') == hdr->ddspf.fourCC))
		{
			// Must be long enough for both headers and magic value
			if (fileInfo.EndOfFile.LowPart < (sizeof(DirectX::DDS_HEADER) + sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER_DXT10)))
			{
				return E_FAIL;
			}

			bDXT10Header = true;
		}

		// setup the pointers in the process request
		*header = hdr;
		auto offset = sizeof(uint32_t) + sizeof(DirectX::DDS_HEADER)
			+ (bDXT10Header ? sizeof(DirectX::DDS_HEADER_DXT10) : 0u);
		*bitData = ddsData.get() + offset;
		*bitSize = fileInfo.EndOfFile.LowPart - offset;

		return S_OK;
	}
}