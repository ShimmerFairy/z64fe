/** \file ROMFileModel.cpp
 *
 *  \brief Implements our subclass for the ROM stuff
 *
 */

#include "ROMFileModel.hpp"
#include "utility.hpp"

#include <QBrush>

ROMFileModel::ROMFileModel(ROM::ROM * ds) : data_src(ds) { }

int ROMFileModel::rowCount(const QModelIndex & /*parent*/) const {
    return data_src->numfiles();
}

int ROMFileModel::columnCount(const QModelIndex & /*parent*/) const {
    // currently only support the four columns, eventually there'll also be a
    // type column, and when able a separate filename column

    return 4;
}

QVariant ROMFileModel::data(const QModelIndex & index, int role) const {
    if (!index.isValid() || index.row() >= rowCount() || index.column() >= columnCount()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
          case 0:
            return QString("0x%1").arg(QString("%1").arg(data_src->recordAt(index.row()).pstart, 0, 16).toUpper());
            break;

          case 1:
            return QString(sizeToIEC(data_src->recordAt(index.row()).psize()).c_str());
            break;

          case 2:
            return QString("0x%1").arg(QString("%1").arg(data_src->recordAt(index.row()).vstart, 0, 16).toUpper());
            break;

          case 3:
            return QString(sizeToIEC(data_src->recordAt(index.row()).vsize()).c_str());
            break;
        }

        return "ERROR!";
    } else if (role == Qt::BackgroundRole) {
        if (data_src->recordAt(index.row()).isCompressed()) {
            return QBrush(Qt::cyan);
        } else if (data_src->recordAt(index.row()).isMissing()) {
            return QBrush(Qt::darkGray);
        }
    }

    return QVariant();
}

QVariant ROMFileModel::headerData(int sect, Qt::Orientation orient, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orient == Qt::Horizontal) {
        switch (sect) {
          case 0:
            return "ROM Location";
            break;

          case 1:
            return "ROM Size";
            break;

          case 2:
            return "Virtual Location";
            break;

          case 3:
            return "Virtual Size";
            break;
        }

        return "WHAT COLUMN IS THIS!?!?";
    } else {
        return QString("#%1").arg(sect + 1);
    }
}

void ROMFileModel::startResetting() {
    beginResetModel();
}

void ROMFileModel::endResetting() {
    endResetModel();
}