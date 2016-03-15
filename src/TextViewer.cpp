/** \file TextViewer.cpp
 *
 *  \brief Implement text viewer
 *
 */

#include "TextViewer.hpp"

#include <map>
#include <QString>

TextViewer::TextViewer(ROMFile td) : tdata(td) {
    viewer = new QTextEdit;
    viewer->setReadOnly(true);
    asciibtn = new QPushButton(tr("&ASCII"));
    sjisbtn = new QPushButton(tr("&Shift-JIS"));

    qhb = new QHBoxLayout;

    qhb->addWidget(asciibtn);
    qhb->addStretch();
    qhb->addWidget(sjisbtn);

    qvb = new QVBoxLayout;

    qvb->addLayout(qhb);
    qvb->addWidget(viewer);

    dummy = new QWidget;
    dummy->setLayout(qvb);

    setCentralWidget(dummy);

    connect(asciibtn, &QPushButton::clicked, this, &TextViewer::transASCII);
    connect(sjisbtn, &QPushButton::clicked, this, &TextViewer::transShiftJIS);
}

void TextViewer::transASCII() {
    viewer->clear();

    std::map<uint8_t, QString> specialChar{
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

    for (size_t i = 0; i < tdata.size(); i++) {
        if (tdata.at(i) < 0x20) {
            switch (tdata.at(i)) {
              case 0x00:
                viewer->insertPlainText("\\0{}");
                break;

              case 0x01:
                viewer->insertPlainText("\n");
                break;

              case 0x02:
                viewer->insertPlainText("\\endMsg{}");
                break;

              case 0x04:
                viewer->insertPlainText("\\newBox{}");
                break;

              case 0x05:
                viewer->insertPlainText("\\color{");
                switch (tdata.at(++i)) {
                  case 0x40:
                    viewer->insertPlainText("white");
                    break;

                  case 0x41:
                    viewer->insertPlainText("red");
                    break;

                  case 0x42:
                    viewer->insertPlainText("green");
                    break;

                  case 0x43:
                    viewer->insertPlainText("blue");
                    break;

                  case 0x44:
                    viewer->insertPlainText("cyan");
                    break;

                  case 0x45:
                    viewer->insertPlainText("magenta");
                    break;

                  case 0x46:
                    viewer->insertPlainText("yellow");
                    break;

                  case 0x47:
                    viewer->insertPlainText("black");
                    break;

                  default:
                    viewer->insertPlainText(QString("UNKNOWN! (0x%1)").arg(tdata.at(i), 2, 16, QChar('0')));
                    break;
                }
                viewer->insertPlainText("}");
                break;

              case 0x06:
                viewer->insertPlainText(QString("\\spaces{%1}").arg(tdata.at(++i)));
                break;

              case 0x07:
                viewer->insertPlainText(QString("\\goto{0x%1").arg(tdata.at(++i), 2, 16, QChar('0')));
                viewer->insertPlainText(QString("%1}").arg(tdata.at(++i), 2, 16, QChar('0')));
                break;

              case 0x08:
                viewer->insertPlainText("\\instantTextOn{}");
                break;

              case 0x09:
                viewer->insertPlainText("\\instantTextOff{}");
                break;

              case 0x0A:
                viewer->insertPlainText("\\stayOpen{}");
                break;

              case 0x0B:
                viewer->insertPlainText("\\UnknownTrigger0B{}");
                break;

              case 0x0C:
                viewer->insertPlainText(QString("\\sleepFrames{%1}").arg(tdata.at(++i)));
                break;

              case 0x0D:
                viewer->insertPlainText("\\waitOnButton{}");
                break;

              case 0x0E:
                viewer->insertPlainText(QString("\\sleepFramesThenFade{%1}").arg(tdata.at(++i)));
                break;

              case 0x0F:
                viewer->insertPlainText("\\playerName{}");
                break;

              case 0x10:
                viewer->insertPlainText("\\startOcarina{}");
                break;

              case 0x11:
                viewer->insertPlainText("\\bailAndFadeOutNow{}");
                break;

              case 0x12:
                viewer->insertPlainText(QString("\\sfx{0x%1").arg(tdata.at(++i), 2, 16, QChar('0')));
                viewer->insertPlainText(QString("%1}").arg(tdata.at(++i), 2, 16, QChar('0')));
                break;

              case 0x13:
                viewer->insertPlainText(QString("\\icon{%1}").arg(tdata.at(++i)));
                break;

              case 0x14:
                viewer->insertPlainText(QString("\\textSpeed{%1}").arg(tdata.at(++i)));
                break;

              case 0x15:
                viewer->insertPlainText(QString("\\msgBackground{0x%1").arg(tdata.at(++i), 2, 16, QChar('0')));
                viewer->insertPlainText(QString("%1").arg(tdata.at(++i), 2, 16, QChar('0')));
                viewer->insertPlainText(QString("%1}").arg(tdata.at(++i), 2, 16, QChar('0')));
                break;

              case 0x16:
                viewer->insertPlainText("\\marathonTime{}");
                break;

              case 0x17:
                viewer->insertPlainText("\\raceTime{}");
                break;

              case 0x18:
                viewer->insertPlainText("\\numberOfPoints{}");
                break;

              case 0x19:
                viewer->insertPlainText("\\numberOfGoldSkulltulas{}");
                break;

              case 0x1A:
                viewer->insertPlainText("\\cantSkipNow{}");
                break;

              case 0x1B:
                viewer->insertPlainText("\\twoChoices{}");
                break;

              case 0x1C:
                viewer->insertPlainText("\\threeChoices{}");
                break;

              case 0x1D:
                viewer->insertPlainText("\\fishWeight{}");
                break;

              case 0x1E:
                viewer->insertPlainText(QString("\\hiscore{0x%1}").arg(tdata.at(++i), 2, 16, QChar('0')));
                break;

              case 0x1F:
                viewer->insertPlainText("\\worldTime{}");
                break;
            }
        } else if (0x20 <= tdata.at(i) && tdata.at(i) <= 0x7E) {
            if (tdata.at(i) == 0x5C) {
                viewer->insertPlainText("¥");
            } else {
                viewer->insertPlainText(QChar(tdata.at(i)));
            }
        } else if (0x7F <= tdata.at(i) && tdata.at(i) <= 0xAB) {
            viewer->insertPlainText(specialChar[tdata.at(i)]);
        } else {
            viewer->insertPlainText(QString("\\x{%1}").arg(tdata.at(i), 2, 16, QChar('0')));
        }
    }            
}

void TextViewer::transShiftJIS() {

}