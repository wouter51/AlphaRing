#include "../mcc/mcc_manager.h"
#include "../imgui/imgui_manager.h"
#include "../d3d11/d3d11_manager.h"
#include <libmcc/libmcc.h>

#include "../halo3/halo3.h"

using namespace libmcc;

LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY:
    case WM_QUIT: 
    case WM_CLOSE: {
        mcc_manager()->detach();
        break;
    }
    case WM_KEYDOWN: {
        switch (wParam) {
        case VK_F4: {
            c_critical_section cs(_critical_section_imgui);
            if (!imgui_manager()->initialized())
                break;

            imgui_manager()->set_visibility(!imgui_manager()->get_visibility());
            imgui_manager()->reset_cursor_position();

            break;
        }
        default:
            break;
        }
        break;
    }
    default: {
		break;
    }
    }

    do {
        c_critical_section cs(_critical_section_imgui);

        if (!imgui_manager()->initialized() || !imgui_manager()->get_visibility())
            break;

        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        if (imgui_manager()->want_capture_cursor())
            return true;

    } while(false);

	return CallWindowProc(d3d11_manager()->wndproc(), hWnd, msg, wParam, lParam);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        mcc_manager()->attach(GetModuleHandleA(nullptr));
    }

    return true;
}
