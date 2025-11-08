#pragma once

#include "SamplerDesc.h"
#include "GraphicsDefines.h"
#include "Utility/RefCounting.h"

namespace Dash
{
	struct FShaderRegisterCounts
	{
		uint8 SamplerCount = 0;
		uint8 ConstantBufferCount = 0;
		uint8 ShaderResourceCount = 0;
		uint8 UnorderedAccessCount = 0;

		inline bool operator==(const FShaderRegisterCounts& RHS) const
		{
			return SamplerCount == RHS.SamplerCount &&
				ConstantBufferCount == RHS.ConstantBufferCount &&
				ShaderResourceCount == RHS.ShaderResourceCount &&
				UnorderedAccessCount == RHS.UnorderedAccessCount;
		}
	};

	struct FQuantizedBoundShaderState
	{
		std::array<FShaderRegisterCounts, GShaderStageCount> RegisterCounts;
		EShaderPassType RootSignatureType = EShaderPassType::Raster;
		uint32 NumRootParameters = 0;
		uint32 NumStaticSamplers = 0;

		inline bool operator==(const FQuantizedBoundShaderState& RHS) const
		{
			return RegisterCounts == RHS.RegisterCounts &&
				RootSignatureType == RHS.RootSignatureType &&
				NumRootParameters == RHS.NumRootParameters &&
				NumStaticSamplers == RHS.NumStaticSamplers;
		}

		size_t GetTypeHash() const;
	};

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

		void InitAsRootConstants(uint32 shaderRegister, uint32 numDwords, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL, uint32 space = 0)
		{
			mRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			mRootParameter.ShaderVisibility = visibility;
			mRootParameter.Constants.Num32BitValues = numDwords;
			mRootParameter.Constants.ShaderRegister = shaderRegister;
			mRootParameter.Constants.RegisterSpace = space;
		}

		void InitAsRootConstantBufferView(uint32 shaderRegister, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL, uint32 space = 0, D3D12_ROOT_DESCRIPTOR_FLAGS flag = D3D12_ROOT_DESCRIPTOR_FLAG_NONE)
		{
			mRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			mRootParameter.ShaderVisibility = visibility;
			mRootParameter.Descriptor.ShaderRegister = shaderRegister;
			mRootParameter.Descriptor.RegisterSpace = space;
			mRootParameter.Descriptor.Flags = flag;
		}

		void InitAsRootShaderResourceView(uint32 shaderRegister, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL, uint32 space = 0, D3D12_ROOT_DESCRIPTOR_FLAGS flag = D3D12_ROOT_DESCRIPTOR_FLAG_NONE)
		{
			mRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
			mRootParameter.ShaderVisibility = visibility;
			mRootParameter.Descriptor.ShaderRegister = shaderRegister;
			mRootParameter.Descriptor.RegisterSpace = space;
			mRootParameter.Descriptor.Flags = flag;
		}

		void InitAsRootUnorderAccessView(uint32 shaderRegister, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL, uint32 space = 0, D3D12_ROOT_DESCRIPTOR_FLAGS flag = D3D12_ROOT_DESCRIPTOR_FLAG_NONE)
		{
			mRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
			mRootParameter.ShaderVisibility = visibility;
			mRootParameter.Descriptor.ShaderRegister = shaderRegister;
			mRootParameter.Descriptor.RegisterSpace = space;
			mRootParameter.Descriptor.Flags = flag;
		}

		void InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE type, uint32 shaderRegister, uint32 count, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL, uint32 space = 0)
		{
			InitAsDescriptorTable(1, visibility);
			SetTableRange(0, type, shaderRegister, count, space);
		}

		void InitAsDescriptorTable(uint32 rangeCount, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL)
		{
			mRootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			mRootParameter.ShaderVisibility = visibility;
			mRootParameter.DescriptorTable.NumDescriptorRanges = rangeCount;
			mRootParameter.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE1[rangeCount];
		}

		void SetTableRange(uint32 rangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE type, uint32 baseShaderRegister, uint32 count, uint32 space = 0, D3D12_DESCRIPTOR_RANGE_FLAGS flag = D3D12_DESCRIPTOR_RANGE_FLAG_NONE)
		{
			D3D12_DESCRIPTOR_RANGE1* range = const_cast<D3D12_DESCRIPTOR_RANGE1*>(mRootParameter.DescriptorTable.pDescriptorRanges + rangeIndex);
			range->BaseShaderRegister = baseShaderRegister;
			range->NumDescriptors = count;
			range->RangeType = type;
			range->RegisterSpace = space;
			range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
			range->Flags = flag;
		}

		const D3D12_ROOT_PARAMETER1& operator() (void) const { return mRootParameter; }

	private: 
		D3D12_ROOT_PARAMETER1 mRootParameter{};
	};

	struct FBoundShaderState
	{
	public:
		FBoundShaderState(const FQuantizedBoundShaderState& boundShaderState)
			: NumParameters(boundShaderState.NumRootParameters)
			, NumStaticSamplers(boundShaderState.NumStaticSamplers)
			, NumInitializedStaticSamplers(0)
			, NumDescriptorsPerTable{ 0 }
		{
			if (NumParameters > 0)
			{
				ParameterArray.reset(new FRootParameter[NumParameters]);
			}
			else
			{
				ParameterArray = nullptr;
			}

			if (NumStaticSamplers > 0)
			{
				SamplerArray.reset(new D3D12_STATIC_SAMPLER_DESC[NumStaticSamplers]);
			}
			else
			{
				SamplerArray = nullptr;
			}

			NumInitializedStaticSamplers = 0;
			HashCode = boundShaderState.GetTypeHash();
		}

		~FBoundShaderState() {};

		FRootParameter& operator[](size_t parameterIndex)
		{
			ASSERT(parameterIndex < NumParameters);
			return ParameterArray.get()[parameterIndex];
		}

		const FRootParameter& operator[](size_t parameterIndex) const
		{
			ASSERT(parameterIndex < NumParameters);
			return ParameterArray.get()[parameterIndex];
		}
		
		void InitStaticSampler(uint32 shaderRegister, const FSamplerDesc& desc, D3D12_SHADER_VISIBILITY visibility, uint32 space = 0);

		void Finalize(D3D12_ROOT_SIGNATURE_FLAGS flag = D3D12_ROOT_SIGNATURE_FLAG_NONE);

		uint32 NumParameters;
		uint32 NumStaticSamplers;
		uint32 NumInitializedStaticSamplers;

		uint32 SamplerTableMask;
		uint32 DescriptorTableMask;
		uint32 NumDescriptorsPerTable[32];

		std::unique_ptr<D3D12_STATIC_SAMPLER_DESC[]> SamplerArray;
		std::unique_ptr<FRootParameter[]> ParameterArray;

		D3D12_ROOT_SIGNATURE_DESC1 RootSignatureDesc;
		size_t HashCode;
	};

	class FRootSignature
	{
	public:
		FRootSignature(const FBoundShaderState& boundShaderState, const std::string& name)
			: mName(name)
			, mNumParameters(boundShaderState.NumParameters)
			, mSamplerTableMask(boundShaderState.SamplerTableMask)
			, mDescriptorTableMask(boundShaderState.DescriptorTableMask)
			, mRootSignatureDesc(boundShaderState.RootSignatureDesc)
		{
			Init(boundShaderState);
		}

		~FRootSignature() {}

		uint32 GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE type) const;

		uint32 GetNumDescriptors(uint32 rootParameterIndex) const;

		ID3D12RootSignature* GetSignature() const { return mRootSignature; }

	protected:
		void Init(const FBoundShaderState& boundShaderState);

	protected:
		std::string mName;
		uint32 mNumParameters;
		uint32 mSamplerTableMask;
		uint32 mDescriptorTableMask;
		uint32 mNumDescriptorsPerTable[32];
		D3D12_ROOT_SIGNATURE_DESC1 mRootSignatureDesc;
		TRefCountPtr<ID3D12RootSignature> mRootSignature;
	};

	class FRootSignatureManager
	{
	public:
		FRootSignatureManager() {}
		~FRootSignatureManager()
		{
			ASSERT(RootSignatureHashMap.size() == 0);
		}

		void Destroy();

		FRootSignature* GetRootSignature(const FBoundShaderState& boundShaderState, const std::string& name);

	private:
		FRootSignature* CreateRootSignature(const FBoundShaderState& boundShaderState, const std::string& name);

	private:
		std::mutex mLock;
		std::map<size_t, FRootSignature*> RootSignatureHashMap;
	};
}