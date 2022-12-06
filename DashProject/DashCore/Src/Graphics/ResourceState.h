#pragma once

#include <d3d12.h>
#include "Utility/BitwiseEnum.h"

namespace Dash
{
    enum class EResourceState : uint32_t
    {
        Common = 0,
        UnorderedAccess = 1 << 1,
        PixelShaderAccess = 1 << 2,
        NonPixelShaderAccess = 1 << 3,
        StreamOut = 1 << 4,
        IndirectArgument = 1 << 5,
        CopyDestination = 1 << 6,
        CopySource = 1 << 7,
        GenericRead = 1 << 8,
        RaytracingAccelerationStructure = 1 << 9,
        Predication = 1 << 10,
        RenderTarget = 1 << 11,
        ResolveDestination = 1 << 12,
        ResolveSource = 1 << 13,
        Present = 1 << 14,
        DepthRead = 1 << 15,
        DepthWrite = 1 << 16,
        ConstantBuffer = 1 << 18,

        AnyShaderAccess = PixelShaderAccess | NonPixelShaderAccess
    };

    bool IsResourceStateReadOnly(EResourceState state);
    bool IsResourceStateUsageSupportedOnGraphicsQueue(EResourceState state);
    bool IsResourceStateTransitionsSupportedOnGraphicsQueue(EResourceState state);
    bool IsResourceStateUsageSupportedOnComputeQueue(EResourceState state);
    bool IsResourceStateTransitionSupportedOnComputeQueue(EResourceState state);
    bool IsResourceStateUsageSupportedOnCopyQueue(EResourceState state);
    bool IsResourceStateTransitionSupportedOnCopyQueue(EResourceState state);

    D3D12_RESOURCE_STATES D3DResourceState(EResourceState state);
    std::string StateString(EResourceState state);

    ENABLE_BITMASK_OPERATORS(EResourceState);
}