/** \file HexViewer.hpp
 *
 *  \brief A basic file viewer, displaying in hex
 *
 */

#pragma once

#include "ROM.hpp"

#include <QAbstractScrollArea>
#include <QByteArray>

class HexViewer : public QAbstractScrollArea {
    Q_OBJECT

  private:
    QByteArray showthis;

    size_t offsetDigits;
    size_t offsetWidth;
    size_t emWidth;
    size_t hexWidth;
    size_t textWidth;

  protected:
    virtual void paintEvent(QPaintEvent * ev) override;
    virtual void resizeEvent(QResizeEvent * ev) override;
  public:
    HexViewer(QByteArray st);
};