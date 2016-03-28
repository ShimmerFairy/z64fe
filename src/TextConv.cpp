/** \file TextConv.cpp
 *
 *  \brief Implementing conversion functions
 *
 */

#include "TextConv.hpp"
#include "Shift-JIS.hpp"
#include "endian.hpp"
#include "utility.hpp"
#include "Exceptions.hpp"

#include <map>

std::vector<TextAST> readASCII_OoT(std::vector<uint8_t>::iterator & takethis) {
    std::vector<TextAST> the_list;
    bool cont = true;

    auto addlit = [&](std::string piece) {
        if (the_list.size() == 0 || !the_list.back().literalAddText(piece)) {
            the_list.emplace_back(piece);
        }
    };

    static const std::map<uint8_t, std::string> specialChar{
        { 0x7F, "‾" },
        { 0x80, "À" },
        { 0x81, "Î" },
        { 0x82, "Â" },
        { 0x83, "Ä" },
        { 0x84, "Ç" },
        { 0x85, "È" },
        { 0x86, "É" },
        { 0x87, "Ê" },
        { 0x88, "Ë" },
        { 0x89, "Ï" },
        { 0x8A, "Ô" },
        { 0x8B, "Ö" },
        { 0x8C, "Ù" },
        { 0x8D, "Û" },
        { 0x8E, "Ü" },
        { 0x8F, "ß" },
        { 0x90, "à" },
        { 0x91, "á" },
        { 0x92, "â" },
        { 0x93, "ä" },
        { 0x94, "ç" },
        { 0x95, "è" },
        { 0x96, "é" },
        { 0x97, "ê" },
        { 0x98, "ë" },
        { 0x99, "ï" },
        { 0x9A, "ô" },
        { 0x9B, "ö" },
        { 0x9C, "ù" },
        { 0x9D, "û" },
        { 0x9E, "ü" }};

    while (cont) {
        if (*takethis < 0x20) {
            switch (*takethis) {
              case 0x00:
                addlit("\0");
                break;

              case 0x01:
                addlit("\n");
                break;

              case 0x02:
                the_list.emplace_back(TextAST::Type::EndMessage);
                cont = false;
                break;

              case 0x04:
                the_list.emplace_back(TextAST::Type::NewBox);
                break;

              case 0x05:
                switch (*(++takethis)) {
                  case 0x40:
                    the_list.emplace_back(TextAST::Color::White);
                    break;

                  case 0x41:
                    the_list.emplace_back(TextAST::Color::Red);
                    break;

                  case 0x42:
                    the_list.emplace_back(TextAST::Color::Green);
                    break;

                  case 0x43:
                    the_list.emplace_back(TextAST::Color::Blue);
                    break;

                  case 0x44:
                    the_list.emplace_back(TextAST::Color::Cyan);
                    break;

                  case 0x45:
                    the_list.emplace_back(TextAST::Color::Magenta);
                    break;

                  case 0x46:
                    the_list.emplace_back(TextAST::Color::Yellow);
                    break;

                  case 0x47:
                    the_list.emplace_back(TextAST::Color::Black);
                    break;

                  default:
                    throw X::Text::BadSequence({0x05, *takethis}, "bad color value");
                    break;
                }
                break;

              case 0x06:
                the_list.emplace_back(TextAST::Type::Multispace, *takethis);
                break;

              case 0x07:
                the_list.emplace_back(TextAST::Type::Goto, be_u16(takethis + 1));
                takethis += 2;
                break;

              case 0x08:
                the_list.emplace_back(TextAST::Type::InstantTextState, true);
                break;

              case 0x09:
                the_list.emplace_back(TextAST::Type::InstantTextState, false);
                break;

              case 0x0A:
                the_list.emplace_back(TextAST::Type::StayOpen);
                break;

              case 0x0B:
                the_list.emplace_back(TextAST::Type::UnknownTrigger);
                break;

              case 0x0C:
                the_list.emplace_back(TextAST::Type::Delay, *++takethis);
                break;

              case 0x0D:
                the_list.emplace_back(TextAST::Type::WaitOnButton);
                break;

              case 0x0E:
                the_list.emplace_back(TextAST::Type::DelayThenFade, *++takethis);
                break;

              case 0x0F:
                the_list.emplace_back(TextAST::Type::PlayerName);
                break;

              case 0x10:
                the_list.emplace_back(TextAST::Type::StartOcarina);
                break;

              case 0x11:
                the_list.emplace_back(TextAST::Type::FadeWaitStop);
                break;

              case 0x12:
                the_list.emplace_back(TextAST::Type::PlaySFX, be_u16(takethis + 1));
                takethis += 2;
                break;

              case 0x13:
                the_list.emplace_back(TextAST::Type::ShowIcon, *++takethis);
                break;

              case 0x14:
                the_list.emplace_back(TextAST::Type::TextSpeedAt, *++takethis);
                break;

              case 0x15:
                the_list.emplace_back(TextAST::Type::ChangeMsgBG, be_u24(takethis + 1));
                takethis += 3;
                break;

              case 0x16:
                the_list.emplace_back(TextAST::Type::MarathonTime);
                break;

              case 0x17:
                the_list.emplace_back(TextAST::Type::RaceTime);
                break;

              case 0x18:
                the_list.emplace_back(TextAST::Type::NumPoints);
                break;

              case 0x19:
                the_list.emplace_back(TextAST::Type::NumGoldSkulls);
                break;

              case 0x1A:
                the_list.emplace_back(TextAST::Type::NoSkipping);
                break;

              case 0x1B:
                the_list.emplace_back(TextAST::Type::TwoChoices);
                break;

              case 0x1C:
                the_list.emplace_back(TextAST::Type::ThreeChoices);
                break;

              case 0x1D:
                the_list.emplace_back(TextAST::Type::FishWeight);
                break;

              case 0x1E:
                the_list.emplace_back(TextAST::Type::Highscore, *++takethis);
                break;

              case 0x1F:
                the_list.emplace_back(TextAST::Type::WorldTime);
                break;
            }
        } else if (0x20 <= *takethis && *takethis <= 0x7E) {
            if (*takethis == 0x5C) {
                addlit("¥");
            } else {
                addlit(std::string(1, *takethis));
            }
        } else if (0x7F <= *takethis && *takethis <= 0x9E) {
            addlit(specialChar.at(*takethis));
        } else if (0x9F <= *takethis && *takethis <= 0xAB) {
            switch (*takethis) {
              case 0x9F:
                the_list.emplace_back(TextAST::Button::A);
                break;
              case 0xA0:
                the_list.emplace_back(TextAST::Button::B);
                break;
              case 0xA1:
                the_list.emplace_back(TextAST::Button::C);
                break;
              case 0xA2:
                the_list.emplace_back(TextAST::Button::L);
                break;
              case 0xA3:
                the_list.emplace_back(TextAST::Button::R);
                break;
              case 0xA4:
                the_list.emplace_back(TextAST::Button::Z);
                break;
              case 0xA5:
                the_list.emplace_back(TextAST::Button::C_UP);
                break;
              case 0xA6:
                the_list.emplace_back(TextAST::Button::C_DOWN);
                break;
              case 0xA7:
                the_list.emplace_back(TextAST::Button::C_LEFT);
                break;
              case 0xA8:
                the_list.emplace_back(TextAST::Button::C_RIGHT);
                break;
              case 0xA9:
                addlit("▼");
                break;
              case 0xAA:
                the_list.emplace_back(TextAST::Button::ASTICK);
                break;
              case 0xAB:
                the_list.emplace_back(TextAST::Button::DPAD);
                break;
            }
        } else {
            throw X::Text::BadSequence({*takethis});
        }

        takethis++;
    }

    return the_list;
}




std::vector<TextAST> readShiftJIS_OoT(std::vector<uint8_t>::iterator & takethis) {
    std::vector<TextAST> the_list;
    bool cont = true;

    auto addlit = [&](std::string piece) {
        if (the_list.size() == 0 || !the_list.back().literalAddText(piece)) {
            the_list.emplace_back(piece);
        }
    };

    while (cont) {
        uint8_t first = *takethis++;
        uint8_t second, third, fourth; // these are to aid readability below

        // first, we try to handle the special control codes that besmirch the
        // good Shift-JIS name by using double-bytes where they're not allowed,
        // and having more-than-two-byte sequences.

        switch (first) {
          case 0x00:
            second = *takethis++;
            switch (second) {
              case 0x0A:
                addlit("\n");
                break;

              case 0x0B:
                third = *takethis++;
                if (third == 0x0C) {
                    fourth = *takethis++;
                    switch (fourth) {
                      case 0x00:
                        the_list.emplace_back(TextAST::Color::White);
                        break;

                      case 0x01:
                        the_list.emplace_back(TextAST::Color::Red);
                        break;

                      case 0x02:
                        the_list.emplace_back(TextAST::Color::Green);
                        break;

                      case 0x03:
                        the_list.emplace_back(TextAST::Color::Blue);
                        break;

                      case 0x04:
                        the_list.emplace_back(TextAST::Color::Cyan);
                        break;

                      case 0x05:
                        the_list.emplace_back(TextAST::Color::Magenta);
                        break;

                      case 0x06:
                        the_list.emplace_back(TextAST::Color::Yellow);
                        break;

                      case 0x07:
                        the_list.emplace_back(TextAST::Color::Black);
                        break;

                      default:
                        throw X::Text::BadSequence({first, second, third, fourth}, "bad color value");
                        break;
                    }
                } else {
                    throw X::Text::BadSequence({first, second, third});
                }
                break;

              default:
                // at this point, 00 must be null, which is currently handled by
                // giving a literal null. We also unadvance the takethis, since
                // the second byte isn't ours after all.
                addlit("\0");
                takethis--;
                break;
            }
            break;

          case 0x81:
            second = *takethis++;
            switch (second) {
              case 0x70:
                the_list.emplace_back(TextAST::Type::EndMessage);
                cont = false;
                break;

              case 0xA5:
                the_list.emplace_back(TextAST::Type::NewBox);
                break;

              case 0xCB:
                // third and fourth implicitly in be_u16, so not assigned in this
                // case
                the_list.emplace_back(TextAST::Type::Goto, be_u16(takethis));
                takethis += 2; // advance past ID bytes (third & fourth)
                break;

              case 0x89:
                the_list.emplace_back(TextAST::Type::InstantTextState, true);
                break;

              case 0x8A:
                the_list.emplace_back(TextAST::Type::InstantTextState, false);
                break;

              case 0x9F:
                the_list.emplace_back(TextAST::Type::UnknownTrigger);
                break;

              case 0xA3:
                third = *takethis++;

                if (third == 0x00) {
                    fourth = *takethis++;
                    the_list.emplace_back(TextAST::Type::Delay, fourth);
                } else {
                    throw X::Text::BadSequence({first, second, third});
                }
                break;

              case 0x9E:
                third = *takethis++;

                if (third == 0x00) {
                    fourth = *takethis++;
                    the_list.emplace_back(TextAST::Type::DelayThenFade, fourth);
                } else {
                    throw X::Text::BadSequence({first, second, third});
                }
                break;

              case 0xF0:
                the_list.emplace_back(TextAST::Type::StartOcarina);
                break;

              case 0xF3:
                // third and fourth implied in be_u16
                the_list.emplace_back(TextAST::Type::PlaySFX, be_u16(takethis));
                takethis += 2; // advance past third and fourth
                break;

              case 0x9A:
                third = *takethis++;

                if (third == 0x00) {
                    fourth = *takethis++;
                    the_list.emplace_back(TextAST::Type::ShowIcon, fourth);
                } else {
                    throw X::Text::BadSequence({first, second, third});
                }
                break;

              case 0x99:
                the_list.emplace_back(TextAST::Type::NoSkipping);
                break;

              case 0xBC:
                the_list.emplace_back(TextAST::Type::TwoChoices);
                break;

              case 0xB8:
                the_list.emplace_back(TextAST::Type::ThreeChoices);
                break;

              case 0xA1:
                the_list.emplace_back(TextAST::Type::WorldTime);
                break;

              default:
                // otherwise, we know it has to be a normal two-byte Shift-JIS
                // character.

                addlit(code_to_utf8(SJIS_doubleTable.at(first).at(second)));
                break;
            }
            break;

          case 0x83:
            second = *takethis++;

            switch (second) {
              case 0x9F:
                the_list.emplace_back(TextAST::Button::A);
                break;
              case 0xA0:
                the_list.emplace_back(TextAST::Button::B);
                break;
              case 0xA1:
                the_list.emplace_back(TextAST::Button::C);
                break;
              case 0xA2:
                the_list.emplace_back(TextAST::Button::L);
                break;
              case 0xA3:
                the_list.emplace_back(TextAST::Button::R);
                break;
              case 0xA4:
                the_list.emplace_back(TextAST::Button::Z);
                break;
              case 0xA5:
                the_list.emplace_back(TextAST::Button::C_UP);
                break;
              case 0xA6:
                the_list.emplace_back(TextAST::Button::C_DOWN);
                break;
              case 0xA7:
                the_list.emplace_back(TextAST::Button::C_LEFT);
                break;
              case 0xA8:
                the_list.emplace_back(TextAST::Button::C_RIGHT);
                break;
              case 0xA9:
                addlit("▼");
                break;
              case 0xAA:
                the_list.emplace_back(TextAST::Button::ASTICK);
                break;
              case 0xAB:
                the_list.emplace_back(TextAST::Button::DPAD);
                break;
              default:
                addlit(code_to_utf8(SJIS_doubleTable.at(first).at(second)));
                break;
            }
            break;

          case 0x86:
            second = *takethis++;

            switch (second) {
              case 0xC7:
                third = *takethis++;

                if (third == 0x00) {
                    fourth = *takethis++;

                    the_list.emplace_back(TextAST::Type::Multispace, fourth);
                } else {
                    throw X::Text::BadSequence({first, second, third});
                }
                break;

              case 0xC8:
                the_list.emplace_back(TextAST::Type::StayOpen);
                break;

              case 0xC9:
                third = *takethis++;

                if (third == 0x00) {
                    fourth = *takethis++;

                    the_list.emplace_back(TextAST::Type::TextSpeedAt, fourth);
                } else {
                    throw X::Text::BadSequence({first, second, third});
                }
                break;

              case 0xB3:
                third = *takethis++;

                if (third == 0x00) {
                    // this value takes up three bytes (fourth, fifth, and
                    // sixth!). We'll be using be_u24 to get them, though.

                    the_list.emplace_back(TextAST::Type::ChangeMsgBG, be_u24(takethis));

                    takethis += 3;
                } else {
                    throw X::Text::BadSequence({first, second, third});
                }
                break;

              case 0xA3:
                the_list.emplace_back(TextAST::Type::NumGoldSkulls);
                break;

              case 0xA4:
                the_list.emplace_back(TextAST::Type::FishWeight);
                break;

              case 0x9F:
                third = *takethis++;

                if (third == 0x00) {
                    fourth = *takethis++;

                    the_list.emplace_back(TextAST::Type::Highscore, fourth);
                } else {
                    throw X::Text::BadSequence({first, second, third});
                }
                break;

              default:
                // no two-bytes start with 86, so die here
                throw X::Text::BadSequence({first});
                takethis--;
                break;
            }
            break;

          case 0x87:
            second = *takethis++;

            switch (second) {
              case 0x4F:
                the_list.emplace_back(TextAST::Type::PlayerName);
                break;

              case 0x91:
                the_list.emplace_back(TextAST::Type::MarathonTime);
                break;

              case 0x92:
                the_list.emplace_back(TextAST::Type::RaceTime);
                break;

              case 0x9B:
                the_list.emplace_back(TextAST::Type::NumPoints);
                break;

              default:
                // no sequences start with 87 in normal shift-JIS, so give back
                // second and mark as bad
                throw X::Text::BadSequence({first});
                takethis--;
                break;
            }
            break;

          default:
            // first, try multi-byte
            if (SJIS_doubleTable.count(first) == 1) {
                second = *takethis++;

                addlit(code_to_utf8(SJIS_doubleTable.at(first).at(second)));
            } else if (SJIS_singleTable.count(first) == 1) {
                // single-byte with special mapping

                addlit(code_to_utf8(SJIS_singleTable.at(first)));
            } else {
                // plain ol' ASCII
                addlit(std::string(1, first));
            }
            break;
        }
    }

    return the_list;
}

std::vector<TextAST> readASCII_MM(std::vector<uint8_t>::iterator & indata) {
    std::vector<TextAST> the_list;
    bool cont = true;

    auto addlit = [&](std::string piece) {
        if (the_list.size() == 0 || !the_list.back().literalAddText(piece)) {
            the_list.emplace_back(piece);
        }
    };

    static const std::map<uint8_t, std::string> specialchar{
        { 0x7F, "°"},
        { 0x80, "À"},
        { 0x81, "Á"},
        { 0x82, "Â"},
        { 0x83, "Ä"},
        { 0x84, "Ç"},
        { 0x85, "È"},
        { 0x86, "É"},
        { 0x87, "Ê"},
        { 0x88, "Ë"},
        { 0x89, "Ì"},
        { 0x8A, "Í"},
        { 0x8B, "Î"},
        { 0x8C, "Ï"},
        { 0x8D, "Ñ"},
        { 0x8E, "Ò"},
        { 0x8F, "Ó"},
        { 0x90, "Ô"},
        { 0x91, "Ö"},
        { 0x92, "Ù"},
        { 0x93, "Ú"},
        { 0x94, "Û"},
        { 0x95, "Ü"},
        { 0x96, "ß"},
        { 0x97, "à"},
        { 0x98, "á"},
        { 0x99, "â"},
        { 0x9A, "ä"},
        { 0x9B, "ç"},
        { 0x9C, "è"},
        { 0x9D, "é"},
        { 0x9E, "ê"},
        { 0x9F, "ë"},
        { 0xA0, "ì"},
        { 0xA1, "í"},
        { 0xA2, "î"},
        { 0xA3, "ï"},
        { 0xA4, "ñ"},
        { 0xA5, "ò"},
        { 0xA6, "ó"},
        { 0xA7, "ô"},
        { 0xA8, "ö"},
        { 0xA9, "ù"},
        { 0xAA, "ú"},
        { 0xAB, "û"},
        { 0xAC, "ü"},
        { 0xAD, "¡"},
        { 0xAE, "¿"},
        { 0xAF, "ª"}};

    // first we read past the header messages have, since we don't currently
    // care about that

    indata += 11;

    // now to read the text

    while (cont) {
        // readability vars
        uint8_t first, second;

        first = *indata++;

        if (0x20 <= first && first <= 0x7E) {
            addlit(std::string(1, first));
        } else if (0x7F <= first && first <= 0xAF) {
            addlit(specialchar.at(first));
        } else {
            switch (first) {
              case 0x00:
                the_list.emplace_back(TextAST::Color::White);
                break;

              case 0x01:
                the_list.emplace_back(TextAST::Color::Red);
                break;

              case 0x02:
                the_list.emplace_back(TextAST::Color::Green);
                break;

              case 0x03:
                the_list.emplace_back(TextAST::Color::Blue);
                break;

              case 0x04:
                the_list.emplace_back(TextAST::Color::Yellow);
                break;

              case 0x05:
                the_list.emplace_back(TextAST::Color::Cyan);
                break;

              case 0x06:
                the_list.emplace_back(TextAST::Color::Magenta);
                break;

              case 0x07:
                the_list.emplace_back(TextAST::Color::Gray);
                break;

              case 0x08:
                the_list.emplace_back(TextAST::Color::Orange);
                break;

              case 0x0A:
                second = *indata++;

                the_list.emplace_back(TextAST::Type::Multispace, second);
                break;

              case 0x0B:
                the_list.emplace_back(TextAST::Type::SwampArchHits);
                break;

              case 0x0C:
                the_list.emplace_back(TextAST::Type::NumFairiesGot);
                break;

              case 0x0D:
                // XXX may want separate type for MM's gold skulltulas
                the_list.emplace_back(TextAST::Type::NumGoldSkulls);
                break;

              case 0x10:
                // XXX not 100% sure on exact behavior
                the_list.emplace_back(TextAST::Type::NewBox);
                break;

              case 0x11:
                addlit("\n");
                break;

              case 0x12:
                // XXX not 100% sure on exact behavior
                the_list.emplace_back(TextAST::Type::NewBox);
                break;

              case 0x13:
                // using type for carriage return so we don't have weirdness
                // possibly in plaintext representation, for instance
                the_list.emplace_back(TextAST::Type::CarriageReturn);
                break;

              case 0x15:
                the_list.emplace_back(TextAST::Type::NoSkipping);
                break;

              case 0x16:
                the_list.emplace_back(TextAST::Type::PlayerName);
                break;

              case 0x17:
                the_list.emplace_back(TextAST::Type::InstantTextState, true);
                break;

              case 0x18:
                the_list.emplace_back(TextAST::Type::InstantTextState, false);
                break;

              case 0x19:
                the_list.emplace_back(TextAST::Type::NoSkipping_withSfx);
                break;

              case 0x1A:
                the_list.emplace_back(TextAST::Type::StayOpen);
                break;

              case 0x1B:
                the_list.emplace_back(TextAST::Type::DelayThenPrint, be_u16(indata));
                indata += 2;
                break;

              case 0x1C:
                the_list.emplace_back(TextAST::Type::StayAfter, be_u16(indata));
                indata += 2;
                break;

              case 0x1D:
                the_list.emplace_back(TextAST::Type::DelayThenEndText, be_u16(indata));
                indata += 2;
                break;

              case 0x1E:
                the_list.emplace_back(TextAST::Type::PlaySFX, be_u16(indata));
                indata += 2;
                break;

              case 0x1F:
                // XXX for sure it's regular Delay?
                the_list.emplace_back(TextAST::Type::Delay, be_u16(indata));
                indata += 2;
                break;

              case 0xB0:
                the_list.emplace_back(TextAST::Button::A);
                break;

              case 0xB1:
                the_list.emplace_back(TextAST::Button::B);
                break;

              case 0xB2:
                the_list.emplace_back(TextAST::Button::C);
                break;

              case 0xB3:
                the_list.emplace_back(TextAST::Button::L);
                break;

              case 0xB4:
                the_list.emplace_back(TextAST::Button::R);
                break;

              case 0xB5:
                the_list.emplace_back(TextAST::Button::Z);
                break;

              case 0xB6:
                the_list.emplace_back(TextAST::Button::C_UP);
                break;

              case 0xB7:
                the_list.emplace_back(TextAST::Button::C_DOWN);
                break;

              case 0xB8:
                the_list.emplace_back(TextAST::Button::C_LEFT);
                break;

              case 0xB9:
                the_list.emplace_back(TextAST::Button::C_RIGHT);
                break;

              case 0xBA:
                addlit("▼");
                break;

              case 0xBB:
                the_list.emplace_back(TextAST::Button::ASTICK);
                break;

                // I'd guess 0xBC for d-pad, but not listed

              case 0xBF:
                the_list.emplace_back(TextAST::Type::EndMessage);
                cont = false;
                break;

              case 0xC1:
                the_list.emplace_back(TextAST::Type::FailedSongX);
                break;

              case 0xC2:
                the_list.emplace_back(TextAST::Type::TwoChoices);
                break;

              case 0xC3:
                the_list.emplace_back(TextAST::Type::ThreeChoices);
                break;

              case 0xC4:
                the_list.emplace_back(TextAST::Type::PostmanGameTime);
                break;

                // this case is an unused character, but seems worthwhile enough
                // include support for.
              case 0xC7:
                the_list.emplace_back(TextAST::Type::TimeLeftInFight);
                break;

              case 0xC8:
                the_list.emplace_back(TextAST::Type::DekuFlowerGameScore);
                break;

              case 0xCB:
                the_list.emplace_back(TextAST::Type::ShootingGalleryScore);
                break;

              case 0xCC:
                the_list.emplace_back(TextAST::Type::BankRupeePrompt);
                break;

              case 0xCD:
                the_list.emplace_back(TextAST::Type::ShowRupeesGiven);
                break;

              case 0xCE:
                the_list.emplace_back(TextAST::Type::ShowRupeesEarned);
                break;

              case 0xCF:
                // XXX want different for MM?
                the_list.emplace_back(TextAST::Type::WorldTime);
                break;

              case 0xD0:
                the_list.emplace_back(TextAST::Type::LotteryRupeePrompt);
                break;

              case 0xD1:
                the_list.emplace_back(TextAST::Type::BomberCodePrompt);
                break;

              case 0xD2:
                the_list.emplace_back(TextAST::Type::WaitOnItem);
                break;

              case 0xD4:
                the_list.emplace_back(TextAST::Type::SoaringDestination);
                break;

              case 0xD5:
                the_list.emplace_back(TextAST::Type::LotteryGuessPrompt);
                break;

                // another [supposedly] unused, but worthy, command
              case 0xD6:
                the_list.emplace_back(TextAST::Type::OceanSpiderMaskOrder);
                break;

              case 0xD7:
                the_list.emplace_back(TextAST::Type::FairiesLeftIn, 1);
                break;

              case 0xD8:
                the_list.emplace_back(TextAST::Type::FairiesLeftIn, 2);
                break;

              case 0xD9:
                the_list.emplace_back(TextAST::Type::FairiesLeftIn, 3);
                break;

              case 0xDA:
                the_list.emplace_back(TextAST::Type::FairiesLeftIn, 4);
                break;

              case 0xDB:
                the_list.emplace_back(TextAST::Type::SwampArchScore);
                break;

              case 0xDC:
                the_list.emplace_back(TextAST::Type::ShowLotteryNumber);
                break;

              case 0xDD:
                the_list.emplace_back(TextAST::Type::ShowLotteryGuess);
                break;

              case 0xDE:
                the_list.emplace_back(TextAST::Type::MonetaryValue);
                break;

              case 0xDF:
                the_list.emplace_back(TextAST::Type::ShowBomberCode);
                break;

              case 0xE0:
                the_list.emplace_back(TextAST::Type::EndConversation);
                // XXX put cont = false here?
                break;

              case 0xE1:
              case 0xE2:
              case 0xE3:
              case 0xE4:
              case 0xE5:
              case 0xE6:
                the_list.emplace_back(TextAST::Type::ShowMaskColor, first & 0x0F);
                break;

              case 0xE7:
                the_list.emplace_back(TextAST::Type::HoursLeft);
                break;

              case 0xE8:
                the_list.emplace_back(TextAST::Type::TimeToMorning);
                break;

              case 0xF6:
                the_list.emplace_back(TextAST::Type::OctoArchHiscore);
                break;

                // supposedly unused, but possibly not
              case 0xF8:
                the_list.emplace_back(TextAST::Type::BeanPrice);
                break;

              case 0xF9:
                the_list.emplace_back(TextAST::Type::EponaArchHiscore);
                break;

              case 0xFA:
                the_list.emplace_back(TextAST::Type::DekuFlowerGameDailyHiscore, 1);
                break;

              case 0xFB:
                the_list.emplace_back(TextAST::Type::DekuFlowerGameDailyHiscore, 2);
                break;

              case 0xFC:
                the_list.emplace_back(TextAST::Type::DekuFlowerGameDailyHiscore, 3);
                break;
            }
        }
    }

    return the_list;
}