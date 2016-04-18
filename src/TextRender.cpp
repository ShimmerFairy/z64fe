/** \file TextRender.cpp
 *
 *  \brief Implements the renderer
 *
 */

#include "TextRender.hpp"

#include <QPainter>
#include <QFontMetricsF>
#include <QtDebug>
#include <QIcon>
#include <QTime>
#include <QLinearGradient>

TextRender::TextRender() {
    // set up size constraints; the +2 is for the border, so we don't lose any
    // virtual screen space on the border.
    //setMinimumSize(320 + 2, 240 + 2);
    //setMaximumSize(640 + 2, 480 + 2);

    setFixedSize(322, 242); // fixed until I find a way to keep the fucking
                            // aspect ratio

    // set up border
    setFrameShape(QFrame::Box);
}

void TextRender::newText(TextAST::MsgInfo nmi, std::vector<TextAST::Box> np) {
    parts = np;
    minfo = nmi;

    update();
}

// the important function™

void TextRender::paintEvent(QPaintEvent * ev) {
    // first of all, ask qframe to do its border thing
    QFrame::paintEvent(ev);

    // until we collect message box types (one thing at a time!), we'll just
    // draw a black box at the bottom for all messages

    QSize actual = size();

    Q_ASSERT((actual.width() - 2) / 320.0 == (actual.height() - 2) / 240.0);

    double scalefac = (actual.width() - 2) / 320.0;

    QPainter qp(this);
    qp.scale(scalefac, scalefac);

    qp.translate(1, 1); // get away from the border

    QFont qf("sans", 8);
    QFontMetricsF qfmet(qf);

    qp.setFont(qf);

    // if the list is empty, do nothing but color the area in a vaguely
    // "disabled" fashion
    if (parts.size() == 0) {
        qp.fillRect(0, 0, 320, 240, QBrush(Qt::BDiagPattern));
        return;
    } else {
        QLinearGradient qg(0, 0, 320, 240);
        qg.setColorAt(0.0,     QColor(Qt::red).lighter());
        qg.setColorAt(1.0/5.0, QColor(Qt::yellow).lighter());
        qg.setColorAt(2.0/5.0, QColor(Qt::green).lighter());
        qg.setColorAt(3.0/5.0, QColor(Qt::cyan).lighter());
        qg.setColorAt(4.0/5.0, QColor(Qt::blue).lighter());
        qg.setColorAt(1.0,     QColor(Qt::magenta).lighter());

        qp.fillRect(0, 0, 320, 240, QBrush(qg));
    }

    // first we figure out our box properties
    QBrush boxcolor;
    QPointF boxpos(34, 0);
    QSizeF boxsize(256, 64);
    qreal xrad = 8.0, yrad = 8.0;
    QPen defColor = QColor(Qt::white);

    switch (minfo.kind) {
      case TextAST::BoxKind::BlackBox:
        boxcolor = QColor(0, 0, 0, 0xC0);
        break;

      case TextAST::BoxKind::WoodenBox:
        // XXX should be wood texture of some kind
        boxcolor = QColor(0x6A, 0x43, 0x00, 0xFF);
        break;

      case TextAST::BoxKind::BlueBox:
        boxcolor = QColor(0x19, 0x24, 0xC2, 0x80);
        break;

      case TextAST::BoxKind::OcarinaInput:
        // this style is like BlackBox, but without rounded corners
        boxcolor = QColor(0, 0, 0, 0xC0);
        xrad = yrad = 0.0;
        break;

      case TextAST::BoxKind::Nothing:
        boxcolor = Qt::transparent;
        break;

      case TextAST::BoxKind::NothingAndBlackFont:
        boxcolor = Qt::transparent;
        defColor = QColor(Qt::black);
        break;

      case TextAST::BoxKind::NothingAndTextTop:
        boxcolor = Qt::transparent;
        break;

      case TextAST::BoxKind::RedBox:
        // XXX unsure of this one, must investigate
        boxcolor = QColor(0xFF, 0, 0, 0xC0);
        break;

      case TextAST::BoxKind::Notebook:
        // XXX texture needed
        boxcolor = QColor(0xEF, 0xE5, 0xB8, 0xC0);
        defColor = QColor(Qt::black);
        break;

      case TextAST::BoxKind::InNotebook:
        boxsize = QSizeF(284, 48);
        xrad = 12.0;
        boxcolor = QColor(Qt::white);
        defColor = QColor(Qt::black);
        boxpos.setX(19);
        break;

      case TextAST::BoxKind::MM_DEFER:
        throw X::Text::HeaderError();
        break;
    }

    if (minfo.kind == TextAST::BoxKind::NothingAndTextTop) {
        boxpos.setY(38);
    } else {
        switch (minfo.where) {
          case TextAST::BoxYPos::TopOrBottom:
            // for now, just assume bottom for this one
          case TextAST::BoxYPos::Bottom:
            if (minfo.kind == TextAST::BoxKind::InNotebook) {
                boxpos.setY(185);
            } else {
                boxpos.setY(142);
            }
            break;

          case TextAST::BoxYPos::Top:
            boxpos.setY(38);
            break;

          case TextAST::BoxYPos::Middle:
            // XXX couldn't find confirming screenshot for position of centered
            // boxes, so a reasonable value assumed
            boxpos.setY(88);
            break;

          case TextAST::BoxYPos::MM_DEFER:
            throw X::Text::HeaderError();
            break;
        }
    }

    // start with our box, placed and colored appropriately (apparently the
    // x-pos is two off from center, who knows why).
    qp.setBrush(boxcolor);
    qp.setPen(Qt::NoPen);

    qp.drawRoundedRect(QRectF(boxpos, boxsize), xrad, yrad);

    qp.setPen(defColor);

    // set up cursor with proper initial position to let us move it relatively
    // speaking (at least in the initial setup)
    QPointF cursor = boxpos;

    // text is 32px off from left edge (presumably right too, but right-aligning
    // would have to be done manually so it doesn't matter)
    cursor += QPointF(32, 0);

    // now to discover where to center things vertically; we'll find the top of
    // the first line when centered by getting the number of lines for the
    // box. (We currently concern ourselves with just the first box.)

    // don't just use lineSpacing() for this, since we don't need the last line's
    // leading added on
    double textWants = qfmet.height() * parts.front().size()
                     + qfmet.leading() * (parts.front().size() - 1);

    // the addition of the ascent is so we have the cursor pointing at baseline.
    cursor += QPointF(0, ((boxsize.height() - textWants) / 2) + qfmet.ascent());

    // and finally, text drawing! (for now, just the first box)

    for (auto & i : parts.front()) {
        // process current line
        for (auto & j : i) {
            std::string curlit;
            QIcon curcon;

            switch (j.getType()) {
              case TextAST::Type::Color:
                switch (j.getValue<TextAST::Color>()) {
                  case TextAST::Color::White:
                    qp.setPen(defColor);
                    break;

                  case TextAST::Color::Red:
                    qp.setPen(QColor(Qt::red));
                    break;

                  case TextAST::Color::Green:
                    qp.setPen(QColor(Qt::green));
                    break;

                  case TextAST::Color::Blue:
                    // full-blue is too saturated/dark/annoying/etc. in this
                    // context, and OoT/MM don't go that route in the first
                    // place.
                    qp.setPen(QColor(0x40, 0x60, 0xC6));
                    break;

                  case TextAST::Color::Cyan:
                    qp.setPen(QColor(Qt::cyan));
                    break;

                  case TextAST::Color::Magenta:
                    qp.setPen(QColor(Qt::magenta));
                    break;

                  case TextAST::Color::Yellow:
                    qp.setPen(QColor(Qt::yellow));
                    break;

                  case TextAST::Color::Black:
                    qp.setPen(QColor(Qt::black));
                    break;

                  case TextAST::Color::Gray:
                    qp.setPen(QColor(Qt::gray));
                    break;

                  case TextAST::Color::Orange:
                    qp.setPen(QColor("orange"));
                    break;
                }
                break;

              case TextAST::Type::Literal:                
                curlit = j.getValue<std::string>();
                qp.drawText(cursor, curlit.c_str());
                cursor += QPointF(qfmet.width(curlit.c_str()), 0);
                break;

              case TextAST::Type::Button:
                switch (j.getValue<TextAST::Button>()) {
                  case TextAST::Button::A:
                    curcon = QIcon(":/controller/buttonA.svg");
                    break;

                  case TextAST::Button::B:
                    curcon = QIcon(":/controller/buttonB.svg");
                    break;

                  case TextAST::Button::C:
                    curcon = QIcon(":/controller/buttonC.svg");
                    break;

                  case TextAST::Button::L:
                    curcon = QIcon(":/controller/buttonL.svg");
                    break;

                  case TextAST::Button::R:
                    curcon = QIcon(":/controller/buttonR.svg");
                    break;

                  case TextAST::Button::Z:
                    curcon = QIcon(":/controller/buttonZ.svg");
                    break;

                  case TextAST::Button::C_UP:
                    curcon = QIcon(":/controller/buttonCUP.svg");
                    break;

                  case TextAST::Button::C_DOWN:
                    curcon = QIcon(":/controller/buttonCDOWN.svg");
                    break;

                  case TextAST::Button::C_LEFT:
                    curcon = QIcon(":/controller/buttonCLEFT.svg");
                    break;

                  case TextAST::Button::C_RIGHT:
                    curcon = QIcon(":/controller/buttonCRIGHT.svg");
                    break;

                  case TextAST::Button::ASTICK:
                    curcon = QIcon(":/controller/analog.svg");
                    break;

                  case TextAST::Button::DPAD:
                    curcon = QIcon(":/controller/buttonDPAD.svg");
                    break;
                }

                curcon.paint(&qp, QRectF(cursor - QPointF(0, qfmet.ascent()),
                                         cursor + QPointF(qfmet.height(), qfmet.descent())).toRect());

                // move cursor after picture (expect a space in text data to be after button)
                cursor += QPointF(qfmet.height(), 0);
                break;

              case TextAST::Type::Multispace:
                // since it's just a bunch of spaces, we won't bother to actually
                // print them; just move the cursor! Yay!
                cursor += QPointF(j.getValue<uint32_t>(), 0);
                break;

              case TextAST::Type::PlayerName:
                qf.setUnderline(true);
                qp.setFont(qf);

                qp.drawText(cursor, "Link");
                cursor += QPointF(qfmet.width("Link"), 0);

                qf.setUnderline(false);
                qp.setFont(qf);
                break;

              case TextAST::Type::WorldTime:
                curlit = QTime::currentTime().toString("HH:mm").toStdString();

                qp.drawText(cursor, curlit.c_str());
                cursor += QPointF(qfmet.width(curlit.c_str()), 0);
                break;

              default:
                // do nothing
                break;
            }
        }

        // now place newline
        cursor.setX(boxpos.x() + 32);
        cursor += QPointF(0, qfmet.lineSpacing());
    }
}