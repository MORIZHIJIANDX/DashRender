#include "PCH.h"
#include "RootSignature.h"
#include "DX12Helper.h"
#include "GraphicsCore.h"
#include "Utility/Hash.h"
#include "RenderDevice.h"

namespace Dash
{
	uint32 FRootSignature::GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE type) const
	{
		if (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		{
			return mDescriptorTableMask;
		}
		else
		{
			return mSamplerTableMask;
		}
	}

	uint32 FRootSignature::GetNumDescriptors(uint32 rootParameterIndex) const
	{
		ASSERT(rootParameterIndex < mNumParameters);
		return mNumDescriptorsPerTable[rootParameterIndex];
	}

	void FRootSignature::Init(const FBoundShaderState& boundShaderState)
	{
		std::memcpy(mNumDescriptorsPerTable, boundShaderState.NumDescriptorsPerTable, sizeof(mNumDescriptorsPerTable));

		TRefCountPtr<ID3DBlob> outBlob;
		TRefCountPtr<ID3DBlob> errorBlob;

		D3D_ROOT_SIGNATURE_VERSION version = FGraphicsCore::GetRootSignatureVersion();

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC VersionedrootSigDesc{};
		VersionedrootSigDesc.Version = version;
		VersionedrootSigDesc.Desc_1_1 = boundShaderState.RootSignatureDesc;

		DX_CALL(D3D12SerializeVersionedRootSignature(&VersionedrootSigDesc, outBlob.GetInitReference(), errorBlob.GetInitReference()));
		DX_CALL(FGraphicsCore::Device->CreateRootSignature(0, outBlob->GetBufferPointer(), outBlob->GetBufferSize(), mRootSignature));

		SetD3D12DebugName(mRootSignature.GetReference(), mName.c_str());
	}

	void FBoundShaderState::InitStaticSampler(UINT shaderRegister, const FSamplerDesc& desc, D3D12_SHADER_VISIBILITY visibility, UINT space /* = 0 */)
	{
		ASSERT(NumInitializedStaticSamplers < NumStaticSamplers);
		const D3D12_SAMPLER_DESC& samplerDesc = desc.D3DSamplerDesc();
		D3D12_STATIC_SAMPLER_DESC& staticSamplerDec = SamplerArray[NumInitializedStaticSamplers++];

		staticSamplerDec.AddressU = samplerDesc.AddressU;
		staticSamplerDec.AddressV = samplerDesc.AddressV;
		staticSamplerDec.AddressW = samplerDesc.AddressW;
		staticSamplerDec.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		staticSamplerDec.ComparisonFunc = samplerDesc.ComparisonFunc;
		staticSamplerDec.Filter = samplerDesc.Filter;
		staticSamplerDec.MaxAnisotropy = samplerDesc.MaxAnisotropy;
		staticSamplerDec.MaxLOD = samplerDesc.MaxLOD;
		staticSamplerDec.MinLOD = samplerDesc.MinLOD;
		staticSamplerDec.MipLODBias = samplerDesc.MipLODBias;
		staticSamplerDec.RegisterSpace = space;
		staticSamplerDec.ShaderRegister = shaderRegister;
		staticSamplerDec.ShaderVisibility = visibility;

		if (samplerDesc.AddressU == D3D12_TEXTURE_ADDRESS_MODE_BORDER ||
			samplerDesc.AddressV == D3D12_TEXTURE_ADDRESS_MODE_BORDER ||
			samplerDesc.AddressW == D3D12_TEXTURE_ADDRESS_MODE_BORDER)
		{
			ASSERT_MSG(samplerDesc.BorderColor[0] == 0.0f && samplerDesc.BorderColor[1] == 0.0f && samplerDesc.BorderColor[2] == 0.0f && samplerDesc.BorderColor[3] == 0.0f || // Transparent Black
				samplerDesc.BorderColor[0] == 0.0f && samplerDesc.BorderColor[1] == 0.0f && samplerDesc.BorderColor[2] == 0.0f && samplerDesc.BorderColor[3] == 1.0f || // Opaque Black
				samplerDesc.BorderColor[0] == 1.0f && samplerDesc.BorderColor[1] == 1.0f && samplerDesc.BorderColor[2] == 1.0f && samplerDesc.BorderColor[3] == 1.0f,  // Opaque White
				"Sampler border color does not match static sampler limitations");

			if (samplerDesc.BorderColor[3] == 1.0f)
			{
				if (samplerDesc.BorderColor[0] == 1.0f)
				{
					staticSamplerDec.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
				}
				else
				{
					staticSamplerDec.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
				}
			}
			else
			{
				staticSamplerDec.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
			}
		}
	}

	void FBoundShaderState::Finalize(D3D12_ROOT_SIGNATURE_FLAGS flag)
	{
		ASSERT(NumInitializedStaticSamplers == NumStaticSamplers);

		RootSignatureDesc.Flags = flag;
		RootSignatureDesc.NumParameters = NumParameters;
		RootSignatureDesc.NumStaticSamplers = NumStaticSamplers;
		RootSignatureDesc.pParameters = (const D3D12_ROOT_PARAMETER1*)(ParameterArray.get());
		RootSignatureDesc.pStaticSamplers = (const D3D12_STATIC_SAMPLER_DESC*)(SamplerArray.get());

		DescriptorTableMask = 0;
		SamplerTableMask = 0;

		HashCode = HashState(&RootSignatureDesc);
		HashCode = HashState(RootSignatureDesc.pStaticSamplers, NumStaticSamplers, HashCode);

		for (UINT paramIndex = 0; paramIndex < NumParameters; paramIndex++)
		{
			const D3D12_ROOT_PARAMETER1& rootParameter = RootSignatureDesc.pParameters[paramIndex];
			NumDescriptorsPerTable[paramIndex] = 0;

			if (rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			{
				ASSERT(rootParameter.DescriptorTable.pDescriptorRanges != nullptr);

				HashCode = HashState(rootParameter.DescriptorTable.pDescriptorRanges, rootParameter.DescriptorTable.NumDescriptorRanges, HashCode);

				if (rootParameter.DescriptorTable.pDescriptorRanges->RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)
				{
					SamplerTableMask |= (1 << paramIndex);
				}
				else
				{
					DescriptorTableMask |= (1 << paramIndex);
				}

				for (UINT rangeIndex = 0; rangeIndex < rootParameter.DescriptorTable.NumDescriptorRanges; rangeIndex++)
				{
					NumDescriptorsPerTable[paramIndex] += rootParameter.DescriptorTable.pDescriptorRanges[rangeIndex].NumDescriptors;
				}
			}
			else
			{
				HashCode = HashState(&rootParameter, 1, HashCode);
			}
		}
	}

	void FRootSignatureManager::Destroy()
	{
		std::lock_guard<std::mutex> lock(mLock);

		for (auto& Pair : RootSignatureHashMap)
		{
			delete Pair.second;
		}

		RootSignatureHashMap.clear();
	}

	FRootSignature* FRootSignatureManager::GetRootSignature(const FBoundShaderState& boundShaderState, const std::string& name)
	{
		std::lock_guard<std::mutex> lock(mLock);

		auto iter = RootSignatureHashMap.find(boundShaderState.HashCode);

		if (iter == RootSignatureHashMap.end())
		{
			return CreateRootSignature(boundShaderState, name);
		}

		ASSERT(iter->second);
		return iter->second;
	}

	FRootSignature* FRootSignatureManager::CreateRootSignature(const FBoundShaderState& boundShaderState, const std::string& name)
	{
		FRootSignature* newRootSignature = new FRootSignature(boundShaderState, name);
		ASSERT(newRootSignature);

		RootSignatureHashMap.emplace(boundShaderState.HashCode, newRootSignature);

		return newRootSignature;
	}
}