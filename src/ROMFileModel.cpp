/** \file ROMFileModel.cpp
 *
 *  \brief Implements our subclass for the ROM stuff
 *
 */

#include "ROMFileModel.hpp"
#include "utility.hpp"

#include <QIcon>

ROMFileModel::ROMFileModel(ROM::ROM * ds) : data_src(ds) { }

int ROMFileModel::rowCount(const QModelIndex & /*parent*/) const {
    return data_src->numFiles();
}

int ROMFileModel::columnCount(const QModelIndex & /*parent*/) const {
    // currently only support the four columns, eventually there'll also be a
    // type column, and when able a separate filename column

    return 3;
}

QVariant ROMFileModel::data(const QModelIndex & index, int role) const {
    if (!index.isValid() || index.row() >= rowCount() || index.column() >= columnCount()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
          case 0:
            return data_src->recordAtNum(index.row()).fname.c_str();
            break;

          case 1:
            return QString("0x%1").arg(QString("%1").arg(data_src->recordAtNum(index.row()).vstart, 0, 16).toUpper());
            break;

          case 2:
            return QString(sizeToIEC(data_src->recordAtNum(index.row()).vsize()).c_str());
            break;
        }

        return "ERROR!";
    } else if (role == Qt::DecorationRole) {
        if (index.column() == 0) {
            if (data_src->recordAtNum(index.row()).isCompressed()) {
                return QIcon::fromTheme("package-x-generic", QIcon(":/icons/package-x-generic.svg"));
            } else if (data_src->recordAtNum(index.row()).isMissing()) {
                return QIcon(":/icons/application-missing.svg");
            }
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
            return "Filename";
            break;

          case 1:
            return "Virtual Location";
            break;

          case 2:
            return "File Size";
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