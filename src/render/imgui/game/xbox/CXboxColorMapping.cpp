#include "CXboxColorMapping.h"

#include <cctype>
#include <cstdlib>

namespace CXboxColorMapping {

// [colorIndex][column] — columns: {H4, HR, H3, ODST, H2, H2A, H1}
static const char* const k_table[kColorCount][7] = {
    // 0  Steel
    // {"H4_Color0_Steel",     "HR_Color_Steel",    "H3_COLOR0_STEEL",     "ODST_COLOR_BLACK",    "H2_Color_2",  "H2A_Color_2",  "H1_Color_2"},
    {"H1_Color_2", "H2_Color_2", "H3_COLOR0_STEEL",  "H4_Color0_Steel", "H2A_Color_2",  "ODST_COLOR_BLACK", "HR_Color_Steel"},
    // 1  Silver
    // {"H4_Color1_Silver",    "HR_Color_Silver",   "H3_COLOR1_SILVER",    "ODST_COLOR_GRAY",     "H2_Color_2",  "H2A_Color_1",  "H1_Color_5"},
    {"H1_Color_5", "H2_Color_2", "H3_COLOR1_SILVER", "H4_Color1_Silver", "H2A_Color_1",  "ODST_COLOR_GRAY",  "HR_Color_Silver"},
    // 2  White
    // {"H4_Color2_White",     "HR_Color_White",    "H3_COLOR2_WHITE",     "ODST_COLOR_SNOW",     "H2_Color_1",  "H2A_Color_3",  "H1_Color_1"},
    {"H1_Color_1", "H2_Color_1", "H3_COLOR2_WHITE",  "H4_Color2_White", "H2A_Color_3",  "ODST_COLOR_SNOW",  "HR_Color_White"},
    // 3  Brown
    // {"H4_Color3_Brown",     "HR_Color_Brown",    "H3_COLOR27_BROWN",    "ODST_COLOR_BROWN",    "H2_Color_17", "H2A_Color_4",  "H1_Color_15"},
    {"H1_Color_15", "H2_Color_17", "H3_COLOR27_BROWN", "H4_Color3_Brown", "H2A_Color_4",  "ODST_COLOR_BROWN",  "HR_Color_Brown"},
    // 4  Tan
    // {"H4_Color4_Tan",       "HR_Color_Tan",      "H3_COLOR28_TAN",      "ODST_COLOR_TAN",      "H2_Color_18", "H2A_Color_5",  "H1_Color_16"},
    {"H1_Color_16", "H2_Color_18", "H3_COLOR28_TAN", "H4_Color4_Tan", "H2A_Color_5",  "ODST_COLOR_TAN",  "HR_Color_Tan"},
    // 5  Khaki
    // {"H4_Color5_Khaki",     "HR_Color_Khaki",    "H3_COLOR29_KHAKI",    "ODST_COLOR_KHAKI",    "H2_Color_18", "H2A_Color_6",  "H1_Color_16"},
    {"H1_Color_16", "H2_Color_18", "H3_COLOR29_KHAKI", "H4_Color5_Khaki", "H2A_Color_6",  "ODST_COLOR_KHAKI",  "HR_Color_Khaki"},
    // 6  Sage
    // {"H4_Color6_Sage",      "HR_Color_Sage",     "H3_COLOR12_SAGE",     "ODST_COLOR_SAGE",     "H2_Color_8",  "H2A_Color_7",  "H1_Color_14"},
    {"H1_Color_14", "H2_Color_8", "H3_COLOR12_SAGE", "H4_Color6_Sage", "H2A_Color_7",  "ODST_COLOR_SAGE",  "HR_Color_Sage"},
    // 7  Olive
    // {"H4_Color7_Olive",     "HR_Color_Olive",    "H3_COLOR14_OLIVE",    "ODST_COLOR_OLIVE",    "H2_Color_6",  "H2A_Color_8",  "H1_Color_14"},
    {"H1_Color_14", "H2_Color_6", "H3_COLOR14_OLIVE", "H4_Color7_Olive", "H2A_Color_8",  "ODST_COLOR_OLIVE",  "HR_Color_Olive"},
    // 8  Drab
    // {"H4_Color8_Drab",      "HR_Color_Drab",     "H3_COLOR13_GREEN",    "ODST_COLOR_DRAB",     "H2_Color_7",  "H2A_Color_9",  "H1_Color_7"},
    {"H1_Color_7", "H2_Color_7", "H3_COLOR13_GREEN", "H4_Color8_Drab", "H2A_Color_9",  "ODST_COLOR_DRAB",  "HR_Color_Drab"},
    // 9  Forest
    // {"H4_Color9_Forest",    "HR_Color_Forest",   "H3_COLOR12_SAGE",     "ODST_COLOR_FOREST",   "H2_Color_8",  "H2A_Color_10", "H1_Color_14"},
    {"H1_Color_14", "H2_Color_8", "H3_COLOR12_SAGE", "H4_Color9_Forest", "H2A_Color_10",  "ODST_COLOR_FOREST",  "HR_Color_Forest"},
    // 10 Green
    // {"H4_Color10_Green",    "HR_Color_Green",    "H3_COLOR13_GREEN",    "ODST_COLOR_GREEN",    "H2_Color_7",  "H2A_Color_11", "H1_Color_14"},
    {"H1_Color_14", "H2_Color_7", "H3_COLOR13_GREEN", "H4_Color10_Green", "H2A_Color_11",  "ODST_COLOR_GREEN",  "HR_Color_Green"},
    // 11 Sea Foam
    // {"H4_Color11_SeaFoam",  "HR_Color_SeaFoam",  "H3_COLOR15_TEAL",     "ODST_COLOR_SEA_FOAM", "H2_Color_10", "H2A_Color_12", "H1_Color_13"},
    {"H1_Color_13", "H2_Color_10", "H3_COLOR15_TEAL", "H4_Color11_SeaFoam", "H2A_Color_12",  "ODST_COLOR_SEA_FOAM",  "HR_Color_SeaFoam"},
    // 12 Teal
    // {"H4_Color12_Teal",     "HR_Color_Teal",     "H3_COLOR15_TEAL",     "ODST_COLOR_TEAL",     "H2_Color_10", "H2A_Color_13", "H1_Color_13"},
    {"H1_Color_13", "H2_Color_10", "H3_COLOR15_TEAL", "H4_Color12_Teal", "H2A_Color_13",  "ODST_COLOR_TEAL",  "HR_Color_Teal"},
    // 13 Aqua
    // {"H4_Color13_Aqua",     "HR_Color_Aqua",     "H3_COLOR16_AQUA",     "ODST_COLOR_AQUA",     "H2_Color_9",  "H2A_Color_14", "H1_Color_10"},
    {"H1_Color_10", "H2_Color_9", "H3_COLOR16_AQUA", "H4_Color13_Aqua", "H2A_Color_14",  "ODST_COLOR_AQUA",  "HR_Color_Aqua"},
    // 14 Cyan
    // {"H4_Color14_Cyan",     "HR_Color_Cyan",     "H3_COLOR17_CYAN",     "ODST_COLOR_CYAN",     "H2_Color_9",  "H2A_Color_15", "H1_Color_10"},
    {"H1_Color_10", "H2_Color_9", "H3_COLOR17_CYAN", "H4_Color14_Cyan", "H2A_Color_15",  "ODST_COLOR_CYAN",  "HR_Color_Cyan"},
    // 15 Blue
    // {"H4_Color15_Blue",     "HR_Color_Blue",     "H3_COLOR18_BLUE",     "ODST_COLOR_BLUE",     "H2_Color_12", "H2A_Color_16", "H1_Color_4"},
    {"H1_Color_4", "H2_Color_12", "H3_COLOR18_BLUE", "H4_Color15_Blue", "H2A_Color_16",  "ODST_COLOR_BLUE",  "HR_Color_Blue"},
    // 16 Cobalt
    {"H4_Color16_Cobalt",   "HR_Color_Cobalt",   "H3_COLOR19_COBALT",   "ODST_COLOR_COBALT",   "H2_Color_11", "H2A_Color_17", "H1_Color_11"},
    // 17 Ice
    // {"H4_Color17_Ice",      "HR_Color_Ice",      "H3_COLOR20_SAPHIRE",  "ODST_COLOR_ICE",      "H2_Color_11", "H2A_Color_18", "H1_Color_11"},
    {"H1_Color_11", "H2_Color_11", "H3_COLOR20_SAPHIRE", "H4_Color17_Ice", "H2A_Color_18",  "ODST_COLOR_ICE",  "HR_Color_Ice"},
    // 18 Violet
    // {"H4_Color18_Violet",   "HR_Color_Violet",   "H3_COLOR21_VIOLET",   "ODST_COLOR_VIOLET",   "H2_Color_13", "H2A_Color_19", "H1_Color_9"},
    {"H1_Color_9", "H2_Color_13", "H3_COLOR21_VIOLET", "H4_Color18_Violet", "H2A_Color_19",  "ODST_COLOR_VIOLET",  "HR_Color_Violet"},
    // 19 Orchid
    // {"H4_Color19_Orchid",   "HR_Color_Orchid",   "H3_COLOR22_ORCHID",   "ODST_COLOR_PINK",     "H2_Color_15", "H2A_Color_20", "H1_Color_8"},
    {"H1_Color_8", "H2_Color_15", "H3_COLOR22_ORCHID", "H4_Color19_Orchid", "H2A_Color_20",  "ODST_COLOR_PINK",  "HR_Color_Orchid"},
    // 20 Lavender
    // {"H4_Color20_Lavender", "HR_Color_Lavender", "H3_COLOR23_LAVENDER", "ODST_COLOR_LAVENDER", "H2_Color_14", "H2A_Color_21", "H1_Color_9"},
    {"H1_Color_9", "H2_Color_14", "H3_COLOR23_LAVENDER", "H4_Color20_Lavender", "H2A_Color_21",  "ODST_COLOR_LAVENDER",  "HR_Color_Lavender"},
    // 21 Maroon
    // {"H4_Color21_Maroon",   "HR_Color_Maroon",   "H3_COLOR3_RED",       "ODST_COLOR_MAROON",   "H2_Color_16", "H2A_Color_22", "H1_Color_17"},
    {"H1_Color_17", "H2_Color_16", "H3_COLOR3_RED", "H4_Color21_Maroon", "H2A_Color_22",  "ODST_COLOR_MAROON",  "HR_Color_Maroon"},
    // 22 Brick
    // {"H4_Color22_Brick",    "HR_Color_Brick",    "H3_COLOR3_RED",       "ODST_COLOR_BRICK",    "H2_Color_3",  "H2A_Color_23", "H1_Color_3"},
    {"H1_Color_3", "H2_Color_3", "H3_COLOR3_RED", "H4_Color22_Brick", "H2A_Color_23",  "ODST_COLOR_BRICK",  "HR_Color_Brick"},
    // 23 Rose
    // {"H4_Color23_Rose",     "HR_Color_Rose",     "H3_COLOR5_SALMON",    "ODST_COLOR_ROSE",     "H2_Color_15", "H2A_Color_24", "H1_Color_8"},
    {"H1_Color_8", "H2_Color_15", "H3_COLOR5_SALMON", "H4_Color23_Rose", "H2A_Color_24",  "ODST_COLOR_ROSE",  "HR_Color_Rose"},
    // 24 Rust
    // {"H4_Color24_Rust",     "HR_Color_Rust",     "H3_COLOR6_ORANGE",    "ODST_COLOR_COCOA",    "H2_Color_4",  "H2A_Color_25", "H1_Color_12"},
    {"H1_Color_12", "H2_Color_4", "H3_COLOR6_ORANGE", "H4_Color24_Rust", "H2A_Color_25",  "ODST_COLOR_COCOA",  "HR_Color_Rust"},
    // 25 Coral
    // {"H4_Color25_Coral",    "HR_Color_Coral",    "H3_COLOR7_CORAL",     "ODST_COLOR_CORAL",    "H2_Color_4",  "H2A_Color_26", "H1_Color_12"},
    {"H1_Color_12", "H2_Color_4", "H3_COLOR7_CORAL", "H4_Color25_Coral", "H2A_Color_26",  "ODST_COLOR_CORAL",  "HR_Color_Coral"},
    // 26 Peach
    // {"H4_Color26_Peach",    "HR_Color_Peach",    "H3_COLOR8_PEACH",     "ODST_COLOR_ROSE",     "H2_Color_15", "H2A_Color_27", "H1_Color_18"},
    {"H1_Color_18", "H2_Color_15", "H3_COLOR8_PEACH", "H4_Color26_Peach", "H2A_Color_27",  "ODST_COLOR_ROSE",  "HR_Color_Peach"},
    // 27 Gold
    // {"H4_Color27_Gold",     "HR_Color_Gold",     "H3_COLOR9_GOLD",      "ODST_COLOR_GOLD",     "H2_Color_5",  "H2A_Color_28", "H1_Color_16"},
    {"H1_Color_16", "H2_Color_5", "H3_COLOR9_GOLD", "H4_Color27_Gold", "H2A_Color_28",  "ODST_COLOR_GOLD",  "HR_Color_Gold"},
    // 28 Yellow
    // {"H4_Color28_Yellow",   "HR_Color_Yellow",   "H3_COLOR10_YELLOW",   "ODST_COLOR_SAND",     "H2_Color_5",  "H2A_Color_29", "H1_Color_6"},
    {"H1_Color_6", "H2_Color_5", "H3_COLOR10_YELLOW", "H4_Color28_Yellow", "H2A_Color_29",  "ODST_COLOR_SAND",  "HR_Color_Yellow"},
    // 29 Pale
    // {"H4_Color28_Yellow",   "HR_Color_Pale",     "H3_COLOR11_PALE",     "ODST_COLOR_DESERT",   "H2_Color_5",  "H2A_Color_29", "H1_Color_6"},
    {"H1_Color_6", "H2_Color_5", "H3_COLOR11_PALE", "H4_Color28_Yellow", "H2A_Color_29",  "ODST_COLOR_DESERT",  "HR_Color_Pale"},
};

// Maps CGameGlobal::eGame int values to table columns.
// Halo1=0, Halo2=1, Halo3=2, Halo4=3, GroundHog=4, Halo3ODST=5, HaloReach=6
// static int GameToColumn(int game) {
//     switch (game) {
//         case 3: return kColHalo4;
//         case 6: return kColReach;
//         case 2: return kColHalo3;
//         case 5: return kColODST;
//         case 1: return kColHalo2;
//         case 0: return kColHalo1;
//         case 4: return kColHalo3; // GroundHog → H3 fallback
//         default: return kColHalo3;
//     }
// }

// customization_item base index per eGame value.
// For H1/H2/H2A the colors are 1-indexed in both the string and the array, so
// the formula is base + embedded_number (same as base - 1 + 1-based index).
// For H3/H4 the colors are 0-indexed, so the formula is also base + embedded_number.
// For HR/ODST there is no embedded number; HR colors are in colorIndex order so
// base + colorIndex works. ODST colors are out of order and need a separate table.
static const int kGameColorItemBase[7] = {
    283,   // Halo1    H1_Color_1  @ ci 284, colors 1-indexed
    488,   // Halo2    H2_Color_1  @ ci 489
    1233,  // Halo3    H3_COLOR0   @ ci 1233
    2025,  // Halo4    H4_Color0   @ ci 2025
    548,   // GroundHog  → H2A_Color_1 @ ci 549
    -1,    // Halo3ODST  → use kODSTColorItems
    202,   // HaloReach  HR_Color_Steel @ ci 202
};

// customization_item indices for ODST colors by colorIndex (0-29).
// ODST entries are not contiguous in colorIndex order inside the array.
static const int kODSTColorItems[kColorCount] = {
    1588, 1589, 1590, 1594, 1597, 1598,  // Steel, Silver, White, Brown, Tan, Khaki
    1603, 1604, 1605, 1606, 1607, 1608,  // Sage, Olive, Drab, Forest, Green, SeaFoam
    1609, 1610, 1611, 1612, 1613, 1614,  // Teal, Aqua, Cyan, Blue, Cobalt, Ice
    1615, 1617, 1616, 1591, 1592, 1593,  // Violet, Orchid, Lavender, Maroon, Brick, Rose
    1596, 1600, 1593, 1601, 1602, 1599,  // Rust, Coral, Peach, Gold, Yellow, Pale
};

const char* GetColorString(int colorIndex, int game) {
    if (colorIndex < 0 || colorIndex >= kColorCount || game < 0 || game > 6)
        return nullptr;
    return k_table[colorIndex][game];
}

int GetColorIndex(int colorIndex, int game) {
    const char* s = GetColorString(colorIndex, game);
    if (!s)
        return colorIndex;
    const char* last = nullptr;
    for (const char* p = s; *p; ++p) {
        if (isdigit((unsigned char)*p) && (p == s || !isdigit((unsigned char)p[-1])))
            last = p;
    }
    if (last)
        return atoi(last);
    return colorIndex;
}

int GetColorItemIndex(int colorIndex, int game) {
    if (colorIndex < 0 || colorIndex >= kColorCount || game < 0 || game > 6)
        return colorIndex;
    if (game == 5)
        return kODSTColorItems[colorIndex];
    int primary = GetColorIndex(colorIndex, game);
    return kGameColorItemBase[game] + primary;
}

} // namespace CXboxColorMapping
