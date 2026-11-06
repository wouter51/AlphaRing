#pragma once

#include <imgui.h>

void drawGradientRect(
    int x,
    int y,
    int w,
    int h,
    ImU32 c1,
    ImU32 c2,
    bool fill)
{
    ImDrawList* draw = ImGui::GetForegroundDrawList();

    if (fill)
    {
        draw->AddRectFilledMultiColor(
            ImVec2(x, y),
            ImVec2(x + w, y + h),
            c1, // top-left
            c1, // top-right
            c2, // bottom-right
            c2  // bottom-left
        );
    }
    else
    {
        draw->AddRect(
            ImVec2(x, y),
            ImVec2(x + w, y + h),
            c1
        );
    }
}