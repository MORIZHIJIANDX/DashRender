#pragma once

#include "d3dx12.h"
#include <wrl.h>

namespace Dash
{
	#define	COMMAND_TYPE_MASK 56

	class FCommandList
	{
	public:
		FCommandList(D3D12_COMMAND_LIST_TYPE type);
		~FCommandList();

		void Reset();
		void Close();

		D3D12_COMMAND_LIST_TYPE GetType() const 
		{
			return mType;
		}

		ID3D12GraphicsCommandList* GetCommandList() { return mGraphicsCommandList.Get(); }
		const ID3D12GraphicsCommandList* GetCommandList() const { return mGraphicsCommandList.Get(); }

	private:
		D3D12_COMMAND_LIST_TYPE mType;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mGraphicsCommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;
	};

	class FCommandListPool
	{
	public:
		FCommandListPool(D3D12_COMMAND_LIST_TYPE type);
		~FCommandListPool();

		void Destroy();

		D3D12_COMMAND_LIST_TYPE GetType() const
		{
			return mType;
		}

		FCommandList* RequestCommandList();
		void RetiredUsedCommandList(uint64_t fenceID, FCommandList* commandList);

	private:
		D3D12_COMMAND_LIST_TYPE mType;

		std::mutex mMutex;

		std::vector<std::unique_ptr<FCommandList>> mCommandListPool;
		std::queue<std::pair<uint64_t, FCommandList*>> mRetiredCommandLists;
		std::queue<FCommandList*> mAvailableCommandLists;
	};

	class FCommandListManager
	{
	public:
		FCommandListManager()
			: mGraphicsCommandListPool(D3D12_COMMAND_LIST_TYPE_DIRECT)
			, mComputeCommandListPool(D3D12_COMMAND_LIST_TYPE_COMPUTE)
			, mCopyCommandListPool(D3D12_COMMAND_LIST_TYPE_COPY)
		{
		}

		~FCommandListManager(){}

		void Destroy();

		FCommandListPool& GetGraphicsCommandListPool();
		FCommandListPool& GetComputeCommandListPool();
		FCommandListPool& GetCopyCommandListPool();

		FCommandListPool& GetCommandListPool(D3D12_COMMAND_LIST_TYPE type);

		FCommandList* RequestCommandList(D3D12_COMMAND_LIST_TYPE type);
		void RetiredUsedCommandList(uint64_t fenceID, FCommandList* commandList);

	private:
		FCommandListPool mGraphicsCommandListPool;
		FCommandListPool mComputeCommandListPool;
		FCommandListPool mCopyCommandListPool;
	};

	class FCommandQueue
	{
	public:
		FCommandQueue(D3D12_COMMAND_LIST_TYPE type);
		~FCommandQueue() { Destroy(); };

		void Destroy();

		uint64_t ExecuteCommandList(FCommandList* commandList);
		uint64_t ExecuteCommandLists(std::vector<FCommandList*> commandLists);
		uint64_t Signal();

		bool IsFenceCompleted(uint64_t fenceValue);

		void WaitForFence(uint64_t fenceValue);
		void WaitForCommandQueue(const FCommandQueue& queue);

		void Flush();

		ID3D12CommandQueue* GetD3DCommandQueue() { return mCommandQueue.Get(); }
		const ID3D12CommandQueue* GetD3DCommandQueue() const { return mCommandQueue.Get(); }
	private:
		uint64_t mNextFenceValue;

		Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
	};

	class FCommandQueueManager
	{
	public:
		FCommandQueueManager()
			: mGraphicsQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)
			, mComputeQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE)
			, mCopyQueue(D3D12_COMMAND_LIST_TYPE_COPY)
		{
		}

		~FCommandQueueManager(){}

		void Destroy();

		FCommandQueue& GetGraphicsQueue();
		FCommandQueue& GetComputeQueue();
		FCommandQueue& GetCopyQueue();

		FCommandQueue& GetQueue(D3D12_COMMAND_LIST_TYPE type);

		bool IsFenceCompleted(uint64_t fenceValue);
		void WaitForFence(uint64_t fenceValue);

		void Flush();
	private:
		FCommandQueue mGraphicsQueue;
		FCommandQueue mComputeQueue;
		FCommandQueue mCopyQueue;
	};
}