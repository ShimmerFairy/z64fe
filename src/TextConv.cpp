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