/** \file HexViewer.cpp
 *
 *  \brief Implementing the hex viewer
 *
 */

#include "HexViewer.hpp"

#include <QHeaderView>

HexFileModel::HexFileModel(ROMFile mf) : myfile(mf) { }

int HexFileModel::rowCount(const QModelIndex & /*parent*/) const {
    return ((myfile.size() - 1) / 16) + 1;
}

int HexFileModel::columnCount(const QModelIndex & /*parent*/) const {
    return 16;
}

QVariant HexFileModel::data(const QModelIndex & idx, int role) const {
    if (!idx.isValid()
     || idx.row() >= rowCount()
     || (idx.row() * 16 + idx.column()) >= myfile.size()) {
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

HexFileTextModel::HexFileTextModel(ROMFile mf) : HexFileModel(mf) { }

QVariant HexFileTextModel::data(const QModelIndex & idx, int role) const {
    if (!idx.isValid()
     || idx.row() >= rowCount()
     || (idx.row() * 16 + idx.column()) >= myfile.size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        uint8_t thebyte = myfile.at(idx.row() * 16 + idx.column());

        if (0 <= thebyte && thebyte <= 0x1F) {
            return ".";
        } else if (0x20 <= thebyte && thebyte <= 0x7E) {
            return QChar(thebyte);
        } else if (0x7F <= thebyte && thebyte <= 0x9F) {
            return ".";
        } else if (0xA0 <= thebyte && thebyte <= 0xFF) {
            return QChar(thebyte);
        }
    } else {
        return QVariant();
    }
}

HexViewer::HexViewer(ROMFile mf) : hfm(mf), hftm(mf) {
    setAttribute(Qt::WA_DeleteOnClose);

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

    txtside.verticalHeader()->hide();

    hexside.setAlternatingRowColors(true);
    txtside.setAlternatingRowColors(true);

    hexside.resizeColumnsToContents();
    hexside.resizeRowsToContents();
    txtside.resizeColumnsToContents();
    txtside.resizeRowsToContents();

    hbl = new QHBoxLayout(this);

    hbl->addWidget(&hexside);
    hbl->addWidget(&txtside);

    setLayout(hbl);
}