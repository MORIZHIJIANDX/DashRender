#pragma once

namespace Dash
{
	class FCpuDescriptorAllocatorPage;

	class FCpuDescriptorAllocation
	{
	public:
		// Creates a NULL descriptor.
		FCpuDescriptorAllocation();

		FCpuDescriptorAllocation(D3D12_CPU_DESCRIPTOR_HANDLE descriptor, uint32_t numDecriptors, uint32_t descriptorSize, std::shared_ptr<FCpuDescriptorAllocatorPage> page);

		// The destructor will automatically free the allocation.
		~FCpuDescriptorAllocation();

		// Copies are not allowed.
		FCpuDescriptorAllocation(const FCpuDescriptorAllocation&) = delete;
		FCpuDescriptorAllocation operator=(const FCpuDescriptorAllocation&) = delete;

		// Move is allowed.
		FCpuDescriptorAllocation(FCpuDescriptorAllocation&& allocation) noexcept;
		FCpuDescriptorAllocation& operator=(FCpuDescriptorAllocation&& other) noexcept;

		// Check if this a valid descriptor.
		bool IsNull() const;
		bool IsValid() const
		{
			return !IsNull();
		}

		// Get a descriptor at a particular offset in the allocation.
		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(uint32_t offset = 0) const;
		
		// Get the number of (consecutive) handles for this allocation.
		uint32_t GetNumDescriptors() const;

		// Get the descriptor type
		D3D12_DESCRIPTOR_HEAP_TYPE GetDescriptorType() const;

	private:
		// Free the descriptor back to the heap it came from.
		void Free();

		// The base descriptor.
		D3D12_CPU_DESCRIPTOR_HANDLE mDescriptorHandle;

		// The number of descriptors in this allocation.
		uint32_t mNumDescriptors;

		// The offset to the next descriptor.
		uint32_t mDescriptorSize;

		// A pointer back to the original page where this allocation came from.
		std::shared_ptr<FCpuDescriptorAllocatorPage> mPage;
	};
}