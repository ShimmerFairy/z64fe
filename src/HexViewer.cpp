/** \file HexViewer.cpp
 *
 *  \brief Implementing the hex viewer
 *
 */

#include "HexViewer.hpp"

HexFileModel::HexFileModel(ROMFile mf, QObject * parent) : myfile(mf), QAbstractTableModel(parent) { }

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