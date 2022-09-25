#pragma once

#include "d3dx12.h"
#include <wrl.h>

namespace Dash
{
	#define	COMMAND_TYPE_MASK 56

	class CommandList
	{
	public:
		CommandList(D3D12_COMMAND_LIST_TYPE type);
		~CommandList();

		void Reset();
		void Close();

		D3D12_COMMAND_LIST_TYPE GetType() const 
		{
			return mType;
		}

		ID3D12GraphicsCommandList* GetD3DCommandList() { return mGraphicsCommandList.Get(); }
		const ID3D12GraphicsCommandList* GetD3DCommandList() const { return mGraphicsCommandList.Get(); }

	private:
		D3D12_COMMAND_LIST_TYPE mType;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mGraphicsCommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;
	};

	class CommandListPool
	{
	public:
		CommandListPool(D3D12_COMMAND_LIST_TYPE type);
		~CommandListPool();

		void Destroy();

		D3D12_COMMAND_LIST_TYPE GetType() const
		{
			return mType;
		}

		CommandList* RequestCommandList();

		void RetiredUsedCommandList(uint64_t fenceID, CommandList* commandList);

	private:
		D3D12_COMMAND_LIST_TYPE mType;

		std::mutex mMutex;

		std::vector<std::unique_ptr<CommandList>> mCommandListPool;
		std::queue<std::pair<uint64_t, CommandList*>> mRetiredCommandLists;
		std::queue<CommandList*> mAvailableCommandLists;
	};

	class CommandListManager
	{
	public:
		CommandListManager()
			: mGraphicsCommandListPool(D3D12_COMMAND_LIST_TYPE_DIRECT)
			, mComputeCommandListPool(D3D12_COMMAND_LIST_TYPE_COMPUTE)
			, mCopyCommandListPool(D3D12_COMMAND_LIST_TYPE_COPY)
		{
		}

		~CommandListManager(){}

		void Destroy();

		CommandListPool& GetGraphicsCommandListPool();
		CommandListPool& GetComputeCommandListPool();
		CommandListPool& GetCopyCommandListPool();

		CommandListPool& GetCommandListPool(D3D12_COMMAND_LIST_TYPE type);

	private:
		CommandListPool mGraphicsCommandListPool;
		CommandListPool mComputeCommandListPool;
		CommandListPool mCopyCommandListPool;
	};

	class CommandQueue
	{
	public:
		CommandQueue(D3D12_COMMAND_LIST_TYPE type);
		~CommandQueue() { Destroy(); };

		void Destroy();

		uint64_t ExecuteCommandList(CommandList* commandList);
		uint64_t ExecuteCommandLists(std::vector<CommandList*> commandLists);
		uint64_t Signal();

		bool IsFenceCompleted(uint64_t fenceValue);

		void WaitForFence(uint64_t fenceValue);
		void WaitForCommandQueue(const CommandQueue& queue);

		void Flush();

		ID3D12CommandQueue* GetD3DCommandQueue() { return mCommandQueue.Get(); }
		const ID3D12CommandQueue* GetD3DCommandQueue() const { return mCommandQueue.Get(); }
	private:
		uint64_t mNextFenceValue;

		Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
	};

	class CommandQueueManager
	{
	public:
		CommandQueueManager()
			: mGraphicsQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)
			, mComputeQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE)
			, mCopyQueue(D3D12_COMMAND_LIST_TYPE_COPY)
		{
		}

		~CommandQueueManager(){}

		void Destroy();

		CommandQueue& GetGraphicsQueue();
		CommandQueue& GetComputeQueue();
		CommandQueue& GetCopyQueue();

		CommandQueue& GetQueue(D3D12_COMMAND_LIST_TYPE type);

		bool IsFenceCompleted(uint64_t fenceValue);
		void WaitForFence(uint64_t fenceValue);

		void Flush();
	private:
		CommandQueue mGraphicsQueue;
		CommandQueue mComputeQueue;
		CommandQueue mCopyQueue;
	};
}