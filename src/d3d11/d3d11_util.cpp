#include "d3d11_util.h"

static const WNDCLASS wc {
	.style = CS_HREDRAW | CS_VREDRAW,
	.lpfnWndProc = DefWindowProc,
	.cbClsExtra = 0,
	.cbWndExtra = 0,
	.hInstance = GetModuleHandle(nullptr),
	.hIcon = nullptr,
	.hCursor = nullptr,
	.hbrBackground = nullptr,
	.lpszMenuName = nullptr,
	.lpszClassName = "ILVDM"
};

static const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

static DXGI_SWAP_CHAIN_DESC swapChainDesc {
	.BufferDesc = {
		.Width = 0,
		.Height = 0,
		.RefreshRate = {
			.Numerator = 60,
			.Denominator = 1
		},
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
		.Scaling = DXGI_MODE_SCALING_UNSPECIFIED
	},
	.SampleDesc = {
		.Count = 1,
		.Quality = 0
	},
	.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
	.BufferCount = 1,
	.OutputWindow = nullptr,
	.Windowed = TRUE,
	.SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
	.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
};

int d3d11_get_vftables(IDXGISwapChainVtbl** ppChainVtbl) {
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pContext;
	IDXGISwapChain* pChain;

	auto atom = RegisterClass(&wc);

	ASSERTF(atom != 0, "Failed to register window class!: {}", get_last_error_string());

	HWND hWnd = CreateWindow(wc.lpszClassName, wc.lpszClassName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, nullptr, nullptr, wc.hInstance, nullptr);

	ASSERT_HD(hWnd, "Failed to create window!");

	swapChainDesc.OutputWindow = hWnd;

	auto hr = g_D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
		featureLevels, 1, D3D11_SDK_VERSION, &swapChainDesc, &pChain, &pDevice, nullptr, &pContext);

	ASSERT_HR(hr, "Failed to create device and swap chain!");

	if (ppChainVtbl)	*ppChainVtbl = pChain->lpVtbl;

	pChain->lpVtbl->Release(pChain);
	pContext->lpVtbl->Release(pContext);
	pDevice->lpVtbl->Release(pDevice);

	DestroyWindow(hWnd);
	UnregisterClassA(wc.lpszClassName, wc.hInstance);

	return 0;
}
