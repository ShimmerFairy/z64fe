/** \file ROMInfoWidget.cpp
 *
 */

#include "ROMInfoWidget.hpp"
#include "utility.hpp"

#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QFontDatabase>

ROMInfoWidget::ROMInfoWidget(QWidget * parent) : QWidget(parent) {
    intname_key = new QLabel(tr("Internal Name:"));
    intname_val = new QLabel;
    intcode_key = new QLabel(tr("Internal Code:"));
    intcode_val = new QLabel;
    size_key = new QLabel(tr("Size:"));
    size_val = new QLabel;
    crc_key = new QLabel(tr("CRC:"));
    crc_val = new QLabel;

    crc_val->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    savebs = new QPushButton(tr("No ROM Loaded"));
    savebs->setEnabled(false);

    wlay = new QGridLayout;

    wlay->addWidget(intname_key, 0, 0, 1, 1, Qt::AlignRight);
    wlay->addWidget(intname_val, 0, 1, 1, 1, Qt::AlignLeft);

    wlay->addWidget(intcode_key, 1, 0, 1, 1, Qt::AlignRight);
    wlay->addWidget(intcode_val, 1, 1, 1, 1, Qt::AlignLeft);

    wlay->addWidget(size_key, 2, 0, 1, 1, Qt::AlignRight);
    wlay->addWidget(size_val, 2, 1, 1, 1, Qt::AlignLeft);

    wlay->addWidget(crc_key, 3, 0, 1, 1, Qt::AlignRight);
    wlay->addWidget(crc_val, 3, 1, 1, 1, Qt::AlignLeft);

    wlay->addWidget(savebs, 4, 0, 1, 2);

    setLayout(wlay);

    /***************
     * CONNECTIONS *
     ***************/

    connect(savebs, &QPushButton::clicked, this, &ROMInfoWidget::saveROM);
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

    crc_val->setText(QString("%1 %2").arg(
                         QString("%1").arg(the_rom->getCRC().first, 8, 16, QChar('0')).toUpper())
                     .arg(
                         QString("%1").arg(the_rom->getCRC().second, 8, 16, QChar('0')).toUpper()));
}

void ROMInfoWidget::saveROM() {
    QSettings qs;

    QString saveto = QFileDialog::getSaveFileName(this, tr("Save ROM"),
                                                  qs.value("main/last_save_rom").toString(),
                                                  tr("N64 ROM Files (*.z64);;Any Files (*)"));

    if (saveto == "") {
        return;
    }

    std::ofstream writeto(saveto.toStdString(), std::ios::binary);

    if (!writeto) {
        // use parentWidget() so we don't possibly center over the dock widget
        // awkwardly
        QMessageBox::critical(parentWidget(), tr("Error in Saving"),
                              tr("Couldn't open file \"%1\" for writing!").arg(saveto));
        return;
    }

    qs.setValue("main/last_save_rom", saveto);

    std::vector<uint8_t> thedata = the_rom->getData();

    writeto.write(reinterpret_cast<char *>(thedata.data()), thedata.size());

    if (!writeto) {
        QMessageBox::warning(parentWidget(), tr("Possible Error in Saving"),
                             tr("Something went wrong in writing the file, the saved file may be incomplete."));
    }

    writeto.close();
}