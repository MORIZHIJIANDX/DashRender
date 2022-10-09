#pragma once

#include "d3dx12.h"

namespace Dash
{
	class FRootParameter : public D3D12_ROOT_PARAMETER
	{
		friend class FRootSignature;
	public:
		FRootParameter()
		{
			ParameterType = D3D12_ROOT_PARAMETER_TYPE(0xFFFFFFFF);
		}

		~FRootParameter()
		{
			Clear();
		}

		void Clear()
		{
			if (ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE && DescriptorTable.pDescriptorRanges != nullptr)
			{
				delete[] DescriptorTable.pDescriptorRanges;
			}

			ParameterType = D3D12_ROOT_PARAMETER_TYPE(0xFFFFFFFF);
		}

		void InitAsRootConstants(UINT shaderRegister, UINT numDwords, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL, UINT space = 0)
		{
			ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			ShaderVisibility = visibility;
			Constants.Num32BitValues = numDwords;
			Constants.ShaderRegister = shaderRegister;
			Constants.RegisterSpace = space;
		}

		void InitAsRootConstantBufferView(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL, UINT space = 0)
		{
			ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			ShaderVisibility = visibility;
			Descriptor.ShaderRegister = shaderRegister;
			Descriptor.RegisterSpace = space;
		}

		void InitAsRootShaderResourceView(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL, UINT space = 0)
		{
			ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
			ShaderVisibility = visibility;
			Descriptor.ShaderRegister = shaderRegister;
			Descriptor.RegisterSpace = space;
		}

		void InitAsRootUnorderAccessView(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL, UINT space = 0)
		{
			ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
			ShaderVisibility = visibility;
			Descriptor.ShaderRegister = shaderRegister;
			Descriptor.RegisterSpace = space;
		}

		void InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE type, UINT shaderRegister, UINT count, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL, UINT space = 0)
		{
			InitAsDescriptorTable(1, visibility);
			SetTableRange(0, type, shaderRegister, count, space);
		}

		void InitAsDescriptorTable(UINT rangeCount, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			ShaderVisibility = visibility;
			DescriptorTable.NumDescriptorRanges = rangeCount;
			DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[rangeCount];
		}

		void SetTableRange(UINT rangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE type, UINT shaderRegister, UINT count, UINT space = 0)
		{
			D3D12_DESCRIPTOR_RANGE* range = const_cast<D3D12_DESCRIPTOR_RANGE*>(DescriptorTable.pDescriptorRanges + rangeIndex);
			range->BaseShaderRegister = shaderRegister;
			range->NumDescriptors = count;
			range->RangeType = type;
			range->RegisterSpace = space;
			range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		}
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

		uint32_t GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE type) const;

		uint32_t GetNumDescriptors(uint32_t rootParameterIndex) const;

		void InitStaticSampler(UINT shaderRegister, const D3D12_SAMPLER_DESC& desc, D3D12_SHADER_VISIBILITY visibility);

		void Finalize(const std::wstring& name, D3D12_ROOT_SIGNATURE_FLAGS flag = D3D12_ROOT_SIGNATURE_FLAG_NONE);

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

	protected:
		std::atomic<bool> mFinalized;
		UINT mNumParameters;
		UINT mNumStaticSamplers;
		UINT mNumInitializedStaticSamplers;
		uint32_t mSamplerTableMask;
		uint32_t mDescriptorTableMask;
		uint32_t mNumDescriptorsPerTable[32];
		std::unique_ptr<D3D12_STATIC_SAMPLER_DESC[]> mSamplerArray;
		std::unique_ptr<FRootParameter[]> mParameterArray;
		ID3D12RootSignature* mRootSignature;
	};
}