/** \file Config.cpp
 *
 *  \brief Implement the configuration class
 *
 */

#include "Config.hpp"

#include <QStandardPaths>

#include <string>
#include <fstream>

namespace Config {
    std::string vDisplayStr(Version v) {
        switch (v) {
          case Version::UNKNOWN:
            return "Unknown version";
            break;

          case Version::OOT_NTSC_1_0:
            return "Ocarina of Time NTSC 1.0";
            break;

          case Version::OOT_NTSC_1_1:
            return "Ocarina of Time NTSC 1.1";
            break;

          case Version::OOT_NTSC_1_2:
            return "Ocarina of Time NTSC 1.2";
            break;

          case Version::OOT_PAL_1_0:
            return "Ocarina of Time PAL 1.0";
            break;

          case Version::OOT_PAL_1_1:
            return "Ocarina of Time PAL 1.1";
            break;

          case Version::OOT_MQ_DEBUG:
            return "Ocarina of Time MQ Debug";
            break;

          case Version::MM_JP_1_0:
            return "Majora's Mask JP 1.0";
            break;

          case Version::MM_JP_1_1:
            return "Majora's Mask JP 1.1";
            break;

          case Version::MM_US:
            return "Majora's Mask US";
            break;

          case Version::MM_EU_1_0:
            return "Majora's Mask EU 1.0";
            break;

          case Version::MM_EU_1_1:
            return "Majora's Mask EU 1.1";
            break;

          case Version::MM_DEBUG:
            return "Majora's Mask Debug";
            break;
        }
    }

    std::string vFileStr(Version v) {
        switch (v) {
          case Version::UNKNOWN:
            return "UNKNOWN";
            break;

          case Version::OOT_NTSC_1_0:
            return "oot_ntsc_1.0";
            break;

          case Version::OOT_NTSC_1_1:
            return "oot_ntsc_1.1";
            break;

          case Version::OOT_NTSC_1_2:
            return "oot_ntsc_1.2";
            break;

          case Version::OOT_PAL_1_0:
            return "oot_pal_1.0";
            break;

          case Version::OOT_PAL_1_1:
            return "oot_pal_1.1";
            break;

          case Version::OOT_MQ_DEBUG:
            return "oot_mq_debug";
            break;

          case Version::MM_JP_1_0:
            return "mm_jp_1.0";
            break;

          case Version::MM_JP_1_1:
            return "mm_jp_1.1";
            break;

          case Version::MM_US:
            return "mm_us";
            break;

          case Version::MM_EU_1_0:
            return "mm_eu_1.0";
            break;

          case Version::MM_EU_1_1:
            return "mm_eu_1.1";
            break;

          case Version::MM_DEBUG:
            return "mm_debug";
            break;
        }
    }

    Game getGame(Version v) {
        switch (v) {
          case Version::OOT_NTSC_1_0:
          case Version::OOT_NTSC_1_1:
          case Version::OOT_NTSC_1_2:
          case Version::OOT_PAL_1_0:
          case Version::OOT_PAL_1_1:
          case Version::OOT_MQ_DEBUG:
            return Game::Ocarina;
            break;

          case Version::MM_JP_1_0:
          case Version::MM_JP_1_1:
          case Version::MM_US:
          case Version::MM_EU_1_0:
          case Version::MM_EU_1_1:
          case Version::MM_DEBUG:
            return Game::Majora;
            break;

          case Version::UNKNOWN:
            return Game::UNKNOWN;
            break;
        }
    }

    Region getRegion(Version v) {
        switch (v) {
          case Version::OOT_NTSC_1_0:
          case Version::OOT_NTSC_1_1:
          case Version::OOT_NTSC_1_2:
            return Region::NTSC;
            break;

          case Version::OOT_PAL_1_0:
          case Version::OOT_PAL_1_1:
          case Version::OOT_MQ_DEBUG:
            return Region::PAL;
            break;

          case Version::MM_JP_1_0:
          case Version::MM_JP_1_1:
            return Region::JP;
            break;

          case Version::MM_US:
            return Region::US;
            break;

          case Version::MM_EU_1_0:
          case Version::MM_EU_1_1:
          case Version::MM_DEBUG:
            return Region::EU;
            break;

          case Version::UNKNOWN:
            return Region::UNKNOWN;
            break;
        }
    }

    std::string langString(Language L) {
        switch (L) {
          case Language::JP:
            return "Japanese";
            break;

          case Language::EN:
            return "English";
            break;

          case Language::DE:
            return "German";
            break;

          case Language::FR:
            return "French";
            break;

          case Language::ES:
            return "Spanish";
            break;
        }
    }
}