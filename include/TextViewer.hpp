/** \file TextViewer.hpp
 *
 *  \brief Declare the text viewer window
 *
 */

#pragma once

#include "ROM.hpp"

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

class TextViewer : public QMainWindow {
  private:
    QTextEdit * viewer;
    QPushButton * asciibtn;
    QPushButton * sjisbtn;

    QHBoxLayout * qhb;
    QVBoxLayout * qvb;

    QWidget * dummy;

    ROMFile tdata;

  private slots:
    void transASCII();
    void transShiftJIS();

  public:
    TextViewer(ROMFile td);
};