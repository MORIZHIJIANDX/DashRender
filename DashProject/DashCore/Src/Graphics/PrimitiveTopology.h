#pragma once

namespace Dash
{
	enum class EPrimitiveTopology
	{
		None,

		PointList,

		TriangleList,
		TriangleList_Adj,

		TriangleStrip,
		TriangleStrip_Adj,

		LineList,
		LineList_Adj,

		LineStrip,
		LineStrip_Adj,

		PatchList			// To select the patch points --> Pass in PipelineStateDesc!
	};

	D3D_PRIMITIVE_TOPOLOGY D3DPrimitiveTopology(EPrimitiveTopology topology, uint8_t numControlPatches = 0);

	D3D12_PRIMITIVE_TOPOLOGY_TYPE D3DPrimitiveTopologyType(EPrimitiveTopology topology);
}

