/** \file MainWindow.hpp
 *
 *  \brief Declares the, er, main window of the application.
 *
 */

#pragma once

#include "ROM.hpp"
#include "ROMFileModel.hpp"
#include "Config.hpp"

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
#include <QToolBar>

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
    QLabel * rversionkey;
    QLabel * rversionval;
    QPushButton * savebs;

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

    QPushButton * txtview;

    QMenu * fileMenu;
    QAction * actOpen;
    QAction * actQuit;

    QToolBar * actBar;

    ROM::ROM the_rom;

    ROMFileModel * the_rom_model;

    ROM::File curfile;

    void processROM(std::string fileName);

    QFrame * makeGridLine(Qt::Orientation orient);

    // function to handle separate parts of the GUI, implemented in separate
    // file
    void guiMakeMenu();
    void guiMakeLister();
    void guiMakeROMInfo();
    void guiMakeFileInfo();
    void guiAssembleWindow();
//  private slots:
    void openROM();
    void saveROM();

    void chooseFile(const QModelIndex & cur, const QModelIndex & old);

    void openRawView();
    void decompAndOpen();

    void textView();

    void rmWindow(QObject * item);

  public:
    MainWindow();
};