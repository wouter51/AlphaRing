#pragma once

#include "../common.h"

#include "d3d11_impl.h"

class c_d3d11_manager : public i_manager {
public:
	int initialize() override;
	int shutdown() override;

	inline ID3D11Device* device() const {return m_device;}
	inline ID3D11DeviceContext* context() const { return m_context; }
	inline IDXGISwapChain* swapchain() const { return m_swapchain; }
	inline HWND hwnd() const { return m_hwnd; }
	inline WNDPROC wndproc() const { return m_wndproc; }

	int check_swap_chain(IDXGISwapChain* pSwapChain);

	inline bool get_wireframe() const { return m_wireframe; }
	inline void set_wireframe(bool wireframe) { m_wireframe = wireframe; }

private:
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;
	IDXGISwapChain* m_swapchain;
	HWND m_hwnd;
	WNDPROC m_wndproc;

	bool m_wireframe = false;

	ID3D11DeviceVtbl* m_device_vtbl;
	ID3D11DeviceContextVtbl* m_context_vtbl;
	IDXGISwapChainVtbl* m_swapchain_vtbl;
};

inline c_d3d11_manager g_d3d11_manager;

inline c_d3d11_manager* d3d11_manager() {
	return &g_d3d11_manager;
}
