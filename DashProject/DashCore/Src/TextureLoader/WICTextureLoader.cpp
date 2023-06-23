#include "PCH.h"
#include "WICTextureLoader.h"
#include <wincodec.h>
#include <wtypes.h>
#include <Unknwn.h>
#include <wrl.h>
#include "Utility/StringUtility.h"
#include "Graphics/DX12Helper.h"

using namespace Microsoft::WRL;

namespace Dash
{
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

	//---------------------------------------------------------------------------------
	size_t _WICBitsPerPixel(REFGUID targetGuid) noexcept
	{
		auto pWIC = _GetWIC();
		if (!pWIC)
			return 0;

		ComPtr<IWICComponentInfo> cinfo;
		if (FAILED(pWIC->CreateComponentInfo(targetGuid, cinfo.GetAddressOf())))
			return 0;

		WICComponentType type;
		if (FAILED(cinfo->GetComponentType(&type)))
			return 0;

		if (type != WICPixelFormat)
			return 0;

		ComPtr<IWICPixelFormatInfo> pfinfo;
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

	bool LoadWICTextureFromFile(const std::string& fileName,
		EWIC_LOADER_FLAGS loadFlags,
		FTextureBufferDescription& textureDescription,
		D3D12_SUBRESOURCE_DATA& subResource,
		std::vector<uint8_t>& decodedData)
	{
		auto pWIC = _GetWIC();
		if (!pWIC)
		{
			ASSERT_FAIL("Get WIC Factory Failed!");
		}

		std::wstring wFileName = FStringUtility::UTF8ToWideString(fileName);

		// Initialize WIC
		Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
		DX_CALL(pWIC->CreateDecoderFromFilename(wFileName.c_str(),
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
		if (EnumMaskContains(loadFlags, EWIC_LOADER_FLAGS::WIC_LOADER_FIT_POW2))
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

		if (EnumMaskContains(loadFlags, EWIC_LOADER_FLAGS::WIC_LOADER_MAKE_SQUARE))
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
				return;
			}
		}
		else
		{
			bpp = _WICBitsPerPixel(pixelFormat);
		}

		if (EnumMaskContains(loadFlags, EWIC_LOADER_FLAGS::WIC_LOADER_FORCE_RGBA32))
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
		if (EnumMaskContains(loadFlags, EWIC_LOADER_FLAGS::WIC_LOADER_FORCE_SRGB))
		{
			format = MakeSRGB(format);
		}
		else if (!EnumMaskContains(loadFlags, EWIC_LOADER_FLAGS::WIC_LOADER_IGNORE_SRGB))
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
							sRGB = EnumMaskContains(loadFlags, EWIC_LOADER_FLAGS::WIC_LOADER_SRGB_DEFAULT);
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
						sRGB = EnumMaskContains(loadFlags, EWIC_LOADER_FLAGS::WIC_LOADER_SRGB_DEFAULT);
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

		size_t rowPitch = static_cast<size_t>(rowBytes);
		size_t imageSize = static_cast<size_t>(numBytes);

		EResourceFormat dashFormat = ResourceFormatFromD3DFormat(format);
		ASSERT(dashFormat != EResourceFormat::Unknown);
		//FTexture texture{ twidth, theight, dashFormat };
		decodedData.resize(imageSize);

		// Load image data
		if (memcmp(&convertGUID, &pixelFormat, sizeof(GUID)) == 0
			&& twidth == width
			&& theight == height)
		{
			// No format conversion or resize needed
			DX_CALL(frame->CopyPixels(nullptr, static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize), decodedData.data()));
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
				DX_CALL(scaler->CopyPixels(nullptr, static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize), decodedData.data()));
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

				DX_CALL(FC->CopyPixels(nullptr, static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize), decodedData.data()));
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

			DX_CALL(FC->CopyPixels(nullptr, static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize), decodedData.data()));
		}

		subResource.pData = decodedData.data();
		subResource.RowPitch = static_cast<LONG>(rowPitch);
		subResource.SlicePitch = static_cast<LONG>(imageSize);

		// TextureInfo
		textureDescription = FTextureBufferDescription::Create2D(dashFormat, width, height);

		return true;
	}
}
