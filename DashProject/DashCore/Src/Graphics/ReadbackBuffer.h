#pragma once

#include "GpuBuffer.h"

namespace Dash
{
	class FReadbackBuffer : public FGpuBuffer
	{
	public:
		FReadbackBuffer();
		virtual ~FReadbackBuffer();

		void* Map();
		void Unmap();

	protected:
		void CreateReadbackBuffer(const std::string& name, uint32_t numElements, uint32_t elementSize);

	protected:
		void* mMappedData = nullptr;
	};
}