/** \file TextViewer.cpp
 *
 *  \brief Implement text viewer
 *
 */

#include "TextViewer.hpp"
#include "TextConv.hpp"
#include "TextAST.hpp"
#include "Exceptions.hpp"

#include <QString>
#include <QMessageBox>

#include <sstream>
#include <iomanip>
#include <iostream>

TextViewer::TextViewer(ROM::ROM & r, TextAST::MessageIndex msgindex) : trom(&r), midx(msgindex) {
    setAttribute(Qt::WA_DeleteOnClose);

    idlist = new QTreeView;
    idmod = new TextIDModel(midx);
    msgview = new QTextEdit;
    qhb = new QHBoxLayout;
    msgrend = new TextRender;
    qvb = new QVBoxLayout;
    dummy = new QWidget;

    idlist->setModel(idmod);
    idlist->setSelectionMode(QAbstractItemView::SingleSelection);

    msgview->setReadOnly(true);

    qhb->addWidget(idlist);
    qhb->addWidget(msgview);

    qvb->addLayout(qhb);
    qvb->addWidget(msgrend);

    dummy->setLayout(qvb);

    setCentralWidget(dummy);

    setWindowTitle(tr("Z64Fe - Text Viewer"));

    connect(idlist->selectionModel(), &QItemSelectionModel::currentChanged, this, &TextViewer::chooseText);
}

void TextViewer::chooseText(const QModelIndex & sel, const QModelIndex & /*desel*/) {
    // we can assume one selection because we asked for single selections in the
    // constructor.

    // we check for a valid parent as a way of making sure we'll only do stuff
    // when an ID, not a language, is selected.
    if (sel.parent().isValid()) {
        readtxt.clear();
        uint16_t id = idmod->data(sel, TextIDModel::rawRole).toUInt();
        Config::Language lang = static_cast<Config::Language>(idmod->data(sel.parent(), TextIDModel::rawRole).toUInt());

        ROM::File msgfile;

        minfo = midx.at(lang).at(id);

        try {
            switch (lang) {
              case Config::Language::JP:
                msgfile = trom->fileAtName("jpn_message_data_static");
                break;

              case Config::Language::EN:
                msgfile = trom->fileAtName("nes_message_data_static");
                break;

              case Config::Language::DE:
                msgfile = trom->fileAtName("ger_message_data_static");
                break;

              case Config::Language::FR:
                msgfile = trom->fileAtName("fra_message_data_static");
                break;

              case Config::Language::ES:
                msgfile = trom->fileAtName("esp_message_data_static");
                break;
            }
        } catch (Exception & e) {
            QMessageBox::critical(this, tr("ERROR!"),
                                  e.what().c_str());
            std::exit(-1);
        }

        std::vector<uint8_t> the_text;

        auto readptr = msgfile.begin() + minfo.address;

        if (Config::getGame(trom->getVersion()) == Config::Game::Ocarina) {
            if (lang == Config::Language::JP) {
                readtxt = readShiftJIS_OoT(readptr);
            } else {
                readtxt = readASCII_OoT(readptr);
            }
        } else {
            // in the case of Majora, we have to check if the msg info was
            // deferred (guaranteed as of now, but still good to check), and if
            // so update our copy of minfo. Our current choice is to update
            // everything if anything's deferred.
            if (minfo.kind == TextAST::BoxKind::MM_DEFER || minfo.where == TextAST::BoxYPos::MM_DEFER) {
                // read the header
                minfo.kind = TextAST::MM_BoxKind(*readptr++);
                minfo.where = TextAST::MM_BoxYPos(*readptr++);

                // advancing the iterator past the header depends on the region,
                // for some reason
            }

            if (lang == Config::Language::JP) {
                readptr += 10;
                readtxt = readShiftJIS_MM(readptr);
            } else {
                readptr += 9;
                readtxt = readASCII_MM(readptr);
            }
        }

        writeCodeText();

        msgrend->newText(minfo, readtxt);
    }
}

void TextViewer::writeCodeText() {
    std::stringstream result;

    result << "\\begin{message}\n";

    for (auto & i : readtxt) {
        result << "\\begin{box}\n";
        for (auto & j : i) {
            for (auto & k : j) {
                result << fragAsCode(k);
            }
            result << "\n";
        }
        result << "\\end{box}\n";
    }

    result << "\\end{message}";

    msgview->setPlainText(result.str().c_str());
}

std::string TextViewer::fragAsCode(TextAST::Fragment frag) {
    std::stringstream r;

    switch (frag.getType()) {
      case TextAST::Type::Literal:
        r << frag.getValue<std::string>();
        break;

      case TextAST::Type::Color:
        switch (frag.getValue<TextAST::Color>()) {
          case TextAST::Color::White:
            r << "\\color{white}";
            break;

          case TextAST::Color::Red:
            r << "\\color{red}";
            break;

          case TextAST::Color::Green:
            r << "\\color{green}";
            break;

          case TextAST::Color::Blue:
            r << "\\color{blue}";
            break;

          case TextAST::Color::Cyan:
            // Sadly, I Myst my chance to put a joke here.
            r << "\\color{cyan}";
            break;

          case TextAST::Color::Magenta:
            r << "\\color{magenta}";
            break;

          case TextAST::Color::Yellow:
            r << "\\color{yellow}";
            break;

          case TextAST::Color::Black:
            r << "\\color{black}";
            break;

          case TextAST::Color::Gray:
            r << "\\color{gray}";
            break;

          case TextAST::Color::Orange:
            r << "\\color{orange}";
            break;
        }
        break;

      case TextAST::Type::EndMessage:
        r << "\\endMessage{}";
        break;

      case TextAST::Type::NewBox:
        r << "\\newBox{}";
        break;

      case TextAST::Type::Multispace:
        r << "\\spaces{" << frag.getValue<uint32_t>() << "}";
        break;

      case TextAST::Type::Goto:
        r << "\\goto{0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4)
          << frag.getValue<uint32_t>() << "}";
        break;

      case TextAST::Type::InstantTextState:
        r << (frag.getValue<uint32_t>() ? "\\instantTextOn" : "\\instantTextOff") << "{}";
        break;

      case TextAST::Type::StayOpen:
        r << "\\keepBoxOpen{}";
        break;

      case TextAST::Type::UnknownTrigger:
        r << "\\unknownTrigger{}";
        break;

      case TextAST::Type::Delay:
        r << "\\waitXFrames{" << frag.getValue<uint32_t>() << "}";
        break;

      case TextAST::Type::WaitOnButton:
        r << "\\waitForAnyButton{}";
        break;

      case TextAST::Type::DelayThenFade:
        r << "\\waitXFramesThenFade{" << frag.getValue<uint32_t>() << "}";
        break;

      case TextAST::Type::PlayerName:
        r << "\\playerName{}";
        break;

      case TextAST::Type::StartOcarina:
        r << "\\startOcarinaPlaying{}";
        break;

      case TextAST::Type::FadeWaitStop:
        r << "\\bailFadeAndWait{}";
        break;

      case TextAST::Type::PlaySFX:
        r << "\\sfx{0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(4)
          << frag.getValue<uint32_t>() << "}";
        break;

      case TextAST::Type::ShowIcon:
        r << "\\icon{0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2)
          << frag.getValue<uint32_t>() << "}";
        break;

      case TextAST::Type::TextSpeedAt:
        r << "\\setTextSpeedTo{0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2)
          << frag.getValue<uint32_t>() << "}";
        break;

      case TextAST::Type::ChangeMsgBG:
        r << "\\setBackground{0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(6)
          << frag.getValue<uint32_t>() << "}";
        break;


      case TextAST::Type::MarathonTime:
        r << "\\marathonTime{}";
        break;

      case TextAST::Type::RaceTime:
        r << "\\raceTime{}";
        break;

      case TextAST::Type::NumPoints:
        r << "\\numberOfPoints{}";
        break;

      case TextAST::Type::NumGoldSkulls:
        r << "\\numberOfGoldSkulltulas{}";
        break;

      case TextAST::Type::NoSkipping:
        r << "\\cantSkipNow{}";
        break;

      case TextAST::Type::TwoChoices:
        r << "\\askTwoChoices{}";
        break;

      case TextAST::Type::ThreeChoices:
        r << "\\askThreeChoices{}";
        break;

      case TextAST::Type::FishWeight:
        r << "\\fishWeight{}";
        break;

      case TextAST::Type::Highscore:
        r << "\\hiscore{0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2)
          << frag.getValue<uint32_t>() << "}";
        break;

      case TextAST::Type::WorldTime:
        r << "\\worldTime{}";
        break;

      case TextAST::Type::Button:
        switch (frag.getValue<TextAST::Button>()) {
          case TextAST::Button::A:
            r << "\\A{}";
            break;

          case TextAST::Button::B:
            r << "\\B{}";
            break;

          case TextAST::Button::C:
            r << "\\C{}";
            break;

          case TextAST::Button::L:
            r << "\\L{}";
            break;

          case TextAST::Button::R:
            r << "\\R{}";
            break;

          case TextAST::Button::Z:
            r << "\\Z{}";
            break;

          case TextAST::Button::C_UP:
            r << "\\C{up}";
            break;

          case TextAST::Button::C_DOWN:
            r << "\\C{down}";
            break;

          case TextAST::Button::C_LEFT:
            r << "\\C{left}";
            break;

          case TextAST::Button::C_RIGHT:
            r << "\\C{right}";
            break;

          case TextAST::Button::ASTICK:
            r << "\\AnalogStick{}";
            break;

          case TextAST::Button::DPAD:
            r << "\\Dpad{}";
            break;
        }
        break;

      case TextAST::Type::SwampArchHits:
        r << "\\archerySwampHitsNeeded{}";
        break;

      case TextAST::Type::NumFairiesGot:
        r << "\\fairiesGotInThisDungeon{}";
        break;

      case TextAST::Type::CarriageReturn:
        r << "\\x{0D}";
        break;

      case TextAST::Type::NoSkipping_withSfx:
        r << "\\CantSkipNow[SFX]{}";
        break;

      case TextAST::Type::DelayThenPrint:
        r << "\\waitXFramesThenPrint{" << frag.getValue<uint32_t>() << "}";
        break;

      case TextAST::Type::StayAfter:
        r << "\\lingerXFramesOnBox{" << frag.getValue<uint32_t>() << "}";
        break;

      case TextAST::Type::DelayThenEndText:
        r << "\\waitXFramesThenEndText{" << frag.getValue<uint32_t>() << "}";
        break;

      case TextAST::Type::FailedSongX:
        r << "\\failedSongIndicator{}";
        break;

      case TextAST::Type::PostmanGameTime:
        r << "\\showTimePostmanGame{}";
        break;

      case TextAST::Type::TimeLeftInFight:
        r << "\\timeLeftInSkullkidFight{}";
        break;

      case TextAST::Type::DekuFlowerGameScore:
        r << "\\dekuFlowerGameScore{}";
        break;

      case TextAST::Type::ShootingGalleryScore:
        r << "\\shootingGalleryScore{}";
        break;

      case TextAST::Type::BankRupeePrompt:
        r << "\\bankPromptRupees{}";
        break;

      case TextAST::Type::ShowRupeesGiven:
        r << "\\showRupeesGiven{}";
        break;

      case TextAST::Type::ShowRupeesEarned:
        r << "\\showRupeesEarned{}";
        break;

      case TextAST::Type::TimeLeft:
        r << "\\timeLeft{}";
        break;

      case TextAST::Type::LotteryRupeePrompt:
        r << "\\lotteryPromptRupees{}";
        break;

      case TextAST::Type::BomberCodePrompt:
        r << "\\bombersPromptCode{}";
        break;

      case TextAST::Type::WaitOnItem:
        r << "\\waitForAnyItem{}";
        break;

      case TextAST::Type::SoaringDestination:
        r << "\\songSoaringDest{}";
        break;

      case TextAST::Type::LotteryGuessPrompt:
        r << "\\lotteryPromptGuess{}";
        break;

      case TextAST::Type::OceanSpiderMaskOrder:
        r << "\\showOceanSpiderMaskOrder{}";
        break;

      case TextAST::Type::FairiesLeftIn:
        r << "\\fairiesLeftAt{";

        switch (frag.getValue<uint32_t>()) {
          case 1:
            r << "Woodfall";
            break;

          case 2:
            r << "Snowhead";
            break;

          case 3:
            r << "Great Bay";
            break;

          case 4:
            r << "Stone Tower";
            break;

          default:
            r << "UNKNOWN!!! " << frag.getValue<uint32_t>();
            break;
        }

        r << "}";
        break;

      case TextAST::Type::SwampArchScore:
        r << "\\archerySwampScore{}";
        break;

      case TextAST::Type::ShowLotteryNumber:
        r << "\\lotteryCorrectAnswer{}";
        break;

      case TextAST::Type::ShowLotteryGuess:
        r << "\\lotteryPlayerAnswer{}";
        break;

      case TextAST::Type::MonetaryValue:
        r << "\\showValueOfItem{}";
        break;

      case TextAST::Type::ShowBomberCode:
        r << "\\showBomberCode{}";
        break;

      case TextAST::Type::EndConversation:
        r << "\\endConversation{}";
        break;

      case TextAST::Type::ShowMaskColor:
        r << "\\showColorOfOceanSpiderMask{" << frag.getValue<uint32_t>() << "}";
        break;

      case TextAST::Type::HoursLeft:
        r << "\\hoursRemaining{}";
        break;

      case TextAST::Type::TimeToMorning:
        r << "\\timeUntilMorning{}";
        break;

      case TextAST::Type::OctoArchHiscore:
        r << "\\archeryOctoHiscore{}";
        break;

      case TextAST::Type::BeanPrice:
        r << "\\priceOfBean{}";
        break;

      case TextAST::Type::EponaArchHiscore:
        r << "\\archeryEponaHiscore{}";
        break;

      case TextAST::Type::DekuFlowerGameDailyHiscore:
        r << "\\dekuFlowerGameHiscoreOnDay{" << frag.getValue<uint32_t>() << "}";
        break;
    }

    return r.str();
}