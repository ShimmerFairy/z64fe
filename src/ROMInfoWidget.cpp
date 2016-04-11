/** \file ROMInfoWidget.cpp
 *
 */

#include "ROMInfoWidget.hpp"
#include "utility.hpp"

ROMInfoWidget::ROMInfoWidget(QWidget * parent) : QWidget(parent) {
    intname_key = new QLabel(tr("Internal Name:"));
    intname_val = new QLabel;
    intcode_key = new QLabel(tr("Internal Code:"));
    intcode_val = new QLabel;
    size_key = new QLabel(tr("Size:"));
    size_val = new QLabel;

    savebs = new QPushButton(tr("No ROM Loaded"));
    savebs->setEnabled(false);

    wlay = new QGridLayout;

    wlay->addWidget(intname_key, 0, 0, 1, 1, Qt::AlignRight);
    wlay->addWidget(intname_val, 0, 1, 1, 1, Qt::AlignLeft);

    wlay->addWidget(intcode_key, 1, 0, 1, 1, Qt::AlignRight);
    wlay->addWidget(intcode_val, 1, 1, 1, 1, Qt::AlignLeft);

    wlay->addWidget(size_key, 2, 0, 1, 1, Qt::AlignRight);
    wlay->addWidget(size_val, 2, 1, 1, 1, Qt::AlignLeft);

    wlay->addWidget(savebs, 3, 0, 1, 2);

    setLayout(wlay);
}

void ROMInfoWidget::changeROM(ROM::ROM * nr) {
    the_rom = nr;

    if (the_rom->wasByteswapped()) {
        savebs->setEnabled(true);
        savebs->setText(tr("&Save un-byteswapped ROM..."));
    } else {
        savebs->setEnabled(false);
        savebs->setText(tr("Wasn't byteswapped"));
    }

    intname_val->setText(the_rom->get_rname().c_str());
    intcode_val->setText(the_rom->get_rcode().c_str());

    size_val->setText(sizeToIEC(the_rom->size()).c_str());
}