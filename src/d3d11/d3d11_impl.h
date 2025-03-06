#pragma once

#include "d3d11_util.h"

extern ID3D11DeviceContextVtbl g_context_vtbl;
inline ID3D11DeviceContextVtbl g_context_vtbl_original;
extern IDXGISwapChainVtbl g_swapchain_vtbl;
inline IDXGISwapChainVtbl g_swapchain_vtbl_original;
