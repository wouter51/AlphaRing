#pragma once

#include <imgui.h>
#include <optional>

#include "../CXboxMenu.hpp"
#include "CXboxRect.hpp"
#include "CXboxText.hpp"
#include "CXboxDot.hpp"


inline void drawButton(
    int x,
    int y,
    int w,
    int h,
    ImFont* font,
    float fontSize,
    bool selected,
    ImU8 opacity,
    const char* text,
    OptionType type,
    int state,
    std::optional<ImU32> colorValue = std::nullopt
) {
    const int buttonSize = 10;
    const int borderSize = 5;
    const int textHeight = 0;

    auto col = [&](int r, int g, int b) -> ImU32 {
        return IM_COL32(r, g, b, opacity);
    };

    const ImU32 borderColor         = col(224, 223, 222);
    const ImU32 buttonColor         = col(255, 255, 255);
    const ImU32 buttonColorSelected = col(9, 117, 6);

    const ImU32 textColor           = col(0, 0, 0);
    const ImU32 textColorSelected   = col(255, 255, 255);

    const ImU32* currentButtonColor = &buttonColor;
    const ImU32* currentTextColor   = &textColor;

    int width  = w - buttonSize;
    int height = h / 7;

    int buttonX = x + borderSize;
    int buttonY = y + borderSize;

    int borderX = buttonX - borderSize;
    int borderY = buttonY - borderSize;

    int borderW = width + 2 * borderSize;
    int borderH = height + 2 * borderSize;

    int textX = x;
    int textY = y + textHeight;

    if (selected) {
        currentButtonColor = &buttonColorSelected;
        currentTextColor   = &textColorSelected;
    }

    ImDrawList* draw = ImGui::GetForegroundDrawList();

    // ----------------------------
    // Increment / Decrement / PointerDisplay
    // ----------------------------
    if (type == OptionType::Increment ||
        type == OptionType::Decrement ||
        type == OptionType::PointerDisplay)
    {
        drawGradientRect(borderX, borderY, borderW / 3, borderH, borderColor, borderColor, true);
        drawGradientRect(buttonX, buttonY, width / 3, height, *currentButtonColor, *currentButtonColor, true);

        const char* symbol =
            (type == OptionType::Increment) ? "+" :
            (type == OptionType::Decrement) ? "-" :
            text;

        drawText(textX, textY, width / 3, height, font, fontSize, *currentTextColor, symbol);
        return;
    }

    // ----------------------------
    // Boolean toggle
    // ----------------------------
    if (type == OptionType::Boolean)
    {
        drawGradientRect(borderX, borderY, borderW, borderH, borderColor, borderColor, true);
        drawGradientRect(buttonX, buttonY, width, height, *currentButtonColor, *currentButtonColor, true);

        int checkSize = height / 2;
        int checkX = buttonX + width - height + (height - checkSize) / 2;
        int checkY = buttonY + (height - checkSize) / 2;
        ImU32 checkColor = (state == 1) ? col(44, 200, 44) : col(150, 150, 150);
        draw->AddRectFilled(
            ImVec2((float)checkX, (float)checkY),
            ImVec2((float)(checkX + checkSize), (float)(checkY + checkSize)),
            checkColor
        );

        drawText(textX, textY, width, height, font, fontSize, *currentTextColor, text);
        return;
    }

    // ----------------------------
    // Team toggle
    // ----------------------------
    if (type == OptionType::TeamToggle)
    {
        drawGradientRect(borderX, borderY, borderW, borderH, borderColor, borderColor, true);
        drawGradientRect(buttonX, buttonY, width, height, *currentButtonColor, *currentButtonColor, true);

        if (state == 0)
        {
            drawDot(draw,
                ImVec2(textX + width / 3, textY + height / 2),
                height / 4,
                IM_COL32(212, 44, 44, opacity)
            );

            drawText(textX, textY, width, height, font, fontSize, *currentTextColor, "Red Team");
        }
        else
        {
            drawDot(draw,
                ImVec2(textX + width / 3, textY + height / 2),
                height / 4,
                IM_COL32(44, 78, 212, opacity)
            );

            drawText(textX, textY, width, height, font, fontSize, *currentTextColor, "Blue Team");
        }

        return;
    }

    // ----------------------------
    // Default
    // ----------------------------
    {
        drawGradientRect(borderX, borderY, borderW, borderH, borderColor, borderColor, true);
        drawGradientRect(buttonX, buttonY, width, height, *currentButtonColor, *currentButtonColor, true);

        if (colorValue.has_value())
        {
            drawDot(draw,
                ImVec2(textX + width / 4, textY + height / 2),
                height / 4,
                colorValue.value()
            );
        }

        drawText(textX, textY, width, height, font, fontSize, *currentTextColor, text);
    }
}