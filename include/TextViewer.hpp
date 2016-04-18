/** \file TextViewer.hpp
 *
 *  \brief Declare the text viewer window
 *
 */

#pragma once

#include "ROM.hpp"
#include "Config.hpp"
#include "TextIDModel.hpp"
#include "TextAST.hpp"
#include "TextRender.hpp"

#include <QMainWindow>
#include <QTreeView>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QFrame>

#include <vector>

class TextViewer : public QMainWindow {
    Q_OBJECT

  private:
    ROM::ROM * trom;

    std::vector<TextAST::Box> readtxt;

    TextAST::MessageIndex midx;

    TextAST::MsgInfo minfo;

    QTreeView * idlist;
    TextIDModel * idmod;

    QTextEdit * msgview;
    QHBoxLayout * qhb;

    TextRender * msgrend;

    QVBoxLayout * qvb;

    QWidget * dummy;

    std::string fragAsCode(TextAST::Fragment frag);
    void writeCodeText();

  private slots:
    void chooseText(const QModelIndex & sel, const QModelIndex & desel);

  public:
    TextViewer(ROM::ROM * r);
};