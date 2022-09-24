#include "PCH.h"
#include "CommandQueue.h"
#include "GraphicsCore.h"
#include "../Utility/Exception.h"

namespace Dash
{
	CommandList::CommandList(D3D12_COMMAND_LIST_TYPE type)
		: mType(type)
	{
		DX_CALL(Graphics::Device->CreateCommandAllocator(type, IID_PPV_ARGS(&mCommandAllocator)));
		DX_CALL(Graphics::Device->CreateCommandList(0, type, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&mGraphicsCommandList)));
	}

	CommandList::~CommandList()
	{
		mGraphicsCommandList = nullptr;
		mCommandAllocator = nullptr;
	}

	void CommandList::Reset()
	{
		DX_CALL(mCommandAllocator->Reset());
		DX_CALL(mGraphicsCommandList->Reset(mCommandAllocator.Get(), nullptr));
	}

	void CommandList::Close()
	{
		DX_CALL(mGraphicsCommandList->Close());
	}

	D3D12_COMMAND_LIST_TYPE CommandList::GetType() const
	{
		return mType;
	}

	CommandListPool::CommandListPool(D3D12_COMMAND_LIST_TYPE type)
		: mType(type)
	{
	}

	CommandListPool::~CommandListPool()
	{
		Destroy();
	}

	void CommandListPool::Destroy()
	{
		std::lock_guard<std::mutex> lock(mMutex);

		if (mCommandListPool.size())
		{
			mCommandListPool.clear();
		}
	}

	CommandList* CommandListPool::RequestCommandList()
	{
		std::lock_guard<std::mutex> lock(mMutex);

		while (!mRetiredCommandLists.empty() && Graphics::QueueManager->IsFenceCompleted(mRetiredCommandLists.front().first))
		{
			mAvailableCommandLists.push(mRetiredCommandLists.front().second);
			mRetiredCommandLists.pop();
		}

		CommandList* commandList = nullptr;

		if (!mAvailableCommandLists.empty())
		{
			commandList = mAvailableCommandLists.front();
			mAvailableCommandLists.pop();

			commandList->Reset();
		}
		else
		{
			commandList = new CommandList(mType);
			mCommandListPool.emplace_back(commandList);
		}

		return commandList;
	}

	void CommandListPool::RetiredUsedCommandList(uint64_t fenceID, CommandList* commandList)
	{
		std::lock_guard<std::mutex> lock(mMutex);
		mRetiredCommandLists.push(std::make_pair(fenceID, commandList));
	}

	CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE type)
	{
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
		commandQueueDesc.Type = type;
		commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		commandQueueDesc.NodeMask = 0;

		DX_CALL(Graphics::Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&mCommandQueue)));

		uint64_t initFenceValue = ((uint64_t)type) << COMMAND_TYPE_MASK;
		DX_CALL(Graphics::Device->CreateFence(initFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));

		mNextFenceValue = initFenceValue + 1;
	}

	void CommandQueue::Destroy()
	{
		if (mCommandQueue)
		{
			Flush();

			mCommandQueue = nullptr;
			mFence = nullptr;
		}
	}

	uint64_t CommandQueue::ExecuteCommandList(CommandList* commandList)
	{
		commandList->Close();

		ID3D12CommandList* d3dCommandLists[] = { commandList->GetGraphicsCommandList() };
		mCommandQueue->ExecuteCommandLists(_countof(d3dCommandLists), d3dCommandLists);

		return Signal();
	}

	uint64_t CommandQueue::ExecuteCommandLists(std::vector<CommandList*> commandLists)
	{
		if (commandLists.empty())
		{
			mNextFenceValue;
		}

		std::vector<ID3D12CommandList*> d3dCommandLists;

		for (CommandList* List : commandLists)
		{
			List->Close();
			d3dCommandLists.push_back(List->GetGraphicsCommandList());
		}

		mCommandQueue->ExecuteCommandLists(d3dCommandLists.size(), d3dCommandLists.data());

		return Signal();
	}

	uint64_t CommandQueue::Signal()
	{
		DX_CALL(mCommandQueue->Signal(mFence.Get(), mNextFenceValue));
		return mNextFenceValue++;
	}

	bool CommandQueue::IsFenceCompleted(uint64_t fenceValue)
	{
		return fenceValue <= mFence->GetCompletedValue();
	}

	void CommandQueue::WaitForFence(uint64_t fenceValue)
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

	void CommandQueue::WaitForCommandQueue(const CommandQueue& queue)
	{
		DX_CALL(mCommandQueue->Wait(queue.mFence.Get(), queue.mNextFenceValue - 1));
	}

	void CommandQueue::Flush()
	{
		WaitForFence(Signal());
	}

	CommandQueueManager::~CommandQueueManager()
	{
		Destroy();
	}

	void CommandQueueManager::Init()
	{
		mGraphicsQueue = std::make_unique<CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT);
		mComputeQueue = std::make_unique<CommandQueue>(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		mCopyQueue = std::make_unique<CommandQueue>(D3D12_COMMAND_LIST_TYPE_COPY);
	}

	void CommandQueueManager::Destroy()
	{
		if (mGraphicsQueue)
		{
			mGraphicsQueue->Destroy();
			mComputeQueue->Destroy();
			mCopyQueue->Destroy();

			mGraphicsQueue.reset();
			mComputeQueue.reset();
			mCopyQueue.reset();
		}
	}

	CommandQueue& CommandQueueManager::GetGraphicsQueue()
	{
		return *mGraphicsQueue;
	}

	CommandQueue& CommandQueueManager::GetComputeQueue()
	{
		return *mComputeQueue;
	}

	CommandQueue& CommandQueueManager::GetCopyQueue()
	{
		return *mCopyQueue;
	}

	CommandQueue& CommandQueueManager::GetQueue(D3D12_COMMAND_LIST_TYPE type)
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			return *mComputeQueue;
		case D3D12_COMMAND_LIST_TYPE_COPY:
			return *mCopyQueue;
		default:
			return *mGraphicsQueue;
		}
	}

	bool CommandQueueManager::IsFenceCompleted(uint64_t fenceValue)
	{
		return GetQueue(D3D12_COMMAND_LIST_TYPE(fenceValue >> COMMAND_TYPE_MASK)).IsFenceCompleted(fenceValue);
	}

	void CommandQueueManager::WaitForFence(uint64_t fenceValue)
	{
		GetQueue(D3D12_COMMAND_LIST_TYPE(fenceValue >> COMMAND_TYPE_MASK)).WaitForFence(fenceValue);
	}

	void CommandQueueManager::Flush()
	{
		mGraphicsQueue->Flush();
		mComputeQueue->Flush();
		mCopyQueue->Flush();
	}

}