/** \file ROMFileModel.hpp
 *
 *  \brief Declares a class for interfacing with the rom data in a way
 *         compatible with Qt's model/view stuff.
 *
 */

#pragma once

#include "ROM.hpp"

#include <QAbstractTableModel>

class ROMFileModel : public QAbstractTableModel {
    Q_OBJECT

  private:
    ROM * data_src;

  public:
    ROMFileModel(ROM * ds);

    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int sect, Qt::Orientation orient, int role = Qt::DisplayRole) const override;

    void startResetting();
    void endResetting();
};