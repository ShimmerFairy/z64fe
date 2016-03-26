/** \file TextAST.cpp
 *
 *  \brief Implementation of stuff
 *
 */

#include "TextAST.hpp"

#include <sstream>
#include <iomanip>

TextAST::TextAST(std::string L) : atype(Type::Literal), lit(L) { }
TextAST::TextAST(Color C) : atype(Type::Color), col(C) { }
TextAST::TextAST(Button B) : atype(Type::Button), btn(B) { }
TextAST::TextAST(Type T, uint32_t V) : atype(T), val(V) { }

TextAST::Color TextAST::getColor() const { return col; }
uint32_t TextAST::getValue() const { return val; }
TextAST::Type TextAST::getType() const { return atype; }
TextAST::Button TextAST::getButton() const { return btn; }

std::string TextAST::codeString() const {
    std::stringstream r;

    switch (atype) {
      case Type::Literal:
        r << lit;
        break;

      case Type::Color:
        switch (col) {
          case Color::White:
            r << "\\color{white}";
            break;

          case Color::Red:
            r << "\\color{red}";
            break;

          case Color::Green:
            r << "\\color{green}";
            break;

          case Color::Blue:
            r << "\\color{blue}";
            break;

          case Color::Cyan:
            // Sadly, I Myst my chance to put a joke here.
            r << "\\color{cyan}";
            break;

          case Color::Magenta:
            r << "\\color{magenta}";
            break;

          case Color::Yellow:
            r << "\\color{yellow}";
            break;

          case Color::Black:
            r << "\\color{black}";
            break;
        }
        break;

      case Type::EndMessage:
        r << "\\endMessage{}";
        break;

      case Type::NewBox:
        r << "\\newBox{}";
        break;

      case Type::Multispace:
        r << "\\spaces{" << val << "}";
        break;

      case Type::Goto:
        r << "\\goto{0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4)
          << val << "}";
        break;

      case Type::InstantTextState:
        r << (val ? "\\instantTextOn" : "\\instantTextOff") << "{}";
        break;

      case Type::StayOpen:
        r << "\\keepBoxOpen{}";
        break;

      case Type::UnknownTrigger:
        r << "\\unknownTrigger{}";
        break;

      case Type::Delay:
        r << "\\waitXFrames{" << val << "}";
        break;

      case Type::WaitOnButton:
        r << "\\waitForAnyButton{}";
        break;

      case Type::DelayThenFade:
        r << "\\waitXFramesThenFade{" << val << "}";
        break;

      case Type::PlayerName:
        r << "\\playerName{}";
        break;

      case Type::StartOcarina:
        r << "\\startOcarinaPlaying{}";
        break;

      case Type::FadeWaitStop:
        r << "\\bailFadeAndWait{}";
        break;

      case Type::PlaySFX:
        r << "\\sfx{0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4)
          << val << "}";
        break;

      case Type::ShowIcon:
        r << "\\icon{0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2)
          << val << "}";
        break;

      case Type::TextSpeedAt:
        r << "\\setTextSpeedTo{0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2)
          << val << "}";
        break;

      case Type::ChangeMsgBG:
        r << "\\setBackground{0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(6)
          << val << "}";
        break;


      case Type::MarathonTime:
        r << "\\marathonTime{}";
        break;

      case Type::RaceTime:
        r << "\\raceTime{}";
        break;

      case Type::NumPoints:
        r << "\\numberOfPoints{}";
        break;

      case Type::NumGoldSkulls:
        r << "\\numberOfGoldSkulltulas{}";
        break;

      case Type::NoSkipping:
        r << "\\cantSkipNow{}";
        break;

      case Type::TwoChoices:
        r << "\\askTwoChoices{}";
        break;

      case Type::ThreeChoices:
        r << "\\askThreeChoices{}";
        break;

      case Type::FishWeight:
        r << "\\fishWeight{}";
        break;

      case Type::Highscore:
        r << "\\hiscore{0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2)
          << val << "}";
        break;

      case Type::WorldTime:
        r << "\\worldTime{}";
        break;

      case Type::Button:
        switch (btn) {
          case Button::A:
            r << "\\A{}";
            break;

          case Button::B:
            r << "\\B{}";
            break;

          case Button::C:
            r << "\\C{}";
            break;

          case Button::L:
            r << "\\L{}";
            break;

          case Button::R:
            r << "\\R{}";
            break;

          case Button::Z:
            r << "\\Z{}";
            break;

          case Button::C_UP:
            r << "\\C{up}";
            break;

          case Button::C_DOWN:
            r << "\\C{down}";
            break;

          case Button::C_LEFT:
            r << "\\C{left}";
            break;

          case Button::C_RIGHT:
            r << "\\C{right}";
            break;

          case Button::ASTICK:
            r << "\\AnalogStick{}";
            break;

          case Button::DPAD:
            r << "\\Dpad{}";
            break;
        }
        break;
    }

    return r.str();
}

bool TextAST::literalAddText(std::string n) {
    if (atype == Type::Literal) {
        lit += n;
        return true;
    }

    return false;
}