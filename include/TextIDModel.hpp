/** \file TextIDModel.hpp
 *
 *  \brief Model for handling text ids
 *
 */

#pragma once

#include "ROM.hpp"
#include "Config.hpp"
#include "TextAST.hpp"

#include <QAbstractItemModel>

#include <map>
#include <vector>
#include <cstdint>
#include <string>

class TextIDModel : public QAbstractItemModel {
    Q_OBJECT

  private:
    TextAST::MessageIndex id_maps;

  public:
    static const unsigned int rawRole = Qt::UserRole;

    TextIDModel(TextAST::MessageIndex im);

    Qt::ItemFlags flags(const QModelIndex & index) const override;

    QVariant data(const QModelIndex & index, int role) const override;
    QVariant headerData(int sect, Qt::Orientation orient, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int col, const QModelIndex & parent = QModelIndex()) const override;

    QModelIndex parent(const QModelIndex & index) const override;

    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;
};