#pragma once

#include <imgui.h>
#include <cmath>

void drawText(
    int x,
    int y,
    int w,
    int h,
    ImFont* font,
    float fontSize,
    ImU32 textColor,
    const char* text)
{
    ImDrawList* draw = ImGui::GetForegroundDrawList();

    ImVec2 textSize = font->CalcTextSizeA(
        fontSize,
        FLT_MAX,
        0.0f,
        text
    );

    float textX = x + (w - textSize.x) * 0.5f;
    float textY = y + (h - textSize.y) * 0.5f;

    draw->AddText(
        font,
        fontSize,
        ImVec2(textX, textY),
        textColor,
        text
    );
}

void drawText90(
    int x,
    int y,
    int w,
    int h,
    ImFont* font,
    float fontSize,
    ImU32 textColor,
    const char* text)
{
    ImDrawList* draw = ImGui::GetForegroundDrawList();

    // Measure text
    ImVec2 textSize = font->CalcTextSizeA(
        fontSize,
        FLT_MAX,
        0.0f,
        text
    );

    // Center inside the supplied rectangle
    float textX = x + (w - textSize.x) * 0.5f;
    float textY = y + (h - textSize.y) * 0.5f;

    // Remember where the new vertices begin
    int startVertex = draw->VtxBuffer.Size;

    // Draw normally
    draw->AddText(
        font,
        fontSize,
        ImVec2(textX, textY),
        textColor,
        text
    );

    int endVertex = draw->VtxBuffer.Size;

    // Rotate around the center of the text
    ImVec2 center(
        textX + textSize.x * 0.5f,
        textY + textSize.y * 0.5f
    );

    constexpr float angle = 3.14159265358979323846f * 0.5f;
    float s = sinf(angle);
    float c = cosf(angle);

    for (int i = startVertex; i < endVertex; ++i)
    {
        ImVec2 p = draw->VtxBuffer[i].pos;

        // Translate to origin
        p.x -= center.x;
        p.y -= center.y;

        // Rotate
        float rx = p.x * c - p.y * s;
        float ry = p.x * s + p.y * c;

        // Translate back
        draw->VtxBuffer[i].pos.x = rx + center.x;
        draw->VtxBuffer[i].pos.y = ry + center.y;
    }
}