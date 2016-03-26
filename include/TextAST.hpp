/** \file TextAST.hpp
 *
 *  \brief Declaring the needed classes and such to handle in-game text without
 *         repetitious code.
 *
 */

#pragma once

#include <cstdint>
#include <string>

class TextAST {
  public:
    enum class Type {
        Literal,
        EndMessage,
        NewBox,
        Color,
        Multispace,
        Goto,
        InstantTextState,
        StayOpen,
        UnknownTrigger,
        Delay,
        WaitOnButton,
        DelayThenFade,
        PlayerName,
        StartOcarina,
        FadeWaitStop,
        PlaySFX,
        ShowIcon,
        TextSpeedAt,
        ChangeMsgBG,
        MarathonTime,
        RaceTime,
        NumPoints,
        NumGoldSkulls,
        NoSkipping,
        TwoChoices,
        ThreeChoices,
        FishWeight,
        Highscore,
        WorldTime,
        Button,
    };

    enum class Color {
        White,
        Red,
        Green,
        Blue,
        Cyan,
        Magenta,
        Yellow,
        Black,
    };

    enum class Button {
        A,
        B,
        C,
        L,
        R,
        Z,
        C_UP,
        C_DOWN,
        C_LEFT,
        C_RIGHT,
        ASTICK,
        DPAD,
    };

  private:
    Type atype;
    std::string lit;
    uint32_t val;
    Color col;
    Button btn;

  public:
    TextAST(std::string L);
    TextAST(Color C);
    TextAST(Button B);
    TextAST(Type T, uint32_t V = 0);

    Color getColor() const;
    uint32_t getValue() const;
    Type getType() const;
    Button getButton() const;

    std::string codeString() const;

    bool literalAddText(std::string n);
};