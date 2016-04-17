/** \file Hex/Widget.hpp
 *
 *  \brief Declaration of the widget actually displaying the hex data.
 *
 */

#pragma once

#include "ROM.hpp"
#include "Hex/Cursor.hpp"

#include <QAbstractScrollArea>
#include <QByteArray>

namespace Hex {
    class Widget : public QAbstractScrollArea {
        Q_OBJECT

      private:
        QByteArray showthis;

        Cursor cursor;
        bool blinkCover = false;
        int blinkID;

        size_t lines_page;

        size_t offsetDigits;
        size_t offsetWidth;
        size_t emWidth;
        size_t hexWidth;
        size_t textWidth;

      protected:
        virtual void paintEvent(QPaintEvent * ev) override;
        virtual void resizeEvent(QResizeEvent * ev) override;
        virtual void keyPressEvent(QKeyEvent * ev) override;

      public:
        Widget(QByteArray st);
    };
}