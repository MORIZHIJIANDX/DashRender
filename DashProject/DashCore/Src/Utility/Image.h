#pragma once

#include <d3d12.h>
#include <fstream>

namespace Dash
{
	class FTexture
	{
	public:
		FTexture();
		FTexture(size_t width, size_t height, EDASH_FORMAT format, size_t rowAlignment = 1);	
		FTexture(const FTexture& other);
		FTexture(FTexture&& other) noexcept;
		~FTexture() {};

		FTexture& operator=(const FTexture& other) noexcept;
		FTexture& operator=(FTexture&& other) noexcept;

		EDASH_FORMAT GetFormat() const { return mFormat; }

		DXGI_FORMAT GetDXGIFormat() const;

		size_t GetBitPerPixel() const { return mBitPerPixel; }

		size_t GetRowPitch() const { return (size_t(mWidth) * size_t(mBitPerPixel) + (mRowAlignment * 8) - 1) / (mRowAlignment * 8); }

		size_t GetRowAlignment() const { return mRowAlignment; }

		size_t GetWidth() const { return mWidth; }

		size_t GetHeight() const { return mHeight; }

		const uint8_t* GetRawData() const { return mByteArray.data(); }

		uint8_t* GetRawData() { return mByteArray.data(); }

		template<typename T>
		void SetPixel(const T& value, size_t x, size_t y);

		template<typename T>
		void SetPixel(const T& value, const FVector2i& index);

		template<typename T>
		const T& GetPixel(size_t x, size_t y) const;

		template<typename T>
		const T& GetPixel(const FVector2i& index) const;

		template<typename T>
		void ClearImage(const T& value = T{ FZero{} });

		void Resize(size_t x, size_t y);

		void Resize(const FVector2i& size);

	private:
		size_t mWidth;
		size_t mHeight;
		
		// Row Alignment in Byte
		size_t mRowAlignment;
		size_t mBitPerPixel;
		EDASH_FORMAT mFormat;
		std::vector<uint8_t> mByteArray;
	};

	template<typename T>
	FORCEINLINE void FTexture::SetPixel(const T& value, size_t x, size_t y)
	{
		ASSERT(x < mWidth&& y < mHeight);
		reinterpret_cast<T&>(mByteArray[x + y * mWidth]) = value;
	}

	template<typename T>
	FORCEINLINE void FTexture::SetPixel(const T& value, const FVector2i& index)
	{
		SetPixel(value, index.x, index.y);
	}

	template<typename T>
	FORCEINLINE const T& FTexture::GetPixel(size_t x, size_t y) const
	{
		ASSERT(x < mWidth&& y < mHeight);

		ASSERT(mFormat == GetFormatForType<T>());

		return reinterpret_cast<const T&>(mByteArray[x + y * mWidth]);
	}

	template<typename T>
	FORCEINLINE const T& FTexture::GetPixel(const FVector2i& index) const
	{
		GetPixel(index.x, index.y);
	}

	template<typename T>
	FORCEINLINE void FTexture::ClearImage(const T& value)
	{
		for (size_t h = 0; h < mHeight; h++)
		{
			for (size_t w = 0; w < mWidth; w++)
			{
				SetPixel(value, w, h);
			}
		}
	}
}