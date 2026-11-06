#pragma once

#include <imgui.h>
#include <cmath>

inline void drawDot(
    ImDrawList* draw_list,
    ImVec2 center,
    int size,
    ImU32 color
) {
    // Outline color (dark gray with same alpha as input)
    ImU32 outlineColor = IM_COL32(
        48,
        48,
        48,
        (color >> 24) & 0xFF
    );

    int outlineRadius = size + 1;

    // OUTLINE (scanline circle)
    for (int dy = -outlineRadius; dy <= outlineRadius; ++dy)
    {
        int dx = (int)std::sqrt(outlineRadius * outlineRadius - dy * dy);

        draw_list->AddLine(
            ImVec2(center.x - dx, center.y + dy),
            ImVec2(center.x + dx, center.y + dy),
            outlineColor
        );
    }

    // FILL (scanline circle)
    for (int dy = -size; dy <= size; ++dy)
    {
        int dx = (int)std::sqrt(size * size - dy * dy);

        draw_list->AddLine(
            ImVec2(center.x - dx, center.y + dy),
            ImVec2(center.x + dx, center.y + dy),
            color
        );
    }
}