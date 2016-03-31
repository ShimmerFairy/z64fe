/** \file TextIDModel.cpp
 */

#include "TextIDModel.hpp"

#include <iostream>

TextIDModel::TextIDModel(std::map<Config::Language, std::map<uint16_t, uint32_t>> im) : id_maps(im) { }

Qt::ItemFlags TextIDModel::flags(const QModelIndex & index) const {
    if (!index.isValid()) {
        return 0;
    }

    return QAbstractItemModel::flags(index);
}

QVariant TextIDModel::data(const QModelIndex & index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        if (index.internalId() == 0) {
            if (index.column() == 0) {
                return Config::langString(std::next(id_maps.begin(), index.row())->first).c_str();
            } else {
                return QVariant();
            }
        } else {
            if (index.column() == 0) {
                return QString("ID 0x%1").arg(
                    QString("%1").arg(
                        std::next(
                            std::next(
                                id_maps.begin(),
                                index.internalId() - 1
                            )->second.begin(), index.row()
                        )->first,
                        4,
                        16,
                        QChar('0')).toUpper());
            } else if (index.column() == 1) {
                return QString("0x%1").arg(
                    QString("%1").arg(
                        std::next(
                            std::next(
                                id_maps.begin(),
                                index.internalId() - 1
                            )->second.begin(), index.row()
                        )->second,
                        6,
                        16,
                        QChar('0')).toUpper());
            } else {
                return QVariant();
            }
        }
    } else if (role == rawRole) {
        if (index.internalId() == 0) {
            return static_cast<uint>(std::next(id_maps.begin(), index.row())->first);
        } else {
            return std::next(
                std::next(id_maps.begin(), index.internalId() - 1)->second.begin(),
                index.row())->second;
        }
    }

    return QVariant();
}

QVariant TextIDModel::headerData(int sect, Qt::Orientation orient, int role) const {
    if (role != Qt::DisplayRole || orient != Qt::Horizontal) {
        return QVariant();
    }

    if (sect == 0) {
        return "ID";
    } else if (sect == 1) {
        return "Address";
    } else {
        return QVariant();
    }
}

QModelIndex TextIDModel::index(int row, int col, const QModelIndex & parent) const {
    if (!parent.isValid()) {
        return createIndex(row, col, quintptr(0)); // this, friends, is why
                                                   // nullptr is better than
                                                   // NULL
    }

    return createIndex(row, col, parent.row() + 1);
}

QModelIndex TextIDModel::parent(const QModelIndex & index) const {
    if (!index.isValid() || index.internalId() == 0) {
        return QModelIndex();
    }

    return createIndex(index.internalId() - 1, 0, quintptr(0));
}

int TextIDModel::rowCount(const QModelIndex & parent) const {
    if (parent.column() > 0) {
        return 0;
    } else if (!parent.isValid()) {
        return id_maps.size();
    } else if (parent.internalId() == 0) {
        return std::next(id_maps.begin(), parent.row())->second.size();
    } else {
        return 0;
    }
}

int TextIDModel::columnCount(const QModelIndex & /*parent*/) const {
    return 2;
}