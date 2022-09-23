#pragma once

#include "d3dx12.h"
#include <wrl.h>

namespace Dash
{
	class CommandList
	{
	public:
		CommandList(D3D12_COMMAND_LIST_TYPE type);
		~CommandList();

		void Reset();

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
		CommandListPool();
		~CommandListPool();

		void Destroy();

		CommandList* RequestCommandList();

	private:
		std::vector<std::unique_ptr<CommandList>> mCommandListPool;
		std::queue<std::pair<uint64_t, CommandList*>> mRetiredCommandLists;
		std::queue<CommandList*> mAvailableCommandLists;
	};
}