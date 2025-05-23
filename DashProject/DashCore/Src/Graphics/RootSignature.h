#pragma once

#include "SamplerDesc.h"

namespace Dash
{
	class FRootSignature;
	using FRootSignatureRef = std::shared_ptr<FRootSignature>;

	class FRootParameter
	{
		friend class FRootSignature;
	public:
		FRootParameter()
		{
			mRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE(0xFFFFFFFF);
		}

		~FRootParameter()
		{
			Clear();
		}

		void Clear()
		{
			if (mRootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE && mRootParameter.DescriptorTable.pDescriptorRanges != nullptr)
			{
				delete[] mRootParameter.DescriptorTable.pDescriptorRanges;
			}

			mRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE(0xFFFFFFFF);
		}

		void InitAsRootConstants(UINT shaderRegister, UINT numDwords, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL, UINT space = 0)
		{
			mRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			mRootParameter.ShaderVisibility = visibility;
			mRootParameter.Constants.Num32BitValues = numDwords;
			mRootParameter.Constants.ShaderRegister = shaderRegister;
			mRootParameter.Constants.RegisterSpace = space;
		}

		void InitAsRootConstantBufferView(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL, UINT space = 0)
		{
			mRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			mRootParameter.ShaderVisibility = visibility;
			mRootParameter.Descriptor.ShaderRegister = shaderRegister;
			mRootParameter.Descriptor.RegisterSpace = space;
			mRootParameter.Descriptor.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE;
		}

		void InitAsRootShaderResourceView(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL, UINT space = 0)
		{
			mRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
			mRootParameter.ShaderVisibility = visibility;
			mRootParameter.Descriptor.ShaderRegister = shaderRegister;
			mRootParameter.Descriptor.RegisterSpace = space;
			mRootParameter.Descriptor.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE;
		}

		void InitAsRootUnorderAccessView(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL, UINT space = 0)
		{
			mRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
			mRootParameter.ShaderVisibility = visibility;
			mRootParameter.Descriptor.ShaderRegister = shaderRegister;
			mRootParameter.Descriptor.RegisterSpace = space;
			mRootParameter.Descriptor.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE;
		}

		void InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE type, UINT shaderRegister, UINT count, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL, UINT space = 0)
		{
			InitAsDescriptorTable(1, visibility);
			SetTableRange(0, type, shaderRegister, count, space);
		}

		void InitAsDescriptorTable(UINT rangeCount, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			mRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			mRootParameter.ShaderVisibility = visibility;
			mRootParameter.DescriptorTable.NumDescriptorRanges = rangeCount;
			mRootParameter.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE1[rangeCount];
		}

		void SetTableRange(UINT rangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE type, UINT shaderRegister, UINT count, UINT space = 0)
		{
			D3D12_DESCRIPTOR_RANGE1* range = const_cast<D3D12_DESCRIPTOR_RANGE1*>(mRootParameter.DescriptorTable.pDescriptorRanges + rangeIndex);
			range->BaseShaderRegister = shaderRegister;
			range->NumDescriptors = count;
			range->RangeType = type;
			range->RegisterSpace = space;
			range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			range->Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
		}

		const D3D12_ROOT_PARAMETER1& operator() (void) const { return mRootParameter; }

	private: 
		D3D12_ROOT_PARAMETER1 mRootParameter{};
	};

	class FRootSignature
	{
	public:
		FRootSignature(UINT numRootParameters = 0, UINT numStaticSamplers = 0)
			: mFinalized(false)
			, mNumParameters(numRootParameters)
			, mNumStaticSamplers(numStaticSamplers)
			, mNumInitializedStaticSamplers(0)
			, mSamplerTableMask(0)
			, mDescriptorTableMask(0)
			, mNumDescriptorsPerTable{0}
			, mRootSignature(nullptr)
		{}

		~FRootSignature() {}

		static FRootSignatureRef MakeRootSignature(UINT numRootParameters = 0, UINT numStaticSamplers = 0);

		static void DestroyAll();

		void Reset(UINT numRootParameters, UINT numStaticSamplers)
		{
			if (numRootParameters > 0)
			{
				mParameterArray.reset(new FRootParameter[numRootParameters]);
			}
			else
			{
				mParameterArray = nullptr;
			}

			if (numStaticSamplers > 0)
			{
				mSamplerArray.reset(new D3D12_STATIC_SAMPLER_DESC[numStaticSamplers]);
			}
			else
			{
				mSamplerArray = nullptr;
			}

			mNumParameters = numRootParameters;
			mNumStaticSamplers = numStaticSamplers;
			mNumInitializedStaticSamplers = 0;
		}

		uint32 GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE type) const;

		uint32 GetNumDescriptors(uint32 rootParameterIndex) const;

		void InitStaticSampler(UINT shaderRegister, const FSamplerDesc& desc, D3D12_SHADER_VISIBILITY visibility, UINT space = 0);

		void Finalize(const std::string& name, D3D12_ROOT_SIGNATURE_FLAGS flag = D3D12_ROOT_SIGNATURE_FLAG_NONE);

		FRootParameter& operator[](size_t parameterIndex)
		{
			ASSERT(parameterIndex < mNumParameters);
			return mParameterArray.get()[parameterIndex];
		}

		const FRootParameter& operator[](size_t parameterIndex) const
		{
			ASSERT(parameterIndex < mNumParameters);
			return mParameterArray.get()[parameterIndex];
		}

		ID3D12RootSignature* GetSignature() const { return mRootSignature; }

		bool IsFinalized() const { return mFinalized; }

	protected:
		std::string mName;
		std::atomic<bool> mFinalized;
		UINT mNumParameters;
		UINT mNumStaticSamplers;
		UINT mNumInitializedStaticSamplers;
		uint32 mSamplerTableMask;
		uint32 mDescriptorTableMask;
		uint32 mNumDescriptorsPerTable[32];
		std::unique_ptr<D3D12_STATIC_SAMPLER_DESC[]> mSamplerArray;
		std::unique_ptr<FRootParameter[]> mParameterArray;
		ID3D12RootSignature* mRootSignature;
	};

	
}