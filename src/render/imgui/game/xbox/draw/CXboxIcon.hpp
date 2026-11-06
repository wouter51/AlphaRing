#pragma once

#include <imgui.h>
#include <unordered_map>
#include <d3d11.h>
#include "../../../../d3d11/Graphics.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void drawIcon(
    ImDrawList* draw,
    float x,
    float y,
    float w,
    float h,
    const void* data,
    size_t len,
    ImU8 alpha = 255
) {
    ImVec2 pos = ImVec2(x, y);
    static std::unordered_map<const void*, ID3D11ShaderResourceView*> s_cache;
    ID3D11ShaderResourceView*& srv = s_cache[data];
    if (srv == nullptr) {
        int iw, ih, ch;
        unsigned char* pixels = stbi_load_from_memory(
            (const stbi_uc*)data, (int)len, &iw, &ih, &ch, 4);
        if (!pixels) return;

        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width            = iw;
        desc.Height           = ih;
        desc.MipLevels        = 1;
        desc.ArraySize        = 1;
        desc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage            = D3D11_USAGE_DEFAULT;
        desc.BindFlags        = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA init = {};
        init.pSysMem     = pixels;
        init.SysMemPitch = iw * 4;

        ID3D11Texture2D* tex = nullptr;
        Graphics()->pDevice->CreateTexture2D(&desc, &init, &tex);
        stbi_image_free(pixels);

        if (!tex) return;

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format                    = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels       = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;
        Graphics()->pDevice->CreateShaderResourceView(tex, &srvDesc, &srv);
        tex->Release();
    }
    if (!srv) return;
    draw->AddImage(
        (ImTextureID)srv,
        pos,
        ImVec2(pos.x + w, pos.y + h),
        ImVec2(0, 0),
        ImVec2(1, 1),
        IM_COL32(255, 255, 255, alpha)
    );
}
