/** \file MainWindow_GUI.cpp
 *
 *  \brief Specifically implement all the necessary functions to do the dirty
 *         GUI work.
 *
 */

#include "MainWindow.hpp"

void MainWindow::guiMakeMenu() {
    fileMenu = menuBar()->addMenu(tr("&File"));

    actOpen = new QAction(QIcon::fromTheme("document-open"), tr("&Open ROM..."), this);
    actOpen->setShortcuts(QKeySequence::Open);
    actOpen->setStatusTip(tr("Open an Ocarina of Time or Majora's Mask ROM."));

    actQuit = new QAction(QIcon::fromTheme("application-exit"), tr("&Quit"), this);
    actQuit->setShortcuts(QKeySequence::Quit);
    actQuit->setStatusTip(tr("Exit this program."));

    fileMenu->addAction(actOpen);
    fileMenu->addSeparator();
    fileMenu->addAction(actQuit);

    actBar = addToolBar("Actions");

    actBar->addAction(actOpen);
    actBar->addSeparator();
    actBar->addAction(actQuit);

    //
    // CONNECTIONS
    //

    connect(actOpen, &QAction::triggered, this, &MainWindow::openROM);
    connect(actQuit, &QAction::triggered, this, &MainWindow::close);
}

void MainWindow::guiMakeLister() {
    filesView = new QTableView;
    the_rom_model = nullptr;
}

void MainWindow::guiMakeROMInfo() {
    rominfo = new QGroupBox(tr("ROM Info"));
    rigrid = new QGridLayout;

    rnamekey = new QLabel(tr("ROM Name:"));
    rnameval = new QLabel;
    rcodekey = new QLabel(tr("ROM Code:"));
    rcodeval = new QLabel;
    rsizekey = new QLabel(tr("ROM Size:"));
    rsizeval = new QLabel;
    rversionkey = new QLabel(tr("ROM Version:"));
    rversionval = new QLabel;

    savebs = new QPushButton(tr("No ROM loaded"));
    savebs->setEnabled(false);

    rigrid->addWidget(rnamekey, 0, 0, Qt::AlignRight);
    rigrid->addWidget(rnameval, 0, 1, Qt::AlignLeft);
    rigrid->addWidget(rcodekey, 1, 0, Qt::AlignRight);
    rigrid->addWidget(rcodeval, 1, 1, Qt::AlignLeft);
    rigrid->addWidget(rsizekey, 2, 0, Qt::AlignRight);
    rigrid->addWidget(rsizeval, 2, 1, Qt::AlignLeft);
    rigrid->addWidget(rversionkey, 3, 0, Qt::AlignRight);
    rigrid->addWidget(rversionval, 3, 1, Qt::AlignLeft);

    rigrid->addWidget(savebs, 4, 0, 1, 2);

    rominfo->setLayout(rigrid);

    //
    // CONNECTIONS
    //

    connect(savebs, &QPushButton::clicked, this, &MainWindow::saveROM);
}

void MainWindow::guiMakeFileInfo() {
    fileinfo = new QGroupBox(tr("File Info"));

    figrid = new QGridLayout;

    fplockey = new QLabel(tr("ROM Location:"));
    fplocval = new QLabel;
    fpsizekey = new QLabel(tr("Size in ROM:"));
    fpsizeval = new QLabel;
    fvlockey = new QLabel(tr("Virtual Location:"));
    fvlocval = new QLabel;
    fvsizekey = new QLabel(tr("Virtual Size:"));
    fvsizeval = new QLabel;
    fcmprkey = new QLabel(tr("Compressed?:"));
    fcmprval = new QLabel;
    femptykey = new QLabel(tr("Empty/Missing?:"));
    femptyval = new QLabel;

    hexviewbtn = new QPushButton(tr("&View Raw File"));
    hexviewbtn->setEnabled(false);

    decompviewbtn = new QPushButton(tr("&Decompress and View Raw"));
    decompviewbtn->setEnabled(false);

    figrid->addWidget(fplockey, 0, 0, Qt::AlignRight);
    figrid->addWidget(fplocval, 0, 1, Qt::AlignLeft);
    figrid->addWidget(makeGridLine(Qt::Vertical), 0, 2, 3, 1);
    figrid->addWidget(fpsizekey, 0, 3, Qt::AlignRight);
    figrid->addWidget(fpsizeval, 0, 4, Qt::AlignLeft);

    figrid->addWidget(fvlockey, 2, 0, Qt::AlignRight);
    figrid->addWidget(fvlocval, 2, 1, Qt::AlignLeft);

    figrid->addWidget(fvsizekey, 2, 3, Qt::AlignRight);
    figrid->addWidget(fvsizeval, 2, 4, Qt::AlignLeft);

    figrid->addWidget(makeGridLine(Qt::Vertical), 0, 5, 3, 1);

    figrid->addWidget(fcmprkey, 0, 6, Qt::AlignRight);
    figrid->addWidget(fcmprval, 0, 7, Qt::AlignLeft);
    figrid->addWidget(femptykey, 2, 6, Qt::AlignRight);
    figrid->addWidget(femptyval, 2, 7, Qt::AlignLeft);

    figrid->addWidget(makeGridLine(Qt::Horizontal), 1, 0, 1, 8);

    figrid->addWidget(hexviewbtn, 3, 0, 1, 4);
    figrid->addWidget(decompviewbtn, 3, 4, 1, 4);

    figrid->setSpacing(10);

    fileinfo->setLayout(figrid);

    //
    // CONNECTIONS
    //

    connect(hexviewbtn, &QPushButton::clicked, this, &MainWindow::openRawView);
    connect(decompviewbtn, &QPushButton::clicked, this, &MainWindow::decompAndOpen);
}

void MainWindow::guiAssembleWindow() {
    // call all the "subordinate" functions
    guiMakeMenu();
    guiMakeLister();
    guiMakeROMInfo();
    guiMakeFileInfo();

    // now to assemble all the disparate pieces

    qhb = new QHBoxLayout;

    qhb->addWidget(rominfo);
    qhb->addStretch();
    qhb->addWidget(fileinfo);

    qvb = new QVBoxLayout;

    qvb->addWidget(filesView);
    qvb->addLayout(qhb);

    dummy = new QWidget;

    dummy->setLayout(qvb);

    setCentralWidget(dummy);
}