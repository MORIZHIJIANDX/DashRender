#include "PCH.h"
#include "GPUProfiler.h"
#include "QueryHeap.h"
#include "GraphicsCore.h"
#include "RenderDevice.h"
#include "ReadbackBuffer.h"
#include "SwapChain.h"

namespace Dash
{
	static constexpr uint64 MaxProfiles = 128;

	struct FProfileData
	{
		bool Started = false;
		bool Finished = false;
	};

	std::array<FProfileData, MaxProfiles> ProfileData;
	std::map<std::string, uint32> NameToProfileIndex;

	FGPUProfiler::FGPUProfiler()
	{
		Init();
	}

	FGPUProfiler::~FGPUProfiler()
	{
		Destroy();
	}

	void FGPUProfiler::Init()
	{
		FQueryHeapDesc heapDesc;
		heapDesc.count = MaxProfiles * 2;
		heapDesc.type = EGpuQueryType::Timestamp;
		mQueryHeap = MakeRefCounted<FQueryHeap>(heapDesc);

		mReadbackBuffer = FGraphicsCore::Device->CreateReadbackBuffer("QueryReadbackBuffer", MaxProfiles * 2 * FGraphicsCore::BackBufferCount, sizeof(uint64));
	}

	void FGPUProfiler::Destroy()
	{
		mQueryHeap.SafeRelease();
		mReadbackBuffer = nullptr;
	}

	void FGPUProfiler::NewFrame()
	{
		NameToProfileIndex.clear();
		for (size_t i = 0; i < MaxProfiles; i++)
		{
			ProfileData[i].Started = false;
			ProfileData[i].Finished = false;
		}

		mProfileCounter = 0;
	}

	uint32 FGPUProfiler::StartProfile(FCopyCommandContextBase& contex, const std::string& name)
	{
		ASSERT(!NameToProfileIndex.contains(name));

		int32 profileIndex = mProfileCounter++;
		ASSERT(profileIndex < MaxProfiles);

		NameToProfileIndex[name] = profileIndex;
		ProfileData[profileIndex].Started = true;
		ASSERT(ProfileData[profileIndex].Finished == false);

		int32 beginQueryIndex = profileIndex * 2;
		contex.EndQuery(mQueryHeap, beginQueryIndex);

		return profileIndex;
	}

	void FGPUProfiler::EndProfile(FCopyCommandContextBase& contex, const std::string& name)
	{
		ASSERT(NameToProfileIndex.contains(name));

		int32 profileIndex = NameToProfileIndex[name];
		ASSERT(profileIndex < MaxProfiles);

		ProfileData[profileIndex].Finished = true;
		ASSERT(ProfileData[profileIndex].Started == true);

		int32 beginQueryIndex = profileIndex * 2;
		int32 endQueryIndex = beginQueryIndex + 1;
		contex.EndQuery(mQueryHeap, endQueryIndex);

		int32 currentBackBufferIndex = FGraphicsCore::SwapChain->GetCurrentBackBufferIndex();
		int32 dataOffset = (MaxProfiles * 2 * currentBackBufferIndex + beginQueryIndex) * sizeof(uint64);
		contex.ResolveQueryData(mQueryHeap, beginQueryIndex, 2, mReadbackBuffer, dataOffset);
	}

	std::vector<FGPUProfiler::FProfileResult> FGPUProfiler::GetQueryResults() const
	{
		uint64 gpuFrequency = 0;
		FGraphicsCore::CommandQueueManager->GetGraphicsQueue().GetD3DCommandQueue()->GetTimestampFrequency(&gpuFrequency);
		float frequency = static_cast<float>(gpuFrequency);

		int32 currentBackBufferIndex = FGraphicsCore::SwapChain->GetCurrentBackBufferIndex();
		int32 currentFrameQueryDataOffset = MaxProfiles * 2 * currentBackBufferIndex;

		const uint64* queryData = static_cast<uint64*>(mReadbackBuffer->Map()) + currentFrameQueryDataOffset;

		std::vector<FProfileResult> results;

		for (auto const& [name, index] : NameToProfileIndex)
		{
			ASSERT(index < MaxProfiles);
			FProfileData& data = ProfileData[index];
			if (data.Started && data.Finished)
			{
				uint64 startTime = queryData[index * 2];
				uint64 endTime = queryData[index * 2 + 1];

				uint64 delta = endTime - startTime;
				float timeMs = delta / frequency * 1000.0f;
				results.emplace_back(timeMs, name);
			}
		}

		return results;
	}

	FGPUProfilerScope::FGPUProfilerScope(FCopyCommandContextBase& contex, const std::string& name)
		: mContex(contex)
		, mName(name)
	{
		FGraphicsCore::Profiler->StartProfile(mContex, mName);
	}

	FGPUProfilerScope::~FGPUProfilerScope()
	{
		FGraphicsCore::Profiler->EndProfile(mContex, mName);
	}
}