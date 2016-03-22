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

#include <vector>

class TextViewer : public QMainWindow {
    Q_OBJECT

  private:
    ROM::ROM * trom;

    QTreeView * idlist;
    TextIDModel * idmod;

    QTextEdit * msgview;
    QHBoxLayout * qhb;

    QWidget * dummy;

    std::string transASCII(std::vector<uint8_t>::iterator & takethis, bool & cont);

  private slots:
    void chooseText(const QModelIndex & sel, const QModelIndex & desel);

  public:
    TextViewer(ROM::ROM & r);
};