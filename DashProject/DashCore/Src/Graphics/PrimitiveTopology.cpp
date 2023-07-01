#include "PCH.h"
#include "PrimitiveTopology.h"

namespace Dash
{
	D3D_PRIMITIVE_TOPOLOGY D3DPrimitiveTopology(EPrimitiveTopology topology, uint8_t numControlPatches)
	{
		switch (topology)
		{
		case EPrimitiveTopology::None:
			return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		case EPrimitiveTopology::PointList:
			return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		case EPrimitiveTopology::LineList:
			return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		case EPrimitiveTopology::LineList_Adj:
			return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
		case EPrimitiveTopology::LineStrip:
			return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case EPrimitiveTopology::LineStrip_Adj:
			return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
		case EPrimitiveTopology::TriangleList:
			return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case EPrimitiveTopology::TriangleList_Adj:
			return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
		case EPrimitiveTopology::TriangleStrip:
			return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		case EPrimitiveTopology::TriangleStrip_Adj:
			return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
		case EPrimitiveTopology::PatchList:
		{
			assert(numControlPatches > 1 && numControlPatches <= 32);
			return D3D_PRIMITIVE_TOPOLOGY(D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST + (numControlPatches - 1));
		}
		default:
			return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		}
	}

	D3D12_PRIMITIVE_TOPOLOGY_TYPE D3DPrimitiveTopologyType(EPrimitiveTopology topology)
	{
		switch (topology)
		{
		case EPrimitiveTopology::None:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
		case EPrimitiveTopology::PointList:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		case EPrimitiveTopology::LineList:
		case EPrimitiveTopology::LineList_Adj:
		case EPrimitiveTopology::LineStrip:
		case EPrimitiveTopology::LineStrip_Adj:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		case EPrimitiveTopology::TriangleList:
		case EPrimitiveTopology::TriangleList_Adj:
		case EPrimitiveTopology::TriangleStrip:
		case EPrimitiveTopology::TriangleStrip_Adj:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		case EPrimitiveTopology::PatchList:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		default:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
		}
	}
}