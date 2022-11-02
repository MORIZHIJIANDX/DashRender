#include "PCH.h"
#include "SamplerDesc.h"
#include "CpuDescriptorAllocation.h"
#include "CpuDescriptorAllocator.h"
#include "GraphicsCore.h"
#include "Utility/Hash.h"

namespace Dash
{
    static std::map<size_t, FCpuDescriptorAllocation> SamplerDescriptorCache;

    void FSamplerDesc::DestroyAll()
    {
        SamplerDescriptorCache.clear();
    }

    void FSamplerDesc::SetTextureAddressMode(D3D12_TEXTURE_ADDRESS_MODE mode)
    {
        AddressU = mode;
        AddressV = mode;
        AddressW = mode;
    }

    void FSamplerDesc::SetBorderColor(FLinearColor color)
    {
        BorderColor[0] = color.r;
        BorderColor[1] = color.g;
        BorderColor[2] = color.b;
        BorderColor[3] = color.a;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE FSamplerDesc::CreateDescriptor()
    {
        size_t hashCode = HashState(this);

        static std::mutex samplerLock;
        std::lock_guard<std::mutex> lock(samplerLock);

        auto iter = SamplerDescriptorCache.find(hashCode);
        if (iter != SamplerDescriptorCache.end())
        {
            return iter->second.GetDescriptorHandle();
        }

        FCpuDescriptorAllocation cpuDescriptor = FGraphicsCore::DescriptorAllocator->AllocateSamplerDescriptor();
        D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = cpuDescriptor.GetDescriptorHandle();
        FGraphicsCore::Device->CreateSampler(this, descriptorHandle);
        SamplerDescriptorCache[hashCode] = std::move(cpuDescriptor);

        return descriptorHandle;
    }
}