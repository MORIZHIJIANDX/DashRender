#include "PCH.h"
#include "GPUProfiler.h"
#include "QueryHeap.h"
#include "GraphicsCore.h"
#include "RenderDevice.h"
#include "ReadbackBuffer.h"
#include "SwapChain.h"

namespace Dash
{
	static constexpr uint64_t MaxProfiles = 128;

	struct FProfileData
	{
		bool Started = false;
		bool Finished = false;
	};

	std::array<FProfileData, MaxProfiles> ProfileData;
	std::map<std::string, uint32_t> NameToProfileIndex;

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
		mQueryHeap = std::make_shared<FQueryHeap>(heapDesc);

		mReadbackBuffer = FGraphicsCore::Device->CreateReadbackBuffer("QueryReadbackBuffer", MaxProfiles * 2 * FGraphicsCore::BackBufferCount, sizeof(uint64_t));
	}

	void FGPUProfiler::Destroy()
	{
		mQueryHeap.reset();
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

	uint32_t FGPUProfiler::StartProfile(FCopyCommandContextBase& contex, const std::string& name)
	{
		ASSERT(!NameToProfileIndex.contains(name));

		int32_t profileIndex = mProfileCounter++;
		ASSERT(profileIndex < MaxProfiles);

		NameToProfileIndex[name] = profileIndex;
		ProfileData[profileIndex].Started = true;
		ASSERT(ProfileData[profileIndex].Finished == false);

		int32_t beginQueryIndex = profileIndex * 2;
		contex.EndQuery(mQueryHeap, beginQueryIndex);

		return profileIndex;
	}

	void FGPUProfiler::EndProfile(FCopyCommandContextBase& contex, const std::string& name)
	{
		ASSERT(NameToProfileIndex.contains(name));

		int32_t profileIndex = NameToProfileIndex[name];
		ASSERT(profileIndex < MaxProfiles);

		ProfileData[profileIndex].Finished = true;
		ASSERT(ProfileData[profileIndex].Started == true);

		int32_t beginQueryIndex = profileIndex * 2;
		int32_t endQueryIndex = beginQueryIndex + 1;
		contex.EndQuery(mQueryHeap, endQueryIndex);

		int32_t currentBackBufferIndex = FGraphicsCore::SwapChain->GetCurrentBackBufferIndex();
		int32_t dataOffset = (MaxProfiles * 2 * currentBackBufferIndex + beginQueryIndex) * sizeof(uint64_t);
		contex.ResolveQueryData(mQueryHeap, beginQueryIndex, 2, mReadbackBuffer, dataOffset);
	}

	std::vector<FGPUProfiler::FProfileResult> FGPUProfiler::GetQueryResults() const
	{
		uint64_t gpuFrequency = 0;
		FGraphicsCore::CommandQueueManager->GetGraphicsQueue().GetD3DCommandQueue()->GetTimestampFrequency(&gpuFrequency);
		float frequency = static_cast<float>(gpuFrequency);

		int32_t currentBackBufferIndex = FGraphicsCore::SwapChain->GetCurrentBackBufferIndex();
		int32_t currentFrameQueryDataOffset = MaxProfiles * 2 * currentBackBufferIndex;

		const uint64_t* queryData = static_cast<uint64_t*>(mReadbackBuffer->Map()) + currentFrameQueryDataOffset;

		std::vector<FProfileResult> results;

		for (auto const& [name, index] : NameToProfileIndex)
		{
			ASSERT(index < MaxProfiles);
			FProfileData& data = ProfileData[index];
			if (data.Started && data.Finished)
			{
				uint64_t startTime = queryData[index * 2];
				uint64_t endTime = queryData[index * 2 + 1];

				uint64_t delta = endTime - startTime;
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