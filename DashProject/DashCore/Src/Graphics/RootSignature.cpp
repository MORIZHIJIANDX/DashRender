#include "PCH.h"
#include "RootSignature.h"

namespace Dash
{
	
	void FRootSignature::DestroyAll()
	{

	}

	void FRootSignature::InitStaticSampler(UINT shaderRegister, const D3D12_SAMPLER_DESC& desc, D3D12_SHADER_VISIBILITY visibility)
	{
		ASSERT(mNumInitializedStaticSamplers < mNumStaticSamplers);
		D3D12_STATIC_SAMPLER_DESC staticSamplerDec{};

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

	void FRootSignature::Finalize(const std::wstring& name, D3D12_ROOT_SIGNATURE_FLAGS flag)
	{
		if (mFinalized == true)
		{
			return;
		}

		ASSERT(mNumInitializedStaticSamplers = mNumStaticSamplers);

		D3D12_ROOT_SIGNATURE_DESC desc;
		desc.Flags = flag;
		desc.NumParameters = mNumParameters;
		desc.NumStaticSamplers = mNumStaticSamplers;
		

		mFinalized = true;
	}
}