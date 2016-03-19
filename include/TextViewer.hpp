/** \file TextViewer.hpp
 *
 *  \brief Declare the text viewer window
 *
 */

#pragma once

#include "ROM.hpp"
#include "Config.hpp"
#include "TextIDModel.hpp"

#include <QMainWindow>
#include <QTreeView>
#include <QTextEdit>
#include <QHBoxLayout>

#include <map>

class TextViewer : public QMainWindow {
    Q_OBJECT

  private:
    ROM::ROM trom;

    QTreeView * idlist;
    TextIDModel * idmod;

    QTextEdit * msgview;
    QHBoxLayout * qhb;

    QWidget * dummy;

  private slots:
    void chooseText(const QItemSelection & sel, const QItemSelection & desel);

  public:
    TextViewer(ROM::ROM r);
};