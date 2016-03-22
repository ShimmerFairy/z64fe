/** \file TextViewer.cpp
 *
 *  \brief Implement text viewer
 *
 */

#include "TextViewer.hpp"
#include "endian.hpp"
#include "Exceptions.hpp"

#include <QString>
#include <QMessageBox>

#include <sstream>
#include <iomanip>
#include <iostream>

TextViewer::TextViewer(ROM::ROM & r) : trom(&r) {
    idlist = new QTreeView;
    idmod = new TextIDModel(trom->msgTbl());
    msgview = new QTextEdit;
    qhb = new QHBoxLayout;
    dummy = new QWidget;

    idlist->setModel(idmod);
    idlist->setSelectionMode(QAbstractItemView::SingleSelection);

    qhb->addWidget(idlist);
    qhb->addWidget(msgview);

    dummy->setLayout(qhb);

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
        uint32_t address = idmod->data(sel, TextIDModel::rawRole).toUInt();
        Config::Language lang = static_cast<Config::Language>(idmod->data(sel.parent(), TextIDModel::rawRole).toUInt());

        ROM::File msgfile;

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
            }
        } catch (Exception & e) {
            QMessageBox::critical(this, tr("ERROR!"),
                                  e.what().c_str());
            std::exit(-1);
        }

        std::vector<uint8_t> the_text;

        auto readptr = msgfile.begin() + address;

        bool keepGoing = true;
        std::string res;

        if (lang == Config::Language::JP) {
            //...
        } else {
            while (keepGoing) {
                res += transASCII(readptr, keepGoing);
            }
        }

        msgview->setPlainText(res.c_str());
    }
}

std::string TextViewer::transASCII(std::vector<uint8_t>::iterator & takethis, bool & cont) {
    std::stringstream r;

    r << std::uppercase;

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
        { 0x9E, "ü" },
        { 0x9F, "\\A{}" },
        { 0xA0, "\\B{}" },
        { 0xA1, "\\C{}" },
        { 0xA2, "\\L{}" },
        { 0xA3, "\\R{}" },
        { 0xA4, "\\Z{}" },
        { 0xA5, "\\Cup{}" },
        { 0xA6, "\\Cdown{}" },
        { 0xA7, "\\Cleft{}" },
        { 0xA8, "\\Cright{}" },
        { 0xA9, "▼" },
        { 0xAA, "\\analogStick{}" },
        { 0xAB, "\\dPad{}" }};

    if (*takethis < 0x20) {
        switch (*takethis) {
          case 0x00:
            r << "\\0{}";
            break;

          case 0x01:
            r << "\n";
            break;

          case 0x02:
            r << "\\endMsg{}";
            cont = false;
            break;

          case 0x04:
            r << "\\newBox{}";
            break;

          case 0x05:
            r << "\\color{";
            switch (*(++takethis)) {
              case 0x40:
                r << "white";
                break;

              case 0x41:
                r << "red";
                break;

              case 0x42:
                r << "green";
                break;

              case 0x43:
                r << "blue";
                break;

              case 0x44:
                r << "cyan";
                break;

              case 0x45:
                r << "magenta";
                break;

              case 0x46:
                r << "yellow";
                break;

              case 0x47:
                r << "black";
                break;

              default:
                r << "UNKNOWN! (0x"
                  << std::hex << std::setfill('0') << std::setw(2) << +*takethis
                  << ")";
                break;
            }
            r << "}";
            break;

          case 0x06:
            r << "\\spaces{" << std::dec << +*takethis << "}";
            break;

          case 0x07:
            r << "\\goto{0x" << std::hex << std::setw(4) << be_u16(takethis + 1) << "}";
            takethis += 2;
            break;

          case 0x08:
            r << "\\instantTextOn{}";
            break;

          case 0x09:
            r << "\\instantTextOff{}";
            break;

          case 0x0A:
            r << "\\stayOpen{}";
            break;

          case 0x0B:
            r << "\\UnknownTrigger0B{}";
            break;

          case 0x0C:
            r <<"\\sleepFrames{" << std::dec << +*(++takethis) << "}";
            break;

          case 0x0D:
            r << "\\waitOnButton{}";
            break;

          case 0x0E:
            r << "\\sleepFramesThenFade{" << std::dec << +*(++takethis) << "}";
            break;

          case 0x0F:
            r << "\\playerName{}";
            break;

          case 0x10:
            r << "\\startOcarina{}";
            break;

          case 0x11:
            r << "\\bailAndFadeOutNow{}";
            break;

          case 0x12:
            r << "\\sfx{0x" << std::hex << std::setw(4) << be_u16(takethis + 1) << "}";
            takethis += 2;
            break;

          case 0x13:
            r << "\\icon{" << std::dec << +*(++takethis) << "}";
            break;

          case 0x14:
            r << "\\textSpeed{" << std::dec << +*(++takethis) << "}";
            break;

          case 0x15:
            r << "\\msgBackground{0x" << std::hex << std::setw(6) << be_u24(takethis + 1) << "}";
            takethis += 3;
            break;

          case 0x16:
            r << "\\marathonTime{}";
            break;

          case 0x17:
            r << "\\raceTime{}";
            break;

          case 0x18:
            r << "\\numberOfPoints{}";
            break;

          case 0x19:
            r << "\\numberOfGoldSkulltulas{}";
            break;

          case 0x1A:
            r << "\\cantSkipNow{}";
            break;

          case 0x1B:
            r << "\\twoChoices{}";
            break;

          case 0x1C:
            r << "\\threeChoices{}";
            break;

          case 0x1D:
            r << "\\fishWeight{}";
            break;

          case 0x1E:
            r << "\\hiscore{0x" << std::hex << std::setw(2) << +*(++takethis) << "}";
            break;

          case 0x1F:
            r << "\\worldTime{}";
            break;
        }
    } else if (0x20 <= *takethis && *takethis <= 0x7E) {
        if (*takethis == 0x5C) {
            r << "¥";
        } else {
            r << *takethis;
        }
    } else if (0x7F <= *takethis && *takethis <= 0xAB) {
        r << specialChar.at(*takethis);
    } else {
        r << "\\x{" << std::hex << std::setw(2) << +*takethis << "}";
    }

    takethis++;

    return r.str();
}