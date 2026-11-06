#pragma once

namespace CXboxColorMapping {
    static constexpr int kColHalo4   = 0;
    static constexpr int kColReach   = 1;
    static constexpr int kColHalo3   = 2;
    static constexpr int kColODST    = 3;
    static constexpr int kColHalo2   = 4;
    static constexpr int kColHalo2A  = 5;
    static constexpr int kColHalo1   = 6;
    static constexpr int kColorCount = 30;

    // Returns the game-specific color identifier string (e.g. "H3_COLOR0_STEEL").
    // colorIndex: 0 to kColorCount-1 (AlphaRing row index from MenuState)
    // game: CGameGlobal::eGame int value
    const char* GetColorString(int colorIndex, int game);

    // Returns the game-specific numeric color index for PlayerModelPrimary/Secondary/TertiaryColorIndex.
    // Parses the embedded number from the string (e.g. 12 from "H3_COLOR12_SAGE").
    // Falls back to colorIndex for Reach and ODST which have no embedded number.
    int GetColorIndex(int colorIndex, int game);

    // Returns the customization_item array index for PlayerModelPrimary/Secondary/TertiaryColor.
    // Each game stores its color entries at a different offset in the shared array.
    int GetColorItemIndex(int colorIndex, int game);
}
