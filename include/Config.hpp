/** \file Config.hpp
 *
 *  \brief Declares various constant maps and such for rom-specific info. May
 *         well be in external files someday, but this works for now.
 *
 */

#pragma once

#include <map>
#include <string>

namespace Config {
    enum class Region {
        UNKNOWN,
        NTSC,
        PAL,
        US,
        JP,
        EU,
    };

    enum class Game {
        UNKNOWN,
        Ocarina,
        Majora,
    };

    enum class Version {
        UNKNOWN,
        OOT_NTSC_1_0,
        OOT_NTSC_1_1,
        OOT_NTSC_1_2,
        OOT_PAL_1_0,
        OOT_PAL_1_1,
        OOT_MQ_DEBUG,
        MM_JP_1_0,
        MM_JP_1_1,
        MM_US,
        MM_EU_1_0,
        MM_EU_1_1,
        MM_DEBUG,
    };

    std::string vDisplayStr(Version v);
    std::string vFileStr(Version v);

    Game getGame(Version v);
    Region getRegion(Version v);

    enum class Language {
        JP,
        EN,
        DE,
        FR,
        ES,
    };

    std::string langString(Language L);
}