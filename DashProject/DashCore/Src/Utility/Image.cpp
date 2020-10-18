#include "PCH.h"

#include "Image.h"
#include "ImageHelper.h"

namespace Dash
{
	FTexture::FTexture()
		: mWidth(0)
		, mHeight(0)
		, mFormat(EDASH_FORMAT::UnKwon)
		, mBitPerPixel(0)
		, mRowAlignment(1)
	{
	}

	FTexture::FTexture(size_t width, size_t height, EDASH_FORMAT format, size_t alignment)
		: mWidth(width)
		, mHeight(height)
		, mFormat(format)
		, mBitPerPixel(GetByteSizeForFormat(format) * 8)
		, mRowAlignment(alignment)
	{
		ASSERT(mRowAlignment >= 1);

		//扩展为1字节(8 bit)的倍数
		size_t picRowPitch = (size_t(mWidth) * size_t(mBitPerPixel) + (mRowAlignment * 8) - 1) / (mRowAlignment * 8);

		mData.resize(picRowPitch * height);
	}

	FTexture::FTexture(const FTexture& other)
		: mWidth(other.mWidth)
		, mHeight(other.mHeight)
		, mFormat(other.mFormat)
		, mBitPerPixel(other.mBitPerPixel)
		, mRowAlignment(other.mRowAlignment)
		, mData(other.mData)
	{
	}

	FTexture::FTexture(FTexture&& other) noexcept
		: mWidth(std::exchange(other.mWidth, 0))
		, mHeight(std::exchange(other.mHeight, 0))
		, mFormat(std::exchange(other.mFormat, EDASH_FORMAT::UnKwon))
		, mBitPerPixel(std::exchange(other.mBitPerPixel, 0))
		, mRowAlignment(std::exchange(other.mRowAlignment, 1))
		, mData(std::move(other.mData))
	{
	}

	FTexture& FTexture::operator=(const FTexture& other) noexcept
	{
		if (this != &other)
		{
			mWidth = other.mWidth;
			mHeight = other.mHeight;
			mFormat = other.mFormat;
			mBitPerPixel = other.mBitPerPixel;
			mRowAlignment = other.mRowAlignment;
			mData = other.mData;
		}
		return *this;
	}

	FTexture& FTexture::operator=(FTexture&& other) noexcept
	{
		if (this != &other)
		{
			mWidth = std::exchange(other.mWidth, 0);
			mHeight = std::exchange(other.mHeight, 0);
			mFormat = std::exchange(other.mFormat, EDASH_FORMAT::UnKwon);
			mBitPerPixel = std::exchange(other.mBitPerPixel, 0);
			mRowAlignment = std::exchange(other.mRowAlignment, 1);
			mData = std::move(other.mData);
		}
		return *this;
	}

	DXGI_FORMAT FTexture::GetDXGIFormat() const
	{
		return DashFormatToDXGIFormat(mFormat);
	}

	void FTexture::Resize(size_t x, size_t y)
	{
		mWidth = x;
		mHeight = y;

		size_t picRowPitch = (size_t(mWidth) * size_t(mBitPerPixel) + (mRowAlignment * 8) - 1) / (mRowAlignment * 8);
		mData.resize(picRowPitch * mHeight);
	}

	void FTexture::Resize(const FVector2i& size)
	{
		Resize(size.x, size.y);
	}

}