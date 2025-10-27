#pragma once

#include "GpuBuffer.h"

namespace Dash
{
	class FReadbackBuffer : public FGpuBuffer
	{
		friend class FRenderDevice;
	public:
		virtual ~FReadbackBuffer();

		void* Map();
		void Unmap();

	protected:
		FReadbackBuffer();

		void CreateReadbackBuffer(const std::string& name, uint32 numElements, uint32 elementSize);
		virtual void CreateViews() {};

	protected:
		void* mMappedData = nullptr;
	};
}