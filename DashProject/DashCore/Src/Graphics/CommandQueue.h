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

		D3D12_COMMAND_LIST_TYPE GetType() const;

		ID3D12GraphicsCommandList* GetGraphicsCommandList() { return mGraphicsCommandList.Get(); }
		const ID3D12GraphicsCommandList* GetGraphicsCommandList() const { return mGraphicsCommandList.Get(); }

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

		CommandList* RequestCommandList();

		void RetiredUsedCommandList(uint64_t fenceID, CommandList* commandList);

	private:
		D3D12_COMMAND_LIST_TYPE mType;

		std::mutex mMutex;

		std::vector<std::unique_ptr<CommandList>> mCommandListPool;
		std::queue<std::pair<uint64_t, CommandList*>> mRetiredCommandLists;
		std::queue<CommandList*> mAvailableCommandLists;
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
	private:
		uint64_t mNextFenceValue;

		Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
	};

	class CommandQueueManager
	{
	public:
		CommandQueueManager() {};
		~CommandQueueManager();

		void Init();
		void Destroy();

		CommandQueue& GetGraphicsQueue();
		CommandQueue& GetComputeQueue();
		CommandQueue& GetCopyQueue();

		CommandQueue& GetQueue(D3D12_COMMAND_LIST_TYPE type);

		bool IsFenceCompleted(uint64_t fenceValue);
		void WaitForFence(uint64_t fenceValue);

		void Flush();
	private:
		std::unique_ptr<CommandQueue> mGraphicsQueue;
		std::unique_ptr<CommandQueue> mComputeQueue;
		std::unique_ptr<CommandQueue> mCopyQueue;
	};
}