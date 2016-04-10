/** \file HexViewer.cpp
 *
 *  \brief Implementing the hex viewer
 *
 */

#include "HexViewer.hpp"

#include <QHeaderView>
#include <QScrollBar>
#include <QSettings>
#include <QFontMetrics>
#include <QFileDialog>
#include <QMessageBox>

HexFileModel::HexFileModel(ROM::File mf) : myfile(mf) { }

int HexFileModel::rowCount(const QModelIndex & /*parent*/) const {
    return ((myfile.size() - 1) / 16) + 1;
}

int HexFileModel::columnCount(const QModelIndex & /*parent*/) const {
    return 16;
}

QVariant HexFileModel::data(const QModelIndex & idx, int role) const {
    if (!idx.isValid()
     || idx.row() >= rowCount()
     || static_cast<unsigned int>(idx.row() * 16 + idx.column()) >= myfile.size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        return QString("%1").arg(myfile.at(idx.row() * 16 + idx.column()), 2, 16, QChar('0')).toUpper();
    } else {
        return QVariant();
    }
}

QVariant HexFileModel::headerData(int sect, Qt::Orientation orient, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orient == Qt::Horizontal) {
        return QString("%1").arg(sect, 1, 16, QChar('0')).toUpper();
    } else {
        return QString("0x%1").arg(QString("%1").arg(sect * 16, 8, 16, QChar('0')).toUpper());
    }
}

HexFileTextModel::HexFileTextModel(ROM::File mf) : HexFileModel(mf) { }

QVariant HexFileTextModel::data(const QModelIndex & idx, int role) const {
    if (!idx.isValid()
     || idx.row() >= rowCount()
     || static_cast<unsigned int>(idx.row() * 16 + idx.column()) >= myfile.size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        QString res;

        uint8_t thebyte = myfile.at(idx.row() * 16 + idx.column());

        HexViewOpts::UnprintKind uk;

        if (QSettings().value("hexview/unprint") == QVariant()) {
            uk = HexViewOpts::UnprintKind::CARET;
        } else {
            uk = static_cast<HexViewOpts::UnprintKind>(QSettings().value("hexview/unprint").toInt());
        }

        if (thebyte <= 0x1F) {
            switch (uk) {
              case HexViewOpts::UnprintKind::CARET:
                res = QString("^%1").arg(QChar(thebyte + 0x40));
                break;

              case HexViewOpts::UnprintKind::ABBR:
                res = abbrtbl.at(thebyte);
                break;

              case HexViewOpts::UnprintKind::DOT:
                res = ".";
                break;
            }
        } else if (0x20 <= thebyte && thebyte <= 0x7E) {
            res = QChar(thebyte);
        } else if (0x7F <= thebyte && thebyte <= 0x9F) {
            switch (uk) {
              case HexViewOpts::UnprintKind::CARET:
                res = QString("^[%1").arg(QChar(thebyte - 0x40));
                break;

              case HexViewOpts::UnprintKind::ABBR:
                res = abbrtbl.at(thebyte);
                break;

              case HexViewOpts::UnprintKind::DOT:
                res = ".";
                break;
            }
        } else if (0xA0 <= thebyte) {
            res = QChar(thebyte);
        }

        return res;
    } else if (role == Qt::ForegroundRole) {
        uint8_t thebyte = myfile.at(idx.row() * 16 + idx.column());

        HexViewOpts::UnprintKind uk;

        if (QSettings().value("hexview/unprint") == QVariant()) {
            uk = HexViewOpts::UnprintKind::CARET;
        } else {
            uk = static_cast<HexViewOpts::UnprintKind>(QSettings().value("hexview/unprint").toInt());
        }

        if (uk == HexViewOpts::UnprintKind::DOT
            && (thebyte < 0x20
                || (0x7F <= thebyte && thebyte <= 0x9F))) {
            return QColor(Qt::magenta);
        } else {
            return QVariant();
        }
    } else {
        return QVariant();
    }
}

HexViewer::HexViewer(ROM::File mf) : fcopy(mf), hfm(mf), hftm(mf) {
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QString("File %1-%2")
                   .arg(QString("%1").arg(mf.record().vstart, 8, 16, QChar('0')).toUpper())
                   .arg(QString("%1").arg(mf.record().vend, 8, 16, QChar('0')).toUpper()));

    hexside.setFont(QFont("monospace"));
    txtside.setFont(QFont("monospace"));

    hexside.setModel(&hfm);
    txtside.setModel(&hftm);

    hexside.horizontalHeader()->setResizeContentsPrecision(1);
    hexside.horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    hexside.verticalHeader()->setResizeContentsPrecision(1);
    hexside.verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    txtside.horizontalHeader()->setResizeContentsPrecision(1);
    txtside.horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    txtside.verticalHeader()->setResizeContentsPrecision(1);
    txtside.verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // we'll set the sizes ourselves, thanks to the relative consistency of
    // monospace characters
    QFontMetrics qfm(QFont("monospace"));

    for (size_t i = 0; i < 16; i++) {
        hexside.setColumnWidth(i, qfm.width('M') * 3); // 3em columns
        txtside.setColumnWidth(i, qfm.width('M') * 4); // 4em columns
    }

    for (size_t i = 0; i < static_cast<unsigned int>(hfm.rowCount()); i++) {
        hexside.setRowHeight(i, qfm.height());
    }

    for (size_t i = 0; i < static_cast<unsigned int>(hftm.rowCount()); i++) {
        txtside.setRowHeight(i, qfm.height());
    }

    // synchronize vertical scrollbars
    connect(hexside.verticalScrollBar(), &QAbstractSlider::valueChanged,
            txtside.verticalScrollBar(), &QAbstractSlider::setValue);

    connect(txtside.verticalScrollBar(), &QAbstractSlider::valueChanged,
            hexside.verticalScrollBar(), &QAbstractSlider::setValue);

    txtside.verticalHeader()->hide();

    hexside.setAlternatingRowColors(true);
    txtside.setAlternatingRowColors(true);

    dumwidg = new QWidget;
    hbl = new QHBoxLayout(dumwidg);

    hbl->addWidget(&hexside);
    hbl->addWidget(&txtside);

    dumwidg->setLayout(hbl);

    setCentralWidget(dumwidg);

    // set up the menu

    fileMenu = menuBar()->addMenu(tr("&File"));

    saveItem = fileMenu->addAction(tr("&Save this file..."));
    fileMenu->addAction(saveItem);

    toolsMenu = menuBar()->addMenu(tr("&Tools"));

    optionsItem = toolsMenu->addAction(tr("&Options..."));
    toolsMenu->addAction(optionsItem);

    connect(saveItem, &QAction::triggered, this, &HexViewer::saveFile);
    connect(optionsItem, &QAction::triggered, this, &HexViewer::doOptions);
}

void HexViewer::saveFile() {
    QSettings qs;
    QString defName = qs.value("hexview/lastfile", QString()).toString();

    QString saveTo = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                  defName,
                                                  tr("Zelda 64 files, unspecified (*.zdata);;All files (*)"));

    std::string savname = saveTo.toStdString();

    std::ofstream ofile(savname, std::ios_base::binary);

    if (!ofile) {
        QMessageBox::critical(this, "Z64Fe",
                              tr("Can't open %1 for saving. Not saving.").arg(savname.c_str()));
        return;
    }

    std::vector<uint8_t> filedata = fcopy.getData();

    ofile.write(reinterpret_cast<char*>(filedata.data()), filedata.size());

    if (!ofile) {
        QMessageBox::warning(this, "Z64Fe",
                             tr("Error occurred while writing file. Written file may not be complete."));
    }
}

void HexViewer::doOptions() {
    HexViewOpts(this).exec();
}


HexViewOpts::HexViewOpts(QWidget * parent) : QDialog(parent) {
    unprint_opts = new QGroupBox(tr("Unprintable Chars"));

    caret_opt = new QRadioButton(tr("&Caret notation (^@ etc.)"), unprint_opts);
    abbr_opt = new QRadioButton(tr("&Symbol abbreviations (NUL etc.)"), unprint_opts);
    colordot_opt = new QRadioButton(tr("&Colored period characters"), unprint_opts);

    caret_opt->setWhatsThis(tr("Use caret notation for unprintable characters, a popular choice in things like terminals. The carets can however be noisy in a lot of non-textual data."));

    abbr_opt->setWhatsThis(tr("Uses two- or three-letter abbreviations to represent unprintable characters, such as <tt>NUL</tt> for null, <tt>FS</tt> for file separator, and so on. Can be more descriptive at-a-glance, but can also crowd the text side of the viewer a bit."));

    colordot_opt->setWhatsThis(tr("Uses a period (<tt>.</tt>) for all unprintable characters. To distinguish them from actual periods (<tt>0x2E</tt>), these special periods are in a different font color. This may be the least noisy option, but it requires looking at the hex side to see the actual data."));

    unprint_opts->setWhatsThis(tr("Choose how to handle unprintable characters in the text side of the file viewer. \"Unprintable\" characters are those which have no visible glyph associated with them when printed directly, such as newline characters and various control codes. Spaces are considered graphical, not unprintable, characters, at least in this case."));

    unprint_box = new QVBoxLayout;

    unprint_box->addWidget(caret_opt);
    unprint_box->addWidget(abbr_opt);
    unprint_box->addWidget(colordot_opt);

    unprint_opts->setLayout(unprint_box);

    grps = new QVBoxLayout;

    grps->addWidget(unprint_opts);

    setLayout(grps);

    setWindowTitle(tr("Hex Viewer Options"));

    QSettings qs;

    if (qs.value("hexview/unprint") == QVariant()) {
        caret_opt->setChecked(true);
    } else {
        UnprintKind uk = static_cast<UnprintKind>(qs.value("hexview/unprint").toInt());

        switch (uk) {
          case UnprintKind::CARET:
            caret_opt->setChecked(true);
            break;
          case UnprintKind::ABBR:
            abbr_opt->setChecked(true);
            break;
          case UnprintKind::DOT:
            colordot_opt->setChecked(true);
            break;
        }
    }

    // make the connections on radio buttons _now_, so we don't possibly fire
    // them when initially setting them above.

    connect(caret_opt,    &QRadioButton::toggled, this, &HexViewOpts::chooseUnprintCaret);
    connect(abbr_opt,     &QRadioButton::toggled, this, &HexViewOpts::chooseUnprintAbbr);
    connect(colordot_opt, &QRadioButton::toggled, this, &HexViewOpts::chooseUnprintDot);
}

void HexViewOpts::chooseUnprintCaret(bool on) {
    if (on) {
        QSettings qs;
        qs.setValue("hexview/unprint", static_cast<int>(UnprintKind::CARET));
    }
}

void HexViewOpts::chooseUnprintAbbr(bool on) {
    if (on) {
        QSettings qs;
        qs.setValue("hexview/unprint", static_cast<int>(UnprintKind::ABBR));
    }
}

void HexViewOpts::chooseUnprintDot(bool on) {
    if (on) {
        QSettings qs;
        qs.setValue("hexview/unprint", static_cast<int>(UnprintKind::DOT));
    }
}