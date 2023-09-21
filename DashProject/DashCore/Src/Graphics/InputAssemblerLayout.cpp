#include "PCH.h"
#include "InputAssemblerLayout.h"
#include "Utility/StringUtility.h"

namespace Dash
{
	void FInputAssemblerLayout::AddPerVertexLayoutElement(const std::string& semanticName, uint32_t semanticIndex, EResourceFormat format, uint32_t inputSlot, uint32_t alignedOffset)
	{
		D3D12_INPUT_ELEMENT_DESC desc{};
		desc.Format = D3DFormat(format);
		desc.AlignedByteOffset = alignedOffset;
		desc.InputSlot = inputSlot;
		desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		desc.SemanticIndex = semanticIndex;

		mInputElements.push_back(desc);
		mElementSemanticNames.push_back(semanticName);
		
		mDesc.NumElements = static_cast<UINT>(mInputElements.size());
		mDesc.pInputElementDescs = &mInputElements[0];

		SetSemanticNames();

		BuildPerVertexSemantics(semanticName, inputSlot);
	}

	void FInputAssemblerLayout::AddPerInstanceLayoutElement(const std::string& semanticName, uint32_t semanticIndex, EResourceFormat format, uint32_t inputSlot, uint32_t alignedOffset, uint32_t stepRate)
	{
		D3D12_INPUT_ELEMENT_DESC desc{};
		desc.Format = D3DFormat(format);
		desc.AlignedByteOffset = alignedOffset;
		desc.InputSlot = inputSlot;
		desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
		desc.SemanticIndex = semanticIndex;
		desc.InstanceDataStepRate = stepRate;

		mInputElements.push_back(desc);
		mElementSemanticNames.push_back(semanticName);

		mDesc.NumElements = static_cast<UINT>(mInputElements.size());
		mDesc.pInputElementDescs = &mInputElements[0];

		SetSemanticNames();
	}

	void FInputAssemblerLayout::SetSemanticNames()
	{
		for (size_t i = 0; i < mInputElements.size(); i++)
		{
			mInputElements[i].SemanticName = mElementSemanticNames[i].c_str();
		}
	}

	
	void FInputAssemblerLayout::BuildPerVertexSemantics(const std::string& semanticName, uint32_t inputSlot)
	{
		bool containsSemantic = false;

		for (auto& pair : mPerVertexSemantics)
		{
			if (pair.second == semanticName)
			{
				containsSemantic = true;
			}

			if (pair.first == inputSlot && pair.second != semanticName)
			{
				ASSERT_MSG(false, "Invalid Override Semantic");
			}
		}

		if (!containsSemantic)
		{
			mPerVertexSemantics.emplace_back(inputSlot, semanticName);
		}

		std::sort(mPerVertexSemantics.begin(), mPerVertexSemantics.end(), [](const std::pair<uint32_t, std::string>& a, std::pair<uint32_t, std::string> b) {
			return a.first < b.first;
			});
	}
}