/** \file ROMFileWidget.cpp
 *
 *  \brief Implementation of the ROMFileWidget class
 *
 */

#include "ROMFileWidget.hpp"

#include "utility.hpp"

#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>

ROMFileWidget::ROMFileWidget(QWidget * parent) : QWidget(parent) {
    the_model = nullptr;
    the_rom = nullptr;

    wlay = new QGridLayout;

    filelist = new QTableView;

    vloc_key  = new QLabel(tr("Virtual Address:"));
    vloc_val  = new QLabel;
    vsize_key = new QLabel(tr("Virtual Size:"));
    vsize_val = new QLabel;

    ploc_key  = new QLabel(tr("Physical Address:"));
    ploc_val  = new QLabel;
    psize_key = new QLabel(tr("Physical Size:"));
    psize_val = new QLabel;

    comp_key = new QLabel(tr("Compressed?:"));
    comp_val = new QLabel;

    empty_key = new QLabel(tr("Empty/Compressed?:"));
    empty_val = new QLabel;

    want_dec = new QCheckBox(tr("&Use decompressed"), this);
    want_dec->setToolTip(tr("If selected, uses the file decompressed if necessary. Otherwise, always use file as-is."));

    view_hex  = new QPushButton(tr("&View Raw File"));
    save_file = new QPushButton(tr("&Save Individual File..."));

    QFont monfont("monospace");

    vloc_val->setFont(monfont);
    vsize_val->setFont(monfont);
    ploc_val->setFont(monfont);
    psize_val->setFont(monfont);

    want_dec->setEnabled(false);
    view_hex->setEnabled(false);
    save_file->setEnabled(false);

    wlay->addWidget(filelist, 0, 0, 1, 2);

    wlay->addWidget(vloc_key, 1, 0, 1, 1, Qt::AlignRight);
    wlay->addWidget(vloc_val, 1, 1, 1, 1, Qt::AlignLeft);
    wlay->addWidget(vsize_key, 2, 0, 1, 1, Qt::AlignRight);
    wlay->addWidget(vsize_val, 2, 1, 1, 1, Qt::AlignLeft);

    wlay->addWidget(ploc_key, 3, 0, 1, 1, Qt::AlignRight);
    wlay->addWidget(ploc_val, 3, 1, 1, 1, Qt::AlignLeft);
    wlay->addWidget(psize_key, 4, 0, 1, 1, Qt::AlignRight);
    wlay->addWidget(psize_val, 4, 1, 1, 1, Qt::AlignLeft);

    wlay->addWidget(comp_key, 5, 0, 1, 1, Qt::AlignRight);
    wlay->addWidget(comp_val, 5, 1, 1, 1, Qt::AlignLeft);

    wlay->addWidget(empty_key, 6, 0, 1, 1, Qt::AlignRight);
    wlay->addWidget(empty_val, 6, 1, 1, 1, Qt::AlignLeft);


    wlay->addWidget(want_dec, 7, 0, 1, 2, Qt::AlignCenter);
    wlay->addWidget(view_hex, 8, 0, 1, 2);
    wlay->addWidget(save_file, 9, 0, 1, 2);

    setLayout(wlay);

    /***************
     * CONNECTIONS *
     ***************/

    connect(save_file, &QPushButton::clicked, this, &ROMFileWidget::saveFile);
}

void ROMFileWidget::changeROM(ROM::ROM * nr) {
    delete the_model;
    the_rom = nr;

    the_model = new ROMFileModel(the_rom);

    QItemSelectionModel * delme = filelist->selectionModel();
    filelist->setModel(the_model);
    delete delme;

    filelist->setSelectionMode(QAbstractItemView::SingleSelection);
    filelist->setSelectionBehavior(QAbstractItemView::SelectRows);

    filelist->resizeColumnsToContents();
    filelist->resizeRowsToContents();

    connect(filelist->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &ROMFileWidget::selectFile);

    want_dec->setEnabled(true);
    view_hex->setEnabled(true);
    save_file->setEnabled(true);
}

void ROMFileWidget::selectFile(const QModelIndex & cur, const QModelIndex & /*old*/) {
    ROM::Record currec = the_rom->recordAtNum(cur.row());

    ploc_val->setText(QString("0x%1").arg(
                          QString("%1").arg(currec.pstart, 8, 16, QChar('0')).toUpper()));

    psize_val->setText(sizeToIEC(currec.psize()).c_str());

    vloc_val->setText(QString("0x%1").arg(
                          QString("%1").arg(currec.vstart, 8, 16, QChar('0')).toUpper()));

    vsize_val->setText(sizeToIEC(currec.vsize()).c_str());

    comp_val->setText(currec.isCompressed() ? tr("yes") : tr("no"));

    empty_val->setText(currec.isMissing() ? tr("yes") : tr("no"));
}

void ROMFileWidget::saveFile() {
    QSettings qs;

    QString saveto = QFileDialog::getSaveFileName(this, tr("Save ROM"),
                                                  qs.value("main/last_save_zdata").toString(),
                                                  tr("Generic Z64 Data (*.zdata);;Any Files (*)"));

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

    qs.setValue("main/last_save_zdata", saveto);

    std::vector<uint8_t> thedata = the_rom->fileAtNum(filelist->currentIndex().row(), want_dec->isChecked()).getData();

    writeto.write(reinterpret_cast<char *>(thedata.data()), thedata.size());

    if (!writeto) {
        QMessageBox::warning(parentWidget(), tr("Possible Error in Saving"),
                             tr("Something went wrong in writing the file, the saved file may be incomplete."));
    }

    writeto.close();
}