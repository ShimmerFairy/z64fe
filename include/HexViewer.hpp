/** \file HexViewer.hpp
 *
 *  \brief A basic file viewer, displaying in hex
 *
 */

#pragma once

#include "ROM.hpp"

#include <QAbstractTableModel>

class HexFileModel : public QAbstractTableModel {
  private:
    ROMFile myfile;

  public:
    HexFileModel(ROMFile mf, QObject * parent = nullptr);

    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;

    QVariant data(const QModelIndex & idx, int role = Qt::DisplayRole) const override;
    QVariant headerData(int sect, Qt::Orientation orient, int role = Qt::DisplayRole) const override;
};