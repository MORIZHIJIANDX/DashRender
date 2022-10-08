#include "PCH.h"
#include "CommandQueue.h"
#include "GraphicsCore.h"
#include "DX12Helper.h"

namespace Dash
{
	FCommandList::FCommandList(D3D12_COMMAND_LIST_TYPE type)
		: mType(type)
	{
		DX_CALL(FGraphicsCore::Device->CreateCommandAllocator(type, IID_PPV_ARGS(&mCommandAllocator)));
		DX_CALL(FGraphicsCore::Device->CreateCommandList(0, type, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&mGraphicsCommandList)));

#ifdef DASH_DEBUG
		mCommandAllocator->SetName(L"CommandAllocator");
		mGraphicsCommandList->SetName(L"CommandList");
#endif // DASH_DEBUG

	}

	FCommandList::~FCommandList()
	{
		mGraphicsCommandList = nullptr;
		mCommandAllocator = nullptr;
	}

	void FCommandList::Reset()
	{
		DX_CALL(mCommandAllocator->Reset());
		DX_CALL(mGraphicsCommandList->Reset(mCommandAllocator.Get(), nullptr));
	}

	void FCommandList::Close()
	{
		DX_CALL(mGraphicsCommandList->Close());
	}

	FCommandListPool::FCommandListPool(D3D12_COMMAND_LIST_TYPE type)
		: mType(type)
	{
	}

	FCommandListPool::~FCommandListPool()
	{
		Destroy();
	}

	void FCommandListPool::Destroy()
	{
		std::lock_guard<std::mutex> lock(mMutex);

		if (mCommandListPool.size())
		{
			mCommandListPool.clear();
		}
	}

	FCommandList* FCommandListPool::RequestCommandList()
	{
		std::lock_guard<std::mutex> lock(mMutex);

		while (!mRetiredCommandLists.empty() && FGraphicsCore::CommandQueueManager->IsFenceCompleted(mRetiredCommandLists.front().first))
		{
			mAvailableCommandLists.push(mRetiredCommandLists.front().second);
			mRetiredCommandLists.pop();
		}

		FCommandList* commandList = nullptr;

		if (!mAvailableCommandLists.empty())
		{
			commandList = mAvailableCommandLists.front();
			mAvailableCommandLists.pop();

			commandList->Reset();
		}
		else
		{
			commandList = new FCommandList(mType);
			mCommandListPool.emplace_back(commandList);
		}

		return commandList;
	}

	void FCommandListPool::RetiredUsedCommandList(uint64_t fenceID, FCommandList* commandList)
	{
		std::lock_guard<std::mutex> lock(mMutex);
		mRetiredCommandLists.push(std::make_pair(fenceID, commandList));
	}

	FCommandQueue::FCommandQueue(D3D12_COMMAND_LIST_TYPE type)
	{
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
		commandQueueDesc.Type = type;
		commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		commandQueueDesc.NodeMask = 0;

		DX_CALL(FGraphicsCore::Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&mCommandQueue)));

		uint64_t initFenceValue = ((uint64_t)type) << COMMAND_TYPE_MASK;
		DX_CALL(FGraphicsCore::Device->CreateFence(initFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));

#ifdef DASH_DEBUG
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			{
				mCommandQueue->SetName(L"ComputeQueue");
				mFence->SetName(L"ComputeFence");
				break;
			}
		case D3D12_COMMAND_LIST_TYPE_COPY:
			{
				mCommandQueue->SetName(L"CopyQueue");
				mFence->SetName(L"CopyFence");
				break;
			}
		default:
			{
				mCommandQueue->SetName(L"GraphicsQueue");
				mFence->SetName(L"GraphicsFence");
				break;
			}
		}
#endif // DASH_DEBUG

		mNextFenceValue = initFenceValue + 1;
	}

	void FCommandQueue::Destroy()
	{
		if (mCommandQueue)
		{
			Flush();

			mCommandQueue = nullptr;
			mFence = nullptr;
		}
	}

	uint64_t FCommandQueue::ExecuteCommandList(FCommandList* commandList)
	{
		commandList->Close();

		ID3D12CommandList* d3dCommandLists[] = { commandList->GetCommandList() };
		mCommandQueue->ExecuteCommandLists(_countof(d3dCommandLists), d3dCommandLists);

		return Signal();
	}

	uint64_t FCommandQueue::ExecuteCommandLists(std::vector<FCommandList*> commandLists)
	{
		if (commandLists.empty())
		{
			mNextFenceValue;
		}

		std::vector<ID3D12CommandList*> d3dCommandLists;

		for (FCommandList* List : commandLists)
		{
			List->Close();
			d3dCommandLists.push_back(List->GetCommandList());
		}

		mCommandQueue->ExecuteCommandLists(static_cast<UINT>(d3dCommandLists.size()), d3dCommandLists.data());

		return Signal();
	}

	uint64_t FCommandQueue::Signal()
	{
		DX_CALL(mCommandQueue->Signal(mFence.Get(), mNextFenceValue));
		return mNextFenceValue++;
	}

	bool FCommandQueue::IsFenceCompleted(uint64_t fenceValue)
	{
		return fenceValue <= mFence->GetCompletedValue();
	}

	void FCommandQueue::WaitForFence(uint64_t fenceValue)
	{
		if (IsFenceCompleted(fenceValue))
		{
			return;
		}

		auto event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		if (event)
		{
			// Is this function thread safe?
			DX_CALL(mFence->SetEventOnCompletion(fenceValue, event));
			::WaitForSingleObject(event, DWORD_MAX);

			::CloseHandle(event);
		}
	}

	void FCommandQueue::WaitForCommandQueue(const FCommandQueue& queue)
	{
		DX_CALL(mCommandQueue->Wait(queue.mFence.Get(), queue.mNextFenceValue - 1));
	}

	void FCommandQueue::Flush()
	{
		WaitForFence(Signal());
	}

	void FCommandQueueManager::Destroy()
	{
		mGraphicsQueue.Destroy();
		mComputeQueue.Destroy();
		mCopyQueue.Destroy();
	}

	FCommandQueue& FCommandQueueManager::GetGraphicsQueue()
	{
		return mGraphicsQueue;
	}

	FCommandQueue& FCommandQueueManager::GetComputeQueue()
	{
		return mComputeQueue;
	}

	FCommandQueue& FCommandQueueManager::GetCopyQueue()
	{
		return mCopyQueue;
	}

	FCommandQueue& FCommandQueueManager::GetQueue(D3D12_COMMAND_LIST_TYPE type)
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			return mComputeQueue;
		case D3D12_COMMAND_LIST_TYPE_COPY:
			return mCopyQueue;
		default:
			return mGraphicsQueue;
		}
	}

	bool FCommandQueueManager::IsFenceCompleted(uint64_t fenceValue)
	{
		return GetQueue(D3D12_COMMAND_LIST_TYPE(fenceValue >> COMMAND_TYPE_MASK)).IsFenceCompleted(fenceValue);
	}

	void FCommandQueueManager::WaitForFence(uint64_t fenceValue)
	{
		GetQueue(D3D12_COMMAND_LIST_TYPE(fenceValue >> COMMAND_TYPE_MASK)).WaitForFence(fenceValue);
	}

	void FCommandQueueManager::Flush()
	{
		mGraphicsQueue.Flush();
		mComputeQueue.Flush();
		mCopyQueue.Flush();
	}

	void FCommandListManager::Destroy()
	{
		mGraphicsCommandListPool.Destroy();
		mComputeCommandListPool.Destroy();
		mCopyCommandListPool.Destroy();
	}

	FCommandListPool& FCommandListManager::GetGraphicsCommandListPool()
	{
		return mGraphicsCommandListPool;
	}

	FCommandListPool& FCommandListManager::GetComputeCommandListPool()
	{
		return mComputeCommandListPool;
	}

	FCommandListPool& FCommandListManager::GetCopyCommandListPool()
	{
		return mCopyCommandListPool;
	}

	FCommandListPool& FCommandListManager::GetCommandListPool(D3D12_COMMAND_LIST_TYPE type)
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			return mComputeCommandListPool;
		case D3D12_COMMAND_LIST_TYPE_COPY:
			return mCopyCommandListPool;
		default:
			return mGraphicsCommandListPool;
		}
	}

	FCommandList* FCommandListManager::RequestCommandList(D3D12_COMMAND_LIST_TYPE type)
	{
		return GetCommandListPool(type).RequestCommandList();
	}

	void FCommandListManager::RetiredUsedCommandList(uint64_t fenceID, FCommandList* commandList)
	{
		GetCommandListPool(commandList->GetType()).RetiredUsedCommandList(fenceID, commandList);
	}

}