/** \file ROMInfoWidget.hpp
 *
 *  \brief Declares a widget displaying info on the ROM as a whole, and possible
 *         associated operations.
 *
 *  The widget declared here displays useful tidbits taken from the header and
 *  ROM construction process, and allows for any possible operations on the ROM
 *  as a whole.
 *
 */

#pragma once

#include "ROM.hpp"

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

class ROMInfoWidget : public QWidget {
    Q_OBJECT

  private:
    ROM::ROM * the_rom;

    QGridLayout * wlay;

    QLabel * intname_key;
    QLabel * intname_val;
    QLabel * intcode_key;
    QLabel * intcode_val;
    QLabel * size_key;
    QLabel * size_val;

    QPushButton * savebs;

  private slots:
    void saveROM();

  public slots:
    void changeROM(ROM::ROM * nr);

  public:
    ROMInfoWidget(QWidget * parent = nullptr);
};