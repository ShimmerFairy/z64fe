/** \file MainWindow.cpp
 *
 *  \brief Implements the main window.
 *
 */

#include "MainWindow.hpp"
#include "Exceptions.hpp"
#include "Hex/Widget.hpp"
#include "TextViewer.hpp"
#include "projectinfo.hpp"

#include <QToolBar>
#include <QMenuBar>
#include <QCloseEvent>
#include <QSettings>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QApplication>

MainWindow::MainWindow() {
    the_rom = nullptr;

    main_portal = new QMdiArea(this);

    file_list_widget = new ROMFileWidget(this);
    rom_info_widget = new ROMInfoWidget(this);

    file_list_dock = new QDockWidget(tr("ROM Files"), this);
    rom_info_dock = new QDockWidget(tr("ROM Info"), this);

    file_list_dock->setWidget(file_list_widget);
    rom_info_dock->setWidget(rom_info_widget);

    addDockWidget(Qt::RightDockWidgetArea, rom_info_dock);
    addDockWidget(Qt::RightDockWidgetArea, file_list_dock);

    load_rom = new QAction(QIcon::fromTheme("document-open", QIcon(":/icons/document-open.svg")),
                           tr("&Open ROM..."),
                           this);
    load_rom->setShortcut(QKeySequence::Open);

    exit_prog = new QAction(QIcon::fromTheme("application-exit", QIcon(":/icons/application-exit.svg")),
                            tr("&Exit"),
                            this);
    exit_prog->setShortcut(QKeySequence::Quit);

    about_this = new QAction(QIcon::fromTheme("help-about", QIcon(":/icons/help-about.svg")),
                             tr("&About Z64Fe"),
                             this);

    about_qt = new QAction(tr("About &Qt"), this);

    file_menu = menuBar()->addMenu(tr("&File"));
    file_menu->addAction(load_rom);
    file_menu->addSeparator();
    file_menu->addAction(exit_prog);

    help_menu = menuBar()->addMenu(tr("&Help"));
    help_menu->addAction(about_this);
    help_menu->addAction(about_qt);

    actions_toolbar = addToolBar(tr("Actions"));
    actions_toolbar->addAction(load_rom);
    actions_toolbar->addSeparator();
    actions_toolbar->addAction(exit_prog);

    setCentralWidget(main_portal);

    setWindowTitle(tr("Zelda 64 File Explorer"));

    /***************
     * CONNECTIONS *
     ***************/

    connect(this, &MainWindow::romChanged, file_list_widget, &ROMFileWidget::changeROM);
    connect(this, &MainWindow::romChanged, rom_info_widget, &ROMInfoWidget::changeROM);

    connect(load_rom, &QAction::triggered, this, &MainWindow::openROM);
    connect(exit_prog, &QAction::triggered, this, &MainWindow::close);
    connect(about_this, &QAction::triggered, this, &MainWindow::aboutMe);
    connect(about_qt, &QAction::triggered, &QApplication::aboutQt);

    connect(file_list_widget, &ROMFileWidget::wantHexWindow, this, &MainWindow::makeHexWindow);
    connect(rom_info_widget, &ROMInfoWidget::wantTextWindow, this, &MainWindow::makeTextWindow);
}

void MainWindow::closeEvent(QCloseEvent * ev) {
    ev->accept();
}

void MainWindow::openROM() {
    QSettings qs;
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Open ROM"),
        qs.value("main/lastfile", QString()).toString(),
        tr("N64 ROM Files (*.z64 *.n64);;All files (*)"));

    if (fileName.isEmpty()) {
        return;
    }

    qs.setValue("main/lastfile", fileName);

    QFile rfile(fileName);

    if (!rfile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("File Error"), rfile.errorString() + "\n(Don't worry, you can still work with the last ROM)");
        return;
    }

    std::vector<uint8_t> rdat;
    QByteArray junk = rfile.readAll();

    std::copy(junk.begin(), junk.end(), std::back_inserter(rdat));

    // now to create the ROM itself, and hopefully it's OK.
    ROM::ROM * nrom;

    try {
        nrom = new ROM::ROM(rdat);
    } catch (Exception & e) {
        QMessageBox::critical(this, tr("ROM Handling Error"), QString(e.what().c_str()) + "\n(You can still work on the previous ROM)");
        return;
    }

    std::swap(the_rom, nrom);

    // signal the change in ROM to everyone who needs it

    romChanged(the_rom);

    // we can now safely delete the old rom
    delete nrom;
}

void MainWindow::makeHexWindow(ROM::File rf) {
    QByteArray qba(reinterpret_cast<char *>(rf.getData().data()), rf.size());
    main_portal->addSubWindow(new Hex::Widget(qba))->show();
}

void MainWindow::makeTextWindow() {
    main_portal->addSubWindow(new TextViewer(the_rom))->show();
}

void MainWindow::aboutMe() {
    QMessageBox::about(this, "About Z64Fe", QString("This is Z64Fe version %1.").arg(
                           PInfo::VERSION.c_str()));
}