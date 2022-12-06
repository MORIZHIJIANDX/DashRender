#pragma once

#include <dxgi1_6.h>
#include <d3d12.h>
#include "ResourceFormat.h"

namespace Dash
{
	class FInputAssemblerLayout
	{
	public:
		void AddPerVertexLayoutElement(const std::string& semanticName, uint32_t semanticIndex, EResourceFormat format, uint32_t inputSlot, uint32_t alignedOffset);
		void AddPerInstanceLayoutElement(const std::string& semanticName, uint32_t semanticIndex, EResourceFormat format, uint32_t inputSlot, uint32_t alignedOffset, uint32_t stepRate);

		const D3D12_INPUT_LAYOUT_DESC& D3DLayout() const { return mDesc; };

	private:
		void SetSemanticNames();

	private:
		std::vector<std::string> mElementSemanticNames;
		std::vector<D3D12_INPUT_ELEMENT_DESC> mInputElements;
		D3D12_INPUT_LAYOUT_DESC mDesc{};
	};

}