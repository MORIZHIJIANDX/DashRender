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
		virtual void CreateViews() {};
	};
}