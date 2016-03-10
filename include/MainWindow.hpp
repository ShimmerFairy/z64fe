/** \file MainWindow.hpp
 *
 *  \brief Declares the, er, main window of the application.
 *
 */

#pragma once

#include "ROM.hpp"
#include "ROMFileModel.hpp"

#include <QMainWindow>
#include <QTableView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QMenuBar>
#include <QAction>

#include <cstdint>
#include <vector>
#include <string>

class MainWindow : public QMainWindow {
    Q_OBJECT

  private:
    QWidget * dummy;

    QVBoxLayout * qvb;

    QTableView * filesView;

    QHBoxLayout * qhb;

    QGroupBox * rominfo;
    QGroupBox * fileinfo;


    QMenu * fileMenu;
    QAction * actOpen;
    QAction * actQuit;

    ROM the_rom;
    ROMFileModel * the_rom_model;

    void processROM(std::string fileName);

  private slots:
    void openROM();

  public:
    MainWindow();
};