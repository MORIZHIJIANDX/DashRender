#include "PCH.h"
#include "CommandListPool.h"
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
		DX_CALL(mGraphicsCommandList->Reset(mCommandAllocator.Get(), nullptr));
	}

	D3D12_COMMAND_LIST_TYPE CommandList::GetType() const
	{
		return mType;
	}

	CommandListPool::CommandListPool()
	{
	}

	CommandListPool::~CommandListPool()
	{
		Destroy();
	}

	void CommandListPool::Destroy()
	{
		if (mCommandListPool.size())
		{
			mCommandListPool.clear();
		}
	}

	CommandList* CommandListPool::RequestCommandList()
	{
		return nullptr;
	}

}