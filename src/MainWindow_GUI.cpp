/** \file MainWindow_GUI.cpp
 *
 *  \brief Specifically implement all the necessary functions to do the dirty
 *         GUI work.
 *
 */

#include "MainWindow.hpp"

void MainWindow::guiMakeMenu() {
    fileMenu = menuBar()->addMenu(tr("&File"));

    actOpen = new QAction(QIcon::fromTheme("document-open", QIcon(":/icons/document-open.svg")), tr("&Open ROM..."), this);
    actOpen->setShortcuts(QKeySequence::Open);
    actOpen->setStatusTip(tr("Open an Ocarina of Time or Majora's Mask ROM."));

    actQuit = new QAction(QIcon::fromTheme("application-exit", QIcon(":/icons/application-exit.svg")), tr("&Quit"), this);
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

void MainWindow::guiMakeROMTab() {
    rom_tab = new QWidget;
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

    rom_tab->setLayout(rigrid);

    //
    // CONNECTIONS
    //

    connect(savebs, &QPushButton::clicked, this, &MainWindow::saveROM);
}

void MainWindow::guiMakeFileTab() {
    basic_file_tab = new QWidget;

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

    basic_file_tab->setLayout(figrid);

    //
    // CONNECTIONS
    //

    connect(hexviewbtn, &QPushButton::clicked, this, &MainWindow::openRawView);
    connect(decompviewbtn, &QPushButton::clicked, this, &MainWindow::decompAndOpen);
}

void MainWindow::guiMakeTextTab() {
    tNumberKey = new QLabel(tr("Number of messages:"));
    tNumberValue = new QLabel(tr("n/a"));
    tLangsKey = new QLabel(tr("Languages:"));
    tLangsValue = new QLabel(tr("n/a"));

    tReadTbl = new QPushButton(tr("No ROM Loaded"));
    tReadTbl->setEnabled(false);
    tSeeText = new QPushButton(tr("No ROM Loaded"));
    tSeeText->setEnabled(false);

    tigrid = new QGridLayout;

    tigrid->addWidget(tNumberKey, 0, 0, Qt::AlignRight);
    tigrid->addWidget(tNumberValue, 0, 1, Qt::AlignLeft);
    tigrid->addWidget(tLangsKey, 1, 0, Qt::AlignRight);
    tigrid->addWidget(tLangsValue, 1, 1, Qt::AlignLeft);

    tigrid->addWidget(makeGridLine(Qt::Vertical), 0, 2, 2, 1);

    tigrid->addWidget(tReadTbl, 0, 3, 1, 3);
    tigrid->addWidget(tSeeText, 1, 3, 1, 3);

    text_tab = new QWidget;

    text_tab->setLayout(tigrid);

    //
    // CONNECTIONS
    //

    connect(tReadTbl, &QPushButton::clicked, this, &MainWindow::analyzeTextTbl);
    connect(tSeeText, &QPushButton::clicked, this, &MainWindow::openTextViewer);
}

void MainWindow::guiIntroScreen() {
    introlbl = new QLabel(tr("Welcome to Z64Fe!"));
    introsublbl = new QLabel(tr("Please open a ROM file to start."));

    QFont fnta = introlbl->font();
    fnta.setPointSize(18);
    introlbl->setFont(fnta);
    introlbl->setAlignment(Qt::AlignCenter);

    QFont fntb = introsublbl->font();
    fntb.setItalic(true);
    introsublbl->setFont(fntb);
    introsublbl->setAlignment(Qt::AlignCenter);

    introload = new QPushButton(QIcon::fromTheme("document-open", QIcon(":/icons/document-open.svg")), tr("Open ROM..."));
    connect(introload, &QPushButton::clicked, this, &MainWindow::openROM);

    introqvb = new QVBoxLayout;

    introqvb->addStretch(2);
    introqvb->addWidget(introlbl);
    introqvb->addWidget(introsublbl);
    introqvb->addStretch(4);
    introqvb->addWidget(introload);
    introqvb->addStretch(1);

    introdummy = new QWidget;
    introdummy->setLayout(introqvb);
}

void MainWindow::guiAssembleWindow() {
    // call all the "subordinate" functions
    guiMakeMenu();
    guiMakeLister();
    guiMakeROMTab();
    guiMakeFileTab();
    guiMakeTextTab();

    // now to assemble all the disparate pieces

    control_panel = new QTabWidget;

    control_panel->addTab(rom_tab, tr("ROM Info"));
    control_panel->addTab(basic_file_tab, tr("File Info"));
    control_panel->addTab(text_tab, tr("Message Text"));

    qvb = new QVBoxLayout;

    qvb->addWidget(filesView);
    qvb->addWidget(control_panel);

    dummy = new QWidget;

    dummy->setLayout(qvb);

    guiIntroScreen();

    setCentralWidget(introdummy);

    resize(400, 500);
}

void MainWindow::guiNewROM_TextTab() {
    tNumberValue->setText("--");
    tLangsValue->setText("--");

    if (Config::getGame(the_rom.getVersion()) == Config::Game::Majora) {
        tReadTbl->setEnabled(false);
        tSeeText->setEnabled(false);

        tReadTbl->setText(tr("Majora's mask text NYI"));
        tSeeText->setText(tr("Majora's mask text NYI"));
    } else if (!the_rom.hasConfigKey({"codeData", "TextMsgTable"})) {
        tReadTbl->setEnabled(false);
        tSeeText->setEnabled(false);

        tReadTbl->setText(tr("Can't read text from this ROM"));
        tSeeText->setText(tr("Missing needed config data"));
    } else {
        tReadTbl->setEnabled(true);
        tSeeText->setEnabled(true);

        tReadTbl->setText(tr("&Analyze Message Table"));
        tSeeText->setText(tr("&View Text Messages"));
    }
}