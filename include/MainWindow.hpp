/** \file MainWindow.hpp
 *
 *  \brief Declares the, er, main window of the application.
 *
 */

#pragma once

#include "ROM.hpp"
#include "ROMFileModel.hpp"
#include "HexViewer.hpp"

#include <QMainWindow>
#include <QTableView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QMenuBar>
#include <QAction>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QFrame>

#include <cstdint>
#include <vector>
#include <string>

class MainWindow : public QMainWindow {
    Q_OBJECT

  private:
    std::vector<QWidget *> childWindows;

    QWidget * dummy;

    QVBoxLayout * qvb;

    QTableView * filesView;

    QHBoxLayout * qhb;

    QGroupBox * rominfo;
    QGroupBox * fileinfo;

    QGridLayout * rigrid;
    QLabel * rnamekey;
    QLabel * rnameval;
    QLabel * rcodekey;
    QLabel * rcodeval;
    QLabel * rsizekey;
    QLabel * rsizeval;

    QGridLayout * figrid;
    QLabel * fplockey;
    QLabel * fplocval;
    QLabel * fpsizekey;
    QLabel * fpsizeval;
    QLabel * fvlockey;
    QLabel * fvlocval;
    QLabel * fvsizekey;
    QLabel * fvsizeval;
    QLabel * fcmprkey;
    QLabel * fcmprval;
    QLabel * femptykey;
    QLabel * femptyval;
    QPushButton * hexviewbtn;
    QPushButton * decompviewbtn;


    QMenu * fileMenu;
    QAction * actOpen;
    QAction * actQuit;

    ROM the_rom;
    ROMFileModel * the_rom_model;

    ROMFile curfile;

    void processROM(std::string fileName);

    QFrame * makeGridLine(Qt::Orientation orient);
  private slots:
    void openROM();

    void chooseFile(const QModelIndex & cur, const QModelIndex & old);

    void openRawView();
    void decompAndOpen();

    void rmWindow(QObject * item);

  public:
    MainWindow();
};