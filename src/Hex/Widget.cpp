/** \file Hex/Widget.cpp
 *
 *  \brief Implementation of hex viewing widget
 *
 */

#include "Hex/Widget.hpp"

#include <QFontDatabase>
#include <QFontMetrics>
#include <QScrollBar>
#include <QPainter>
#include <QApplication>
#include <QStyle>
#include <QResizeEvent>

#include <cstddef>
#include <cmath>

namespace Hex {
    // the cursor in this initialization list is just a hacky workaround, since
    // we really don't want a default constructor on Cursors (it really makes no
    // sense).
    Widget::Widget(QByteArray st) : showthis(st), cursor(0, 0) {
        // first is first, set us up for a monospace font
        setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

        // next, let's figure out how many rows we'll have, and how many digits
        // we'll need for the column listing offsets.
        size_t rows = (showthis.size() - 1) / 0x10 + 1;

        // floating-point truncation handled by conversion to size_t
        offsetDigits = std::log((rows - 1) * 0x10) / std::log(0x10) + 1;

        // now to calculate our minimum width
        QFontMetrics qfm(font());

        // we'd like an em width, but if we can't then we'll just use the height of
        // the font as a width
        if (qfm.inFontUcs4(0x2003)) {
            emWidth = qfm.width(QChar(0x2003));
        } else {
            emWidth = qfm.height();
        }

        offsetWidth = qfm.width(QString("0x%1").arg(QString(offsetDigits, '0'))) + emWidth;

        // byte width + left-of-byte space + group-of-4 extra spacing
        hexWidth = qfm.width("00") * 16 + emWidth * 16 + emWidth * 3;

        textWidth = qfm.width(QString(16, '0'));

        // maybe someday we'll be nice and handle the use of horizontal
        // scrollbar. Not today. The bit using pixelMetric is to account for the
        // all-too-likely vertical scrollbar.
        setMinimumWidth(offsetWidth + hexWidth + emWidth + textWidth + (emWidth / 2)
                        + QApplication::style()->pixelMetric(QStyle::PM_ScrollBarExtent));

        // now to calculate the number of lines we have per "page".
        lines_page = height() / qfm.lineSpacing();

        verticalScrollBar()->setPageStep(lines_page);
        verticalScrollBar()->setRange(0, rows - lines_page);

        // now that all that's out of the way, let us set up the other parts needed
        setFocusPolicy(Qt::StrongFocus);
        cursor = Cursor(showthis.size(), lines_page);
    }

    void Widget::resizeEvent(QResizeEvent * ev) {
        QFontMetrics qfm(font());

        lines_page = ev->size().height() / qfm.lineSpacing();

        size_t rows = (showthis.size() - 1) / 0x10 + 1;

        cursor.newPageSize(lines_page);

        verticalScrollBar()->setPageStep(lines_page);
        verticalScrollBar()->setRange(0, rows - lines_page);
    }

    void Widget::keyPressEvent(QKeyEvent * ev) {
        bool wentback = false;
        switch (ev->key()) {
          case Qt::Key_Left:
            cursor.prevByte();
            wentback = true;
            break;

          case Qt::Key_Right:
            cursor.nextByte();
            break;

          case Qt::Key_Down:
            cursor.nextLine();
            break;

          case Qt::Key_Up:
            cursor.prevLine();
            wentback = true;
            break;

          case Qt::Key_PageDown:
            cursor.nextPage();
            verticalScrollBar()->setValue(verticalScrollBar()->value() + lines_page);
            break;

          case Qt::Key_PageUp:
            cursor.prevPage();
            verticalScrollBar()->setValue(verticalScrollBar()->value() - lines_page);
            break;

          case Qt::Key_Home:
            cursor.goToStart();
            verticalScrollBar()->setValue(0);
            break;

          case Qt::Key_End:
            cursor.goToEnd();
            verticalScrollBar()->setValue(verticalScrollBar()->maximum());
            break;

          default:
            QAbstractScrollArea::keyPressEvent(ev);
            break;
        }

        // if we need to adjust the scrollbar, do so
        if (!cursor.onPage(verticalScrollBar()->value() * 0x10)) {
            if (wentback) {
                verticalScrollBar()->setValue(cursor.row());
            } else {
                verticalScrollBar()->setValue(cursor.row() - lines_page + 1);
            }
        }

        // now update the viewport
        viewport()->update();
    }

    void Widget::paintEvent(QPaintEvent * ev) {
        QPainter qp(viewport());
        QFontMetrics qfm(font());
        QPalette qpal = viewport()->palette();

        // first, fill the viewport with the correct background color for the usual
        // text input things
        qp.fillRect(qp.window(), qpal.base());

        // now, to fill in the offset section and draw the divider line
        QRect drawHere(0, 0, offsetWidth, height());

        qp.fillRect(drawHere, qpal.window());

        qp.setPen(QPen(qpal.window(), 1));
        qp.drawLine(offsetWidth + hexWidth + (emWidth / 2), 0,
                    offsetWidth + hexWidth + (emWidth / 2), viewport()->height());

        // and now for writing down text

        for (size_t i = 0; i < verticalScrollBar()->pageStep(); i++) {
            if ((i + verticalScrollBar()->value()) * 0x10 >= showthis.size()) {
                break;
            }


            qp.setPen(QPen(qpal.windowText(), 1));
            QPoint linecurse(emWidth / 2,
                             qfm.lineSpacing() * i + qfm.ascent());

            qp.drawText(linecurse, QString("0x%1").arg(
                            QString("%1").arg(
                                (i + verticalScrollBar()->value()) * 0x10, offsetDigits, 16, QChar('0'))
                            .toUpper()));

            // the funky emWidth - (emWidth / 2) is so we don't potentially lose a
            // pixel due to integer division truncation (e.g. emWidth == 5)
            linecurse.setX(offsetWidth);

            // use a second cursor so we only loop through the array once
            QPoint textcurse(linecurse);
            textcurse += QPoint(hexWidth + emWidth, 0);

            // this bit's a cheat so the byte-printing loop is a bit simpler
            linecurse -= QPoint(emWidth, 0);

            for (size_t j = 0; j < 16; j++) {
                // if this is a group of four, start by moving 2em to the right;
                // otherwise, 1em (hex side)
                if (j % 4 == 0) {
                    linecurse += QPoint(emWidth * 2, 0);
                } else {
                    linecurse += QPoint(emWidth, 0);
                }

                if ((i + verticalScrollBar()->value()) * 0x10 + j >= showthis.size()) {
                    break;
                }

                uint8_t thebyte = showthis.at((i + verticalScrollBar()->value()) * 0x10 + j);

                QString hexchar;
                QString textchar;

                if (thebyte < 0x20 || (0x7F <= thebyte && thebyte <= 0x9F)) {
                    textchar = ".";
                    // XXX better choice of alternate color
                    qp.setPen(QPen(qpal.link(), 1));
                } else {
                    textchar = QChar(thebyte);
                    qp.setPen(QPen(qpal.text(), 1));
                }

                hexchar = QString("%1").arg(QString("%1").arg(thebyte, 2, 16, QChar('0')).toUpper());

                qp.drawText(linecurse, hexchar);
                qp.drawText(textcurse, textchar);

                linecurse += QPoint(qfm.width(hexchar), 0);
                textcurse += QPoint(qfm.width(textchar), 0);
            }
        }

        // now that the text is drawn, finally draw the cursor
        if (cursor.onPage(verticalScrollBar()->value() * 0x10)) {
            QRect hexbox;
            QRect txtbox;
            size_t rr, cc;

            if (cursor.eof()) {
                cc = (cursor.col() - 1) % 0x10;
                rr = cursor.row();

                if (cursor.col() == 0) {
                    rr--;
                }
            } else {
                cc = cursor.col();
                rr = cursor.row();
            }

            hexbox.setY(qfm.lineSpacing() * (rr - verticalScrollBar()->value()));
            txtbox.setY(hexbox.y());

            hexbox.setX(offsetWidth + (emWidth + qfm.width("00")) * cc
                        + emWidth * (cc / 4 + 1));
            txtbox.setX(offsetWidth + hexWidth + emWidth + qfm.width("0") * cc);

            if (cursor.eof()) {
                hexbox.setX(hexbox.x() + qfm.width("00"));
                txtbox.setX(txtbox.x() + qfm.width("0"));

                hexbox.setWidth(2);
                txtbox.setWidth(2);
            } else {
                hexbox.setWidth(qfm.width("00"));
                txtbox.setWidth(qfm.width("0"));
            }

            hexbox.setHeight(qfm.height());
            txtbox.setHeight(qfm.height());

            if (cursor.eof()) {
                qp.setBrush(qpal.text());
            } else {
                qp.setBrush(QBrush());
            }

            qp.setPen(QPen(qpal.text(), 1));

            qp.drawRect(txtbox);

            if (blinkCover) {
                qp.setBrush(qpal.text());
            }

            qp.drawRect(hexbox);
        }
    }
}