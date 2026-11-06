#pragma once

#include "CXboxRect.hpp"
#include "CXboxText.hpp"

#include <imgui.h>
#include <optional>

void drawPage(
    int menuPosX,
    int menuPosY,
    int menuWidth,
    int menuHeight,
    const char* label,
    bool selected,
    ImU8 opacity,
    ImFont* font,
    float fontSize,
    std::optional<float> margin = 0.0f)
{
    const int tabWidth = menuWidth / 9;
    const float offset = margin.value_or(0.0f);

    if (selected)
    {
        drawGradientRect(
            menuPosX - tabWidth,
            menuPosY,
            tabWidth,
            menuHeight,
            IM_COL32(255, 255, 255, 255),
            IM_COL32(224, 223, 222, 255),
            true
        );

        drawText90(
            menuPosX - tabWidth,
            menuPosY - menuHeight * 0.25f + offset,
            tabWidth,
            menuHeight,
            font,
            fontSize,
            IM_COL32(78, 81, 86, opacity),
            label
        );

        return;
    }

    drawGradientRect(
        menuPosX - tabWidth,
        menuPosY,
        tabWidth,
        menuHeight,
        IM_COL32(73, 87, 99, opacity),
        IM_COL32(66, 76, 86, opacity),
        true
    );

    drawText90(
        menuPosX - tabWidth,
        menuPosY - menuHeight * 0.25f + offset,
        tabWidth,
        menuHeight,
        font,
        fontSize,
        IM_COL32(206, 215, 222, opacity),
        label
    );
}