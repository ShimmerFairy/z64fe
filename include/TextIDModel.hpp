/** \file TextIDModel.hpp
 *
 *  \brief Model for handling text ids
 *
 */

#pragma once

#include "ROM.hpp"
#include "Config.hpp"

#include <QAbstractItemModel>

#include <map>
#include <vector>
#include <cstdint>
#include <string>

class TextIDModel : public QAbstractItemModel {
    Q_OBJECT

  private:
    std::map<Config::Language, std::map<uint16_t, size_t>> id_maps;

  public:
    const unsigned int rawRole = Qt::UserRole;

    TextIDModel(ROM::File idset, Config::Version theV);

    Qt::ItemFlags flags(const QModelIndex & index) const override;

    QVariant data(const QModelIndex & index, int role) const override;
    QVariant headerData(int sect, Qt::Orientation orient, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int col, const QModelIndex & parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex & index) const override;

    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;
};