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

    FSamplerDesc::FSamplerDesc()
    {
        mD3DSampler.Filter = D3D12_FILTER_ANISOTROPIC;
        mD3DSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        mD3DSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        mD3DSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        mD3DSampler.MipLODBias = 0.0f;
        mD3DSampler.MaxAnisotropy = 16;
        mD3DSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        mD3DSampler.BorderColor[0] = 1.0f;
        mD3DSampler.BorderColor[1] = 1.0f;
        mD3DSampler.BorderColor[2] = 1.0f;
        mD3DSampler.BorderColor[3] = 1.0f;
        mD3DSampler.MinLOD = 0.0f;
        mD3DSampler.MaxLOD = D3D12_FLOAT32_MAX;
    }

    FSamplerDesc::FSamplerDesc(ESamplerFilter filter, ESamplerAddressMode addressMode, ESamplerComparisonFunc comparisonFunc, const FLinearColor& borderColor, float lodBias, UINT maxAnisotropy, float minLod, float maxLod)
    {
        mD3DSampler.Filter = GetD3DFilter(filter);
        mD3DSampler.AddressU = GetD3DAddressMode(addressMode);
        mD3DSampler.AddressV = GetD3DAddressMode(addressMode);
        mD3DSampler.AddressW = GetD3DAddressMode(addressMode);
        mD3DSampler.MipLODBias = lodBias;
        mD3DSampler.MaxAnisotropy = maxAnisotropy;
        mD3DSampler.ComparisonFunc = GetD3DComparisonFunc(comparisonFunc);
        mD3DSampler.BorderColor[0] = borderColor.r;
        mD3DSampler.BorderColor[1] = borderColor.g;
        mD3DSampler.BorderColor[2] = borderColor.b;
        mD3DSampler.BorderColor[3] = borderColor.a;
        mD3DSampler.MinLOD = minLod;
        mD3DSampler.MaxLOD = maxLod;
    }

    void FSamplerDesc::SetFilter(ESamplerFilter filter)
    {
        mD3DSampler.Filter = GetD3DFilter(filter);
    }

    void FSamplerDesc::SetAddressMode(ESamplerAddressMode addressMode)
    {
        mD3DSampler.AddressU = GetD3DAddressMode(addressMode);
        mD3DSampler.AddressV = GetD3DAddressMode(addressMode);
        mD3DSampler.AddressW = GetD3DAddressMode(addressMode);
    }

    void FSamplerDesc::SetComparisonFunc(ESamplerComparisonFunc comparisonFunc)
    {
        mD3DSampler.ComparisonFunc = GetD3DComparisonFunc(comparisonFunc);
    }

    void FSamplerDesc::SetBorderColor(FLinearColor color)
    {
        mD3DSampler.BorderColor[0] = color.r;
        mD3DSampler.BorderColor[1] = color.g;
        mD3DSampler.BorderColor[2] = color.b;
        mD3DSampler.BorderColor[3] = color.a;
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
        FGraphicsCore::Device->CreateSampler(&this->mD3DSampler, descriptorHandle);
        SamplerDescriptorCache[hashCode] = std::move(cpuDescriptor);

        return descriptorHandle;
    }

    D3D12_FILTER FSamplerDesc::GetD3DFilter(ESamplerFilter filter)
    {
        switch (filter)
        {
        case ESamplerFilter::Point:
            return D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_POINT;
        case ESamplerFilter::Linear:
            return D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        case ESamplerFilter::Anisotropic:
            return D3D12_FILTER::D3D12_FILTER_ANISOTROPIC;
        default:
            ASSERT(false);
            return D3D12_FILTER::D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        }
    }

    D3D12_TEXTURE_ADDRESS_MODE FSamplerDesc::GetD3DAddressMode(ESamplerAddressMode mode)
    {
        switch (mode)
        {
        case ESamplerAddressMode::Wrap:
            return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        case ESamplerAddressMode::Mirror:
            return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        case ESamplerAddressMode::Clamp:
            return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        case ESamplerAddressMode::Border:
            return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        case ESamplerAddressMode::MirrorOnce:
            return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
        default:
            ASSERT(false);
            return D3D12_TEXTURE_ADDRESS_MODE::D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        }
    }

    D3D12_COMPARISON_FUNC FSamplerDesc::GetD3DComparisonFunc(ESamplerComparisonFunc func)
    {
        switch (func)
        {
        case ESamplerComparisonFunc::Never:
            return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;
        case ESamplerComparisonFunc::Less:
            return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS;
        case ESamplerComparisonFunc::Equal:
            return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_EQUAL;
        case ESamplerComparisonFunc::LessEqual:
            return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_LESS_EQUAL;
        case ESamplerComparisonFunc::Greater:
            return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER;
        case ESamplerComparisonFunc::NotEqual:
            return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER;
        case ESamplerComparisonFunc::GreaterEqual:
            return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_GREATER_EQUAL;
        case ESamplerComparisonFunc::Always:
            return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_ALWAYS;
        default:
            ASSERT(false);
            return D3D12_COMPARISON_FUNC::D3D12_COMPARISON_FUNC_NEVER;
        }
    }
}