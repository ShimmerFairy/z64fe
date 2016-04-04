/** \file TextAST.hpp
 *
 *  \brief Declaring the needed classes and such to handle in-game text without
 *         repetitious code.
 *
 */

#pragma once

#include "Exceptions.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace TextAST {
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
        // MM-only types
        SwampArchHits,
        NumFairiesGot,
        CarriageReturn,
        NoSkipping_withSfx,
        DelayThenPrint,
        StayAfter,
        DelayThenEndText,
        FailedSongX,
        PostmanGameTime,
        TimeLeftInFight,
        DekuFlowerGameScore,
        ShootingGalleryScore,
        BankRupeePrompt,
        ShowRupeesGiven,
        ShowRupeesEarned,
        TimeLeft,
        LotteryRupeePrompt,
        BomberCodePrompt,
        WaitOnItem,
        SoaringDestination,
        LotteryGuessPrompt,
        OceanSpiderMaskOrder,
        FairiesLeftIn, // XXX want enum for this one's values?
        SwampArchScore,
        ShowLotteryNumber,
        ShowLotteryGuess,
        MonetaryValue,
        ShowBomberCode,
        EndConversation,
        ShowMaskColor,
        HoursLeft,
        TimeToMorning,
        OctoArchHiscore,
        BeanPrice,
        EponaArchHiscore,
        DekuFlowerGameDailyHiscore,
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
        Gray,
        Orange,
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

    class Fragment {
      private:
        Type ftype;

        std::string strval;
        uint32_t intval;
        Color colval;
        Button btnval;

      public:
        Fragment(std::string L);
        Fragment(Color C);
        Fragment(Button B);
        Fragment(Type T, uint32_t V = 0);

        Type getType() const;

        template<typename T>
        T getValue() const;

        bool tryMoreText(std::string txt);
    };

    class Line {
      private:
        std::vector<Fragment> pieces;

      public:
        void push(Fragment np);

        void addMoreText(std::string txt);

        std::vector<Fragment>::iterator begin();
        std::vector<Fragment>::iterator end();
    };

    class Box {
      private:
        std::vector<Line> lines;

      public:
        void push(Line nl);

        Line & curline();

        size_t size() const;

        std::vector<Line>::iterator begin();
        std::vector<Line>::iterator end();
    };
}

// put here to avoid the generic exceptions include depending on this one
// instead

namespace X {
    namespace Text {
        class WrongVariant : public Exception {
          private:
            TextAST::Type got;
            TextAST::Type expect;
            bool expect_specific;

          public:
            WrongVariant(TextAST::Type g);
            WrongVariant(TextAST::Type g, TextAST::Type e);

            std::string what();
        };
    }
}