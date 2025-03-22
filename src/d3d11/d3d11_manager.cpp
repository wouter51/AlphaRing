#include "d3d11_manager.h"

#include "../imgui/imgui_manager.h"

int c_d3d11_manager::initialize(){
	// Initailize d3d11
	// Load d3d11.dll
	g_d3d11_module = LoadLibraryA("d3d11.dll");

	ASSERT_HD(g_d3d11_module, "Failed to load d3d11!"); // impossible

	// Get D3D11CreateDeviceAndSwapChain
	g_D3D11CreateDeviceAndSwapChain = reinterpret_cast<t_D3D11CreateDeviceAndSwapChain>(
		GetProcAddress(g_d3d11_module, "D3D11CreateDeviceAndSwapChain"));

	ASSERT_HD(g_D3D11CreateDeviceAndSwapChain, "Failed to get D3D11CreateDeviceAndSwapChain!"); // impossible

	// Detour swapchain
	// Cannot detour the device and device_context, cause their vftables are dynamic changed
	d3d11_get_vftables(&m_swapchain_vtbl);

	vftable_manager()->create(
		m_swapchain_vtbl, 
		(const void**)&g_swapchain_vtbl, 
		sizeof(g_swapchain_vtbl), 
		(void**) &g_swapchain_vtbl_original);

	auto status = vftable_manager()->enable(m_swapchain_vtbl);

	CHECK(status == 0, "Failed to override swapchain vftable!");

	return 0;
}

int c_d3d11_manager::shutdown() {
	imgui_manager()->shutdown();

	return 0;
}

int c_d3d11_manager::check_swap_chain(IDXGISwapChain* pSwapChain) {
	DXGI_SWAP_CHAIN_DESC desc;

	if (m_swapchain == pSwapChain) {
		return 0; // Do nothing
	}

	// Update Swap Chain
	m_swapchain = pSwapChain; 

	// Destory Imgui Context 
	imgui_manager()->shutdown();

	// Get HWND
	m_swapchain->lpVtbl->GetDesc(m_swapchain, &desc);
	m_hwnd = desc.OutputWindow;

	if (m_wndproc == nullptr) {
		m_wndproc = reinterpret_cast<WNDPROC>(
			SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc)));
	}

	// Get Device
	m_swapchain->lpVtbl->GetDevice(m_swapchain, IID_ID3D11Device, (void**)&m_device);
	m_device_vtbl = m_device->lpVtbl;

	// Get Device Context
	m_device_vtbl->GetImmediateContext(m_device, &m_context);
	m_context_vtbl = m_context->lpVtbl;

	//auto wireframe_hook = hook_manager()
	//	->create(m_context_vtbl->DrawIndexed, g_context_vtbl.DrawIndexed, (void**)&g_context_vtbl_original.DrawIndexed);

	//auto result = hook_manager()
	//	->enable(wireframe_hook);

	//if (result != 0) {
	//	LOG_WARN("Wireframe is not available.");
	//}

	// Create Imgui Context
	imgui_manager()->initialize();

	return 0;
}
