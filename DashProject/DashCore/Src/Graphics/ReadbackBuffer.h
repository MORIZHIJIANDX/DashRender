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
		void CreateReadbackBuffer(const std::string& name, uint32 numElements, uint32 elementSize);
		virtual void CreateViews() {};

	protected:
		void* mMappedData = nullptr;
	};
}