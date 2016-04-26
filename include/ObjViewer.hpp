/** \file
 *
 *  \brief Sets up the window for viewing an object.
 *
 */

#pragma once

#include "RCP/DisplayList.hpp"
#include "ROM.hpp"

#include <QAbstractListModel>
#include <QWidget>
#include <QHBoxLayout>
#include <QListView>
#include <QTextEdit>

class ObjDLModel : public QAbstractListModel {
  private:
    std::map<size_t, RCP::DisplayList> * dlmod;

  public:
    ObjDLModel(std::map<size_t, RCP::DisplayList> * dm);

    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
};

class ObjViewer : public QWidget {
    Q_OBJECT

  private:
    std::map<size_t, RCP::DisplayList> dl_map;

    QHBoxLayout * qhb;
    QListView * dl_list;
    ObjDLModel * odlm;
    QTextEdit * qte;

  private slots:
    void selectItem(const QModelIndex & cur, const QModelIndex & prev);

  public:
    ObjViewer(ROM::File rf, QWidget * parent = nullptr);
};