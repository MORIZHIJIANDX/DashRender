#include "PCH.h"
#include "RootSignature.h"
#include "DX12Helper.h"
#include "GraphicsCore.h"
#include "Utility/Hash.h"
#include "RenderDevice.h"

namespace Dash
{
	using namespace Microsoft::WRL;

	static std::map<size_t, Microsoft::WRL::ComPtr<ID3D12RootSignature>> RootSignatureHashMap;

	FRootSignatureRef FRootSignature::MakeRootSignature(UINT numRootParameters, UINT numStaticSamplers)
	{
		return std::make_shared<FRootSignature>(numRootParameters, numStaticSamplers);
	}

	void FRootSignature::DestroyAll()
	{
		RootSignatureHashMap.clear();
	}

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

	void FRootSignature::InitStaticSampler(UINT shaderRegister, const FSamplerDesc& desc, D3D12_SHADER_VISIBILITY visibility, UINT space /* = 0 */)
	{
		ASSERT(mNumInitializedStaticSamplers < mNumStaticSamplers);
		const D3D12_SAMPLER_DESC& samplerDesc = desc.D3DSamplerDesc();
		D3D12_STATIC_SAMPLER_DESC& staticSamplerDec = mSamplerArray[mNumInitializedStaticSamplers++];

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
			ASSERT_MSG(	samplerDesc.BorderColor[0] == 0.0f && samplerDesc.BorderColor[1] == 0.0f && samplerDesc.BorderColor[2] == 0.0f && samplerDesc.BorderColor[3] == 0.0f || // Transparent Black
						samplerDesc.BorderColor[0] == 0.0f && samplerDesc.BorderColor[1] == 0.0f && samplerDesc.BorderColor[2] == 0.0f && samplerDesc.BorderColor[3] == 1.0f || // Opaque Black
						samplerDesc.BorderColor[0] == 1.0f && samplerDesc.BorderColor[1] == 1.0f && samplerDesc.BorderColor[2] == 1.0f && samplerDesc.BorderColor[3] == 1.0f ,  // Opaque White
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

	void FRootSignature::Finalize(const std::string& name, D3D12_ROOT_SIGNATURE_FLAGS flag)
	{
		if (mFinalized == true)
		{
			return;
		}

		ASSERT(mNumInitializedStaticSamplers == mNumStaticSamplers);

		mName = name;

		D3D12_ROOT_SIGNATURE_DESC1 desc{};
		desc.Flags = flag;
		desc.NumParameters = mNumParameters;
		desc.NumStaticSamplers = mNumStaticSamplers;
		desc.pParameters = (const D3D12_ROOT_PARAMETER1*)(mParameterArray.get());
		desc.pStaticSamplers = (const D3D12_STATIC_SAMPLER_DESC*)(mSamplerArray.get());

		mDescriptorTableMask = 0;
		mSamplerTableMask = 0;

		size_t hashCode = HashState(&desc);
		hashCode = HashState(desc.pStaticSamplers, mNumStaticSamplers, hashCode);

		for (UINT paramIndex = 0; paramIndex < mNumParameters; paramIndex++)
		{
			const D3D12_ROOT_PARAMETER1& rootParameter = desc.pParameters[paramIndex];
			mNumDescriptorsPerTable[paramIndex] = 0;

			if (rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			{
				ASSERT(rootParameter.DescriptorTable.pDescriptorRanges != nullptr);

				hashCode = HashState(rootParameter.DescriptorTable.pDescriptorRanges, rootParameter.DescriptorTable.NumDescriptorRanges, hashCode);

				if (rootParameter.DescriptorTable.pDescriptorRanges->RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)
				{
					 mSamplerTableMask |= (1 << paramIndex);
				}
				else
				{
					mDescriptorTableMask |= (1 << paramIndex);
				}

				for (UINT rangeIndex = 0; rangeIndex < rootParameter.DescriptorTable.NumDescriptorRanges; rangeIndex++)
				{
					mNumDescriptorsPerTable[paramIndex] += rootParameter.DescriptorTable.pDescriptorRanges[rangeIndex].NumDescriptors;
				}
			}
			else
			{
				hashCode = HashState(&rootParameter, 1, hashCode);
			}
		}

		static std::mutex signatureMutex;
		ComPtr<ID3D12RootSignature>* signatureRef = nullptr;
		bool firstTimeCompile = false;
		{
			std::lock_guard<std::mutex> lock(signatureMutex);

			auto iter = RootSignatureHashMap.find(hashCode);
			if (iter == RootSignatureHashMap.end())
			{
				signatureRef = &RootSignatureHashMap[hashCode];
				firstTimeCompile = true;
			}
			else
			{
				signatureRef = &iter->second;
			}
		}

		if (firstTimeCompile == true)
		{
			Microsoft::WRL::ComPtr<ID3DBlob> outBlob, errorBlob;

			D3D_ROOT_SIGNATURE_VERSION version = FGraphicsCore::GetRootSignatureVersion(); 

			D3D12_VERSIONED_ROOT_SIGNATURE_DESC VersionedrootSigDesc{};
			VersionedrootSigDesc.Version = version;
			VersionedrootSigDesc.Desc_1_1 = desc;

			Microsoft::WRL::ComPtr<ID3D12RootSignature> newRootSignature;
			DX_CALL(D3D12SerializeVersionedRootSignature(&VersionedrootSigDesc, &outBlob, &errorBlob));
			DX_CALL(FGraphicsCore::Device->CreateRootSignature(0, outBlob->GetBufferPointer(), outBlob->GetBufferSize(), newRootSignature));

			SetD3D12DebugName(newRootSignature.Get(), name.c_str());

			RootSignatureHashMap[hashCode] = newRootSignature;
			mRootSignature = newRootSignature.Get();

			ASSERT(*signatureRef == newRootSignature);
		}
		else
		{
			while (*signatureRef == nullptr)
			{
				std::this_thread::yield();
			}

			mRootSignature = signatureRef->Get();
		}

		mFinalized = true;
	}
}