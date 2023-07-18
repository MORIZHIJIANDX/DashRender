#include "PCH.h"
#include "ResourceState.h"

namespace Dash
{
	bool IsResourceStateReadOnly(EResourceState state)
	{
        return (EnumMaskContains(state, EResourceState::PixelShaderAccess)) ||
            (EnumMaskContains(state, EResourceState::NonPixelShaderAccess)) ||
            (EnumMaskContains(state, EResourceState::IndirectArgument)) ||
            (EnumMaskContains(state, EResourceState::CopySource)) ||
            (EnumMaskContains(state, EResourceState::GenericRead)) ||
            (EnumMaskContains(state, EResourceState::RaytracingAccelerationStructure)) ||
            (EnumMaskContains(state, EResourceState::Predication)) ||
            (EnumMaskContains(state, EResourceState::Present)) ||
            (EnumMaskContains(state, EResourceState::DepthRead)) ||
            (EnumMaskContains(state, EResourceState::ConstantBuffer)) ||
            (EnumMaskContains(state, EResourceState::VertexBuffer)) || 
            (EnumMaskContains(state, EResourceState::IndexBuffer));
	}

    bool IsResourceStateUsageSupportedOnGraphicsQueue(EResourceState state)
    {
        return true;
    }

    bool IsResourceStateTransitionsSupportedOnGraphicsQueue(EResourceState state)
    {
        return true;
    }

    bool IsResourceStateUsageSupportedOnComputeQueue(EResourceState state)
    {
        EResourceState allowedStates =
            EResourceState::Common |
            EResourceState::AnyShaderAccess |
            EResourceState::GenericRead |
            EResourceState::CopyDestination |
            EResourceState::CopySource |
            EResourceState::UnorderedAccess |
            EResourceState::RaytracingAccelerationStructure |
            EResourceState::ConstantBuffer |
            EResourceState::DepthRead;

        return !EnumMaskContains(state, ~allowedStates);
    }

    bool IsResourceStateTransitionSupportedOnComputeQueue(EResourceState state)
    {
        EResourceState allowedStates =
            EResourceState::Common |
            EResourceState::NonPixelShaderAccess |
            EResourceState::GenericRead |
            EResourceState::CopyDestination |
            EResourceState::CopySource |
            EResourceState::UnorderedAccess |
            EResourceState::RaytracingAccelerationStructure |
            EResourceState::ConstantBuffer;

        return !EnumMaskContains(state, ~allowedStates);
    }

    bool IsResourceStateUsageSupportedOnCopyQueue(EResourceState state)
    {
        EResourceState allowedStates = EResourceState::Common | EResourceState::CopyDestination | EResourceState::CopySource;
        return !EnumMaskContains(state, ~allowedStates);
    }

    bool IsResourceStateTransitionSupportedOnCopyQueue(EResourceState state)
    {
        EResourceState allowedStates = EResourceState::Common | EResourceState::CopyDestination | EResourceState::CopySource;
        return !EnumMaskContains(state, ~allowedStates);
    }

    D3D12_RESOURCE_STATES D3DResourceState(EResourceState state)
    {
        D3D12_RESOURCE_STATES states = D3D12_RESOURCE_STATE_COMMON;
        if (EnumMaskContains(state, EResourceState::UnorderedAccess)) states |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        if (EnumMaskContains(state, EResourceState::PixelShaderAccess)) states |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        if (EnumMaskContains(state, EResourceState::NonPixelShaderAccess)) states |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        if (EnumMaskContains(state, EResourceState::StreamOut)) states |= D3D12_RESOURCE_STATE_STREAM_OUT;
        if (EnumMaskContains(state, EResourceState::IndirectArgument)) states |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
        if (EnumMaskContains(state, EResourceState::CopyDestination)) states |= D3D12_RESOURCE_STATE_COPY_DEST;
        if (EnumMaskContains(state, EResourceState::CopySource)) states |= D3D12_RESOURCE_STATE_COPY_SOURCE;
        if (EnumMaskContains(state, EResourceState::GenericRead)) states |= D3D12_RESOURCE_STATE_GENERIC_READ;
        if (EnumMaskContains(state, EResourceState::RaytracingAccelerationStructure)) states |= D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
        if (EnumMaskContains(state, EResourceState::Predication)) states |= D3D12_RESOURCE_STATE_PREDICATION;
        if (EnumMaskContains(state, EResourceState::RenderTarget)) states |= D3D12_RESOURCE_STATE_RENDER_TARGET;
        if (EnumMaskContains(state, EResourceState::ResolveDestination)) states |= D3D12_RESOURCE_STATE_RESOLVE_DEST;
        if (EnumMaskContains(state, EResourceState::ResolveSource)) states |= D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
        if (EnumMaskContains(state, EResourceState::Present)) states |= D3D12_RESOURCE_STATE_PRESENT;
        if (EnumMaskContains(state, EResourceState::DepthRead)) states |= D3D12_RESOURCE_STATE_DEPTH_READ;
        if (EnumMaskContains(state, EResourceState::DepthWrite)) states |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
        if (EnumMaskContains(state, EResourceState::ConstantBuffer)) states |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        if (EnumMaskContains(state, EResourceState::VertexBuffer)) states |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        if (EnumMaskContains(state, EResourceState::IndexBuffer)) states |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
        return states;
    }

    std::string StateString(EResourceState state)
    {
        std::string string = "";

        auto addStateString = [&string](const std::string& newString)
        {
            if (!string.empty())
            {
                string += " | ";
            }

            string += newString;
        };

        if (EnumMaskContains(state, EResourceState::UnorderedAccess)) addStateString("UnorderedAccess");
        if (EnumMaskContains(state, EResourceState::PixelShaderAccess))  addStateString("PixelShaderAccess");
        if (EnumMaskContains(state, EResourceState::NonPixelShaderAccess))  addStateString("NonPixelShaderAccess");
        if (EnumMaskContains(state, EResourceState::StreamOut))  addStateString("StreamOut");
        if (EnumMaskContains(state, EResourceState::IndirectArgument))  addStateString("IndirectArgument");
        if (EnumMaskContains(state, EResourceState::CopyDestination))  addStateString("CopyDestination");
        if (EnumMaskContains(state, EResourceState::CopySource))  addStateString("CopySource");
        if (EnumMaskContains(state, EResourceState::GenericRead))  addStateString("GenericRead");
        if (EnumMaskContains(state, EResourceState::RaytracingAccelerationStructure))  addStateString("RaytracingAccelerationStructure");
        if (EnumMaskContains(state, EResourceState::Predication))  addStateString("Predication");
        if (EnumMaskContains(state, EResourceState::RenderTarget))  addStateString("RenderTarget");
        if (EnumMaskContains(state, EResourceState::ResolveDestination))  addStateString("ResolveDestination");
        if (EnumMaskContains(state, EResourceState::ResolveSource))  addStateString("ResolveSource");
        if (EnumMaskContains(state, EResourceState::Present))  addStateString("Present");
        if (EnumMaskContains(state, EResourceState::DepthRead))  addStateString("DepthRead");
        if (EnumMaskContains(state, EResourceState::DepthWrite))  addStateString("DepthWrite");
        if (EnumMaskContains(state, EResourceState::ConstantBuffer))  addStateString("ConstantBuffer");
        if (EnumMaskContains(state, EResourceState::VertexBuffer))  addStateString("VertexBuffer");
        if (EnumMaskContains(state, EResourceState::IndexBuffer))  addStateString("IndexBuffer");

        if (string.empty())
        {
            string = "Common";
        }

        return string;
    }
}