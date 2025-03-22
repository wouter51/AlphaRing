#include "imgui_manager.h"

#include "../mcc/mcc_manager.h"
#include "../d3d11/d3d11_manager.h"

#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>

int c_imgui_manager::initialize() {
    ImFontConfig ftcfg;

	if (m_initialized) {
		return 0;
	}

	// create render target view for imgui
	create_view();

	// create imgui context
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(d3d11_manager()->hwnd());
	ImGui_ImplDX11_Init(d3d11_manager()->device(), d3d11_manager()->context());
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();

    // config
    io.ConfigFlags = ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_NoMouseCursorChange;
    io.MouseDrawCursor = true;

    // ini
    io.IniFilename = nullptr;
	auto ini = mcc_manager()->read_resource("imgui.ini");
    if (ini != nullptr) {
        ImGui::LoadIniSettingsFromMemory(ini->data(), ini->size());
    }

    const float scale = GetDpiForWindow(d3d11_manager()->hwnd()) * 1.0f / 96.0f;

    io.Fonts->Clear();
    ftcfg.SizePixels = 16.0f * scale;
    io.Fonts->AddFontDefault(&ftcfg);

    ImGui::GetStyle().ScaleAllSizes(scale);

	m_initialized = true;

    return 0;
}

int c_imgui_manager::shutdown() {
    size_t size;

	if (!m_initialized) {
		return 0;
	}

    // save ini
	auto ini = ImGui::SaveIniSettingsToMemory(&size);
	mcc_manager()->write_resource("imgui.ini", ini, size);

    destroy_view();
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

	m_initialized = false;
	
	return 0;
}

int c_imgui_manager::begin_frame() {
    critical_section_manager()->enter(_critical_section_imgui);
    m_cached_visibility = get_visibility();
    critical_section_manager()->leave(_critical_section_imgui);

	if (m_cached_visibility == false) {
		return 2;
	}

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

	return 0;
}

int c_imgui_manager::end_frame() {
    if (m_cached_visibility == false) {
        return 2;
    }

    auto context = d3d11_manager()->context();

	context->lpVtbl->OMSetRenderTargets(context, 1, &m_imgui_view, nullptr);

    ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return 0;
}

bool c_imgui_manager::get_visibility() {
	return m_visibility;
}

void c_imgui_manager::set_visibility(bool visibility) {
	m_visibility = visibility;
}

bool c_imgui_manager::want_capture_cursor() {
    return ImGui::GetIO().WantCaptureMouse;
}

void c_imgui_manager::reset_cursor_position() {
    POINT position;

    auto io = &ImGui::GetIO();

    if (!GetCursorPos(&position) || !ScreenToClient(d3d11_manager()->hwnd(), &position)) {
        return;
    }

    io->MousePos.x = position.x;
    io->MousePos.y = position.y;
}

int c_imgui_manager::create_view() {
    ID3D11Texture2D* back_buffer;
	auto chain = d3d11_manager()->swapchain();
    auto device = d3d11_manager()->device();

    destroy_view();
	
    chain->lpVtbl->GetBuffer(chain, 0, IID_ID3D11Texture2D, (void**)&back_buffer);
	device->lpVtbl->CreateRenderTargetView(device, (ID3D11Resource*)back_buffer, nullptr, &m_imgui_view);
	back_buffer->lpVtbl->Release(back_buffer);

    return 0;
}

int c_imgui_manager::destroy_view() {
    if (m_imgui_view != nullptr) {
        m_imgui_view->lpVtbl->Release(m_imgui_view);
        m_imgui_view = nullptr;
    }

    return 0;
}
