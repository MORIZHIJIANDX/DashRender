#pragma once

#include "CommandContext.h"

namespace Dash
{
	class FQueryHeap;

	class FGPUProfiler
	{
	public:
		struct FProfileResult
		{
			float TimeInMs;
			std::string Name;
		};

	public:

		FGPUProfiler();
		~FGPUProfiler();

		void Init();
		void Destroy();

		void NewFrame();

		uint32_t StartProfile(FCopyCommandContextBase& contex, const std::string& name);
		void EndProfile(FCopyCommandContextBase& contex, const std::string& name);

		std::vector<FProfileResult> GetQueryResults() const;
	
	private:
		FQueryHeapRef mQueryHeap;
		FReadbackBufferRef mReadbackBuffer;

		int32_t mProfileCounter = 0;
	};
}