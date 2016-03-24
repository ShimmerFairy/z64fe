/** \file TextViewer.cpp
 *
 *  \brief Implement text viewer
 *
 */

#include "TextViewer.hpp"
#include "endian.hpp"
#include "Exceptions.hpp"
#include "Shift-JIS.hpp"
#include "utility.hpp"

#include <QString>
#include <QMessageBox>

#include <sstream>
#include <iomanip>
#include <iostream>

TextViewer::TextViewer(ROM::ROM & r) : trom(&r) {
    setAttribute(Qt::WA_DeleteOnClose);

    idlist = new QTreeView;
    idmod = new TextIDModel(trom->msgTbl());
    msgview = new QTextEdit;
    qhb = new QHBoxLayout;
    dummy = new QWidget;

    idlist->setModel(idmod);
    idlist->setSelectionMode(QAbstractItemView::SingleSelection);

    msgview->setReadOnly(true);

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

        // the reason we put loops in side the conditional, instead of the
        // conditional inside one loop, is because the conditional will never
        // change, so checking it every cycle is redundant (not to mention
        // potentially dangerous, since it would allow switching between ASCII
        // and Shift-JIS mode when you shouldn't be able to).
        if (lang == Config::Language::JP) {
            while (keepGoing) {
                res += transSJIS(readptr, keepGoing);
            }
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

    r << std::uppercase << std::setfill('0');

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
                  << std::hex << std::setw(2) << +*takethis
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
            r << "\\UnknownTrigger{}";
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

std::string TextViewer::transSJIS(std::vector<uint8_t>::iterator & takethis, bool & cont) {
    std::stringstream res;

    res << std::uppercase;

    // note: unlike the ASCII handling above, we won't save one iterator
    // advancement for after all the branching; I think for this it'll be
    // ultimately easier to handle all the advancements in each outcome
    // appropriately.

    uint8_t first = *takethis++;
    uint8_t second, third, fourth; // these are to aid readability below

    // first, we try to handle the special control codes that besmirch the good
    // Shift-JIS name by using double-bytes where they're not allowed, and
    // having more-than-two-byte sequences.

    switch (first) {
      case 0x00:
        second = *takethis++;
        switch (second) {
          case 0x0A:
            res << "\n";
            break;

          case 0x0B:
            third = *takethis++;
            if (third == 0x0C) {
                fourth = *takethis++;
                res << "\\color{";
                switch (fourth) {
                  case 0x00:
                    res << "white";
                    break;

                  case 0x01:
                    res << "red";
                    break;

                  case 0x02:
                    res << "green";
                    break;

                  case 0x03:
                    res << "blue";
                    break;

                  case 0x04:
                    res << "cyan";
                    break;

                  case 0x05:
                    res << "magenta";
                    break;

                  case 0x06:
                    res << "yellow";
                    break;

                  case 0x07:
                    res << "black";
                    break;

                  default:
                    res << "UNKNOWN! (0x"
                        << std::hex << std::setw(2) << +fourth
                        << ")";
                    break;
                }
                res << "}";
            } else {
                res << "\\badSequence{000B" << std::hex << std::setw(2) << +third << "}";
            }
            break;

          default:
            // at this point, this 00 has to be a null character, might as well
            // take care of it now (and undo the advancement after second, since
            // that second byte turns out not to be ours)
            res << "\0{}";
            takethis--;
            break;
        }
        break;

      case 0x81:
        second = *takethis++;
        switch (second) {
          case 0x70:
            res << "\\endMsg{}";
            cont = false;
            break;

          case 0xA5:
            res << "\\newBox{}";
            break;

          case 0xCB:
            // third and fourth implicitly in be_u16, so not assigned in this
            // case
            res << "\\goto{0x" << std::hex << std::setw(4) << be_u16(takethis) << "}";
            takethis += 2; // advance past ID bytes (third & fourth)
            break;

          case 0x89:
            res << "\\instantTextOn{}";
            break;

          case 0x8A:
            res << "\\instantTextOff{}";
            break;

          case 0x9F:
            res << "\\UnknownTrigger{}";
            break;

          case 0xA3:
            third = *takethis++;

            if (third == 0x00) {
                fourth = *takethis++;
                res << "\\sleepFrames{" << std::dec << +fourth << "}";
            } else {
                res << "\\badSequence{81A3" << std::hex << std::setw(2) << +third << "}";
            }
            break;

          case 0x9E:
            third = *takethis++;

            if (third == 0x00) {
                fourth = *takethis++;
                res << "\\sleepFramesThenFade{" << std::dec << +fourth << "}";
            } else {
                res << "\\badSequence{819E" << std::hex << std::setw(2) << +third << "}";
            }
            break;

          case 0xF0:
            res << "\\startOcarina{}";
            break;

          case 0xF3:
            // third and fourth implied in be_u16
            res << "\\sfx{0x" << std::hex << std::setw(4) << be_u16(takethis) << "}";
            takethis += 2; // advance past third and fourth
            break;

          case 0x9A:
            third = *takethis++;

            if (third == 0x00) {
                fourth = *takethis++;
                res << "\\icon{" << std::dec << +fourth << "}";
            } else {
                res << "\\badSequence{819A" << std::hex << std::setw(2) << +third << "}";
            }
            break;

          case 0x99:
            res << "\\cantSkipNow{}";
            break;

          case 0xBC:
            res << "\\twoChoices{}";
            break;

          case 0xB8:
            res << "\\threeChoices{}";
            break;

          case 0xA1:
            res << "\\worldTime{}";
            break;

          default:
            // otherwise, we know it has to be a normal two-byte Shift-JIS
            // character.

            res << code_to_utf8(SJIS_doubleTable.at(first).at(second));
            break;
        }
        break;

      case 0x83:
        second = *takethis++;

        switch (second) {
          case 0x9F:
            res << "\\A{}";
            break;
          case 0xA0:
            res << "\\B{}";
            break;
          case 0xA1:
            res << "\\C{}";
            break;
          case 0xA2:
            res << "\\L{}";
            break;
          case 0xA3:
            res << "\\R{}";
            break;
          case 0xA4:
            res << "\\Z{}";
            break;
          case 0xA5:
            res << "\\Cup{}";
            break;
          case 0xA6:
            res << "\\Cdown{}";
            break;
          case 0xA7:
            res << "\\Cleft{}";
            break;
          case 0xA8:
            res << "\\Cright{}";
            break;
          case 0xA9:
            res << "▼";
            break;
          case 0xAA:
            res << "\\analogStick{}";
            break;
          case 0xAB:
            res << "\\dPad{}";
            break;
          default:
            res << code_to_utf8(SJIS_doubleTable.at(first).at(second));
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

                res << "\\spaces{" << std::dec << +fourth << "}";
            } else {
                res << "\\badSequence{86C7" << std::hex << std::setw(2) << +third << "}";
            }
            break;

          case 0xC8:
            res << "\\stayOpen{}";
            break;

          case 0xC9:
            third = *takethis++;

            if (third == 0x00) {
                fourth = *takethis++;

                res << "\\textSpeed{" << std::dec << +fourth << "}";
            } else {
                res << "\\badSequence{86C9" << std::hex << std::setw(2) << +third << "}";
            }
            break;

          case 0xB3:
            third = *takethis++;

            if (third == 0x00) {
                // this value takes up three bytes (fourth, fifth, and
                // sixth!). We'll be using be_u24 to get them, though.

                res << "\\msgBackground{0x" << std::hex << std::setw(6) << be_u24(takethis) << "}";

                takethis += 3;
            } else {
                res << "\\badSequence{86B3" << std::hex << std::setw(2) << +third << "}";
            }
            break;

          case 0xA3:
            res << "\\numberOfGoldSkulltulas{}";
            break;

          case 0xA4:
            res << "\\fishWeight{}";
            break;

          case 0x9F:
            third = *takethis++;

            if (third == 0x00) {
                fourth = *takethis++;

                res << "\\hiscore{0x" << std::hex << std::setw(2) << +fourth << "}";
            } else {
                res << "\\badSequence{869F" << std::hex << std::setw(2) << +third << "}";
            }
            break;

          default:
            // no two-bytes start with 86, so give back the second and mark as bad
            res << "\\badSequence{86}";
            takethis--;
            break;
        }
        break;

      case 0x87:
        second = *takethis++;

        switch (second) {
          case 0x4F:
            res << "\\playerName{}";
            break;

          case 0x91:
            res << "\\marathonTime{}";
            break;

          case 0x92:
            res << "\\raceTime{}";
            break;

          case 0x9B:
            res << "\\numberOfPoints{}";
            break;

          default:
            // no sequences start with 87 in normal shift-JIS, so give back
            // second and mark as bad
            res << "\\badSequence{87}";
            takethis--;
            break;
        }
        break;

      default:
        // first, try multi-byte
        if (SJIS_doubleTable.count(first) == 1) {
            second = *takethis++;

            res << code_to_utf8(SJIS_doubleTable.at(first).at(second));
        } else if (SJIS_singleTable.count(first) == 1) {
            // single-byte with special mapping

            res << code_to_utf8(SJIS_singleTable.at(first));
        } else {
            // plain ol' ASCII
            res << first;
        }
        break;
    }

    return res.str();
}