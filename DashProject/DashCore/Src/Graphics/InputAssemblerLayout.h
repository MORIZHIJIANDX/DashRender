#pragma once

#include "ResourceFormat.h"

namespace Dash
{
	enum class EPerVertexSemantic
	{
		Position,
		Normal,
		Tangent,
		VertexColor,
		TexCoord,
		Unknown,
	};

	class FInputAssemblerLayout
	{
	public:
		void AddPerVertexLayoutElement(const std::string& semanticName, uint32_t semanticIndex, EResourceFormat format, uint32_t inputSlot = 0, uint32_t alignedOffset = D3D12_APPEND_ALIGNED_ELEMENT);
		void AddPerInstanceLayoutElement(const std::string& semanticName, uint32_t semanticIndex, EResourceFormat format, uint32_t inputSlot = 0, uint32_t alignedOffset = D3D12_APPEND_ALIGNED_ELEMENT, uint32_t stepRate = 1);

		const D3D12_INPUT_LAYOUT_DESC& D3DLayout() const { return mDesc; }

		const std::vector<std::pair<uint32_t, std::string>>& GetPerVertexSemantics() const { return mPerVertexSemantics; }

	private:
		void SetSemanticNames();
		void BuildPerVertexSemantics(const std::string& semanticName, uint32_t inputSlot);

	private:

		std::vector<std::pair<uint32_t, std::string>> mPerVertexSemantics;

		std::vector<std::string> mElementSemanticNames;
		std::vector<D3D12_INPUT_ELEMENT_DESC> mInputElements;
		D3D12_INPUT_LAYOUT_DESC mDesc{};
	};

}