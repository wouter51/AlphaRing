#include "d3d11_impl.h"

#include "d3d11_manager.h"

#include "../imgui/imgui_manager.h"

#define VODEF static void STDMETHODCALLTYPE
#define HRDEF static HRESULT STDMETHODCALLTYPE 

VODEF DrawIndexed(ID3D11DeviceContext* This, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
VODEF OMSetRenderTargets(ID3D11DeviceContext* This, UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView);

ID3D11DeviceContextVtbl g_context_vtbl {
	//.DrawIndexed = DrawIndexed,
	//.OMSetRenderTargets = OMSetRenderTargets,
};

VODEF DrawIndexed(ID3D11DeviceContext* This, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation) {
	D3D11_PRIMITIVE_TOPOLOGY topology;

	// won't need cs
	auto wireframe = d3d11_manager()->get_wireframe();

	if (wireframe) {
		This->lpVtbl->IAGetPrimitiveTopology(This, &topology);

		This->lpVtbl->IASetPrimitiveTopology(This, D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	}

	g_context_vtbl_original.DrawIndexed(This, IndexCount, StartIndexLocation, BaseVertexLocation);

	if (wireframe) {
		This->lpVtbl->IASetPrimitiveTopology(This, topology);
	}
}

VODEF OMSetRenderTargets(ID3D11DeviceContext* This, UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView) {
	g_context_vtbl_original.OMSetRenderTargets(This, NumViews, ppRenderTargetViews, pDepthStencilView);
}

HRDEF Present(IDXGISwapChain* This, UINT SyncInterval, UINT Flags);
HRDEF ResizeBuffers(IDXGISwapChain* This, 
	UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);

IDXGISwapChainVtbl g_swapchain_vtbl{
	.Present = Present,
	.ResizeBuffers = ResizeBuffers
};

#include <libmcc/libmcc.h>
using namespace libmcc;

HRDEF Present(IDXGISwapChain* This, UINT SyncInterval, UINT Flags) {
	if (imgui_manager()->get_game_render()) {
		imgui_manager()->set_game_render(false);
	} else if (!d3d11_manager()->check_swap_chain(This) && !imgui_manager()->begin_frame()) {
		c_imgui_render::main();
		imgui_manager()->end_frame();
	}

	return g_swapchain_vtbl_original.Present(This, SyncInterval, Flags);
}

HRDEF ResizeBuffers(IDXGISwapChain* This,
	UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
	// Check swap chain
	d3d11_manager()->check_swap_chain(This);

	imgui_manager()->destroy_view();

	auto result = g_swapchain_vtbl_original.ResizeBuffers(This, BufferCount, Width, Height, NewFormat, SwapChainFlags);

	imgui_manager()->create_view();

	return result;
}