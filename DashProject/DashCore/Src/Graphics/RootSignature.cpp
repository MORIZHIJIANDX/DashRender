#include "PCH.h"
#include "RootSignature.h"
#include "DX12Helper.h"
#include "GraphicsCore.h"
#include "Utility/Hash.h"

namespace Dash
{
	static std::map<size_t, Microsoft::WRL::ComPtr<ID3D12RootSignature>> RootSignatureHashMap;

	void FRootSignature::DestroyAll()
	{
		RootSignatureHashMap.clear();
	}

	uint32_t FRootSignature::GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE type) const
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

	uint32_t FRootSignature::GetNumDescriptors(uint32_t rootParameterIndex) const
	{
		ASSERT(rootParameterIndex < mNumParameters);
		return mNumDescriptorsPerTable[rootParameterIndex];
	}

	void FRootSignature::InitStaticSampler(UINT shaderRegister, const D3D12_SAMPLER_DESC& desc, D3D12_SHADER_VISIBILITY visibility)
	{
		ASSERT(mNumInitializedStaticSamplers < mNumStaticSamplers);
		D3D12_STATIC_SAMPLER_DESC& staticSamplerDec = mSamplerArray[mNumInitializedStaticSamplers++];

		staticSamplerDec.AddressU = desc.AddressU;
		staticSamplerDec.AddressV = desc.AddressV;
		staticSamplerDec.AddressW = desc.AddressW;
		staticSamplerDec.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		staticSamplerDec.ComparisonFunc = desc.ComparisonFunc;
		staticSamplerDec.Filter = desc.Filter;
		staticSamplerDec.MaxAnisotropy = desc.MaxAnisotropy;
		staticSamplerDec.MaxLOD = desc.MaxLOD;
		staticSamplerDec.MinLOD = desc.MinLOD;
		staticSamplerDec.MipLODBias = desc.MipLODBias;
		staticSamplerDec.RegisterSpace = staticSamplerDec.RegisterSpace;
		staticSamplerDec.ShaderRegister = shaderRegister;
		staticSamplerDec.ShaderVisibility = visibility;

		if (desc.AddressU == D3D12_TEXTURE_ADDRESS_MODE_BORDER ||
			desc.AddressV == D3D12_TEXTURE_ADDRESS_MODE_BORDER ||
			desc.AddressW == D3D12_TEXTURE_ADDRESS_MODE_BORDER)
		{
			ASSERT_MSG(	desc.BorderColor[0] == 0.0f && desc.BorderColor[1] == 0.0f && desc.BorderColor[2] == 0.0f && desc.BorderColor[3] == 0.0f || // Transparent Black
						desc.BorderColor[0] == 0.0f && desc.BorderColor[1] == 0.0f && desc.BorderColor[2] == 0.0f && desc.BorderColor[3] == 1.0f || // Opaque Black
						desc.BorderColor[0] == 1.0f && desc.BorderColor[1] == 1.0f && desc.BorderColor[2] == 1.0f && desc.BorderColor[3] == 1.0f ,  // Opaque White
						"Sampler border color does not match static sampler limitations");

			if (desc.BorderColor[3] == 1.0f)
			{
				if (desc.BorderColor[0] == 1.0f)
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

		D3D12_ROOT_SIGNATURE_DESC desc{};
		desc.Flags = flag;
		desc.NumParameters = mNumParameters;
		desc.NumStaticSamplers = mNumStaticSamplers;
		desc.pParameters = (const D3D12_ROOT_PARAMETER*)(mParameterArray.get());
		desc.pStaticSamplers = (const D3D12_STATIC_SAMPLER_DESC*)(mSamplerArray.get());

		mDescriptorTableMask = 0;
		mSamplerTableMask = 0;

		size_t hashCode = HashState(&desc);
		hashCode = HashState(desc.pStaticSamplers, mNumStaticSamplers, hashCode);

		for (UINT paramIndex = 0; paramIndex < mNumParameters; paramIndex++)
		{
			const D3D12_ROOT_PARAMETER& rootParameter = desc.pParameters[paramIndex];
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
		ID3D12RootSignature** signatureRef = nullptr;
		bool firstTimeCompile = false;
		{
			std::lock_guard<std::mutex> lock(signatureMutex);

			auto iter = RootSignatureHashMap.find(hashCode);
			if (iter == RootSignatureHashMap.end())
			{
				signatureRef = RootSignatureHashMap[hashCode].GetAddressOf();
				firstTimeCompile = true;
			}
			else
			{
				signatureRef = iter->second.GetAddressOf();
			}
		}

		if (firstTimeCompile == true)
		{
			Microsoft::WRL::ComPtr<ID3DBlob> outBlob, errorBlob;

			//DX_CALL(D3D12SerializeRootSignature(&desc, FGraphicsCore::GetRootSignatureVersion(), &outBlob, &errorBlob));
			D3D_ROOT_SIGNATURE_VERSION version = FGraphicsCore::GetRootSignatureVersion(); 
			DX_CALL(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &outBlob, &errorBlob));

			DX_CALL(FGraphicsCore::Device->CreateRootSignature(0, outBlob->GetBufferPointer(), outBlob->GetBufferSize(), IID_PPV_ARGS(&mRootSignature)));

			SetD3D12DebugName(mRootSignature, name.c_str());

			RootSignatureHashMap[hashCode].Attach(mRootSignature);

			ASSERT(*signatureRef == mRootSignature);
		}
		else
		{
			while (*signatureRef == nullptr)
			{
				std::this_thread::yield();
			}

			mRootSignature = *signatureRef;
		}

		mFinalized = true;
	}
}