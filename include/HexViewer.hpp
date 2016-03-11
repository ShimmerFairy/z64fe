/** \file HexViewer.hpp
 *
 *  \brief A basic file viewer, displaying in hex
 *
 */

#pragma once

#include "ROM.hpp"

#include <QAbstractTableModel>
#include <QWidget>
#include <QTableView>
#include <QHBoxLayout>

class HexFileModel : public QAbstractTableModel {
  protected:
    ROMFile myfile;

  public:
    HexFileModel(ROMFile mf);

    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;

    QVariant data(const QModelIndex & idx, int role = Qt::DisplayRole) const override;
    QVariant headerData(int sect, Qt::Orientation orient, int role = Qt::DisplayRole) const override;
};

class HexFileTextModel : public HexFileModel {
  public:
    HexFileTextModel(ROMFile mf);

    QVariant data(const QModelIndex & idx, int role = Qt::DisplayRole) const override;
};

class HexViewer : public QWidget {
  private:
    HexFileModel hfm;
    HexFileTextModel hftm;
    QTableView hexside;
    QTableView txtside;

    QHBoxLayout * hbl;

  public:
    HexViewer(ROMFile mf);
};