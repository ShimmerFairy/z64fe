/** \file TextViewer.cpp
 *
 *  \brief Implement text viewer
 *
 */

#include "TextViewer.hpp"
#include "TextConv.hpp"
#include "TextAST.hpp"
#include "Exceptions.hpp"

#include <QString>
#include <QMessageBox>

#include <sstream>
#include <iomanip>
#include <iostream>

TextViewer::TextViewer(ROM::ROM & r) : trom(&r) {
    setAttribute(Qt::WA_DeleteOnClose);

    idlist = new QTreeView;
    idmod = new TextIDModel(trom->msgTbl());
    msgview = new QTextEdit;
    qhb = new QHBoxLayout;
    dummy = new QWidget;

    idlist->setModel(idmod);
    idlist->setSelectionMode(QAbstractItemView::SingleSelection);

    msgview->setReadOnly(true);

    qhb->addWidget(idlist);
    qhb->addWidget(msgview);

    dummy->setLayout(qhb);

    setCentralWidget(dummy);

    setWindowTitle(tr("Z64Fe - Text Viewer"));

    connect(idlist->selectionModel(), &QItemSelectionModel::currentChanged, this, &TextViewer::chooseText);
}

void TextViewer::chooseText(const QModelIndex & sel, const QModelIndex & /*desel*/) {
    // we can assume one selection because we asked for single selections in the
    // constructor.

    // we check for a valid parent as a way of making sure we'll only do stuff
    // when an ID, not a language, is selected.
    if (sel.parent().isValid()) {
        readtxt.clear();
        uint32_t address = idmod->data(sel, TextIDModel::rawRole).toUInt();
        Config::Language lang = static_cast<Config::Language>(idmod->data(sel.parent(), TextIDModel::rawRole).toUInt());

        ROM::File msgfile;

        try {
            switch (lang) {
              case Config::Language::JP:
                msgfile = trom->fileAtName("jpn_message_data_static");
                break;

              case Config::Language::EN:
                msgfile = trom->fileAtName("nes_message_data_static");
                break;

              case Config::Language::DE:
                msgfile = trom->fileAtName("ger_message_data_static");
                break;

              case Config::Language::FR:
                msgfile = trom->fileAtName("fra_message_data_static");
                break;

              case Config::Language::ES:
                msgfile = trom->fileAtName("esp_message_data_static");
                break;
            }
        } catch (Exception & e) {
            QMessageBox::critical(this, tr("ERROR!"),
                                  e.what().c_str());
            std::exit(-1);
        }

        std::vector<uint8_t> the_text;

        auto readptr = msgfile.begin() + address;

        std::stringstream res;

        if (Config::getGame(trom->getVersion()) == Config::Game::Ocarina) {
            if (lang == Config::Language::JP) {
                readtxt = readShiftJIS_OoT(readptr);
            } else {
                readtxt = readASCII_OoT(readptr);
            }
        } else {
            readtxt = readASCII_MM(readptr);
        }

        for (auto & i : readtxt) {
            res << i.codeString();
        }

        msgview->setPlainText(res.str().c_str());
    }
}