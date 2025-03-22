#pragma once

#include "../common.h"
#include <d3d11.h>

inline HMODULE g_d3d11_module;

typedef long (STDMETHODCALLTYPE* t_D3D11CreateDeviceAndSwapChain)(
	IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT,
	const D3D_FEATURE_LEVEL*, UINT, UINT,
	const DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D11Device**,
	D3D_FEATURE_LEVEL*, ID3D11DeviceContext**);

inline t_D3D11CreateDeviceAndSwapChain g_D3D11CreateDeviceAndSwapChain;

int d3d11_get_vftables(IDXGISwapChainVtbl** ppChainVtbl);
