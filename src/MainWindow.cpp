/** \file MainWindow.cpp
 *
 *  \brief Implements the main window.
 *
 */

#include "MainWindow.hpp"
#include "Exceptions.hpp"

#include <QToolBar>
#include <QMenuBar>
#include <QCloseEvent>
#include <QSettings>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

MainWindow::MainWindow() {
    the_rom = nullptr;

    main_portal = new QMdiArea(this);

    file_list_widget = new ROMFileWidget(this);
    rom_info_widget = new ROMInfoWidget(this);

    file_list_dock = new QDockWidget(tr("ROM Files"), this);
    rom_info_dock = new QDockWidget(tr("ROM Info"), this);
    text_data_dock = new QDockWidget(tr("Game Text"), this);

    file_list_dock->setWidget(file_list_widget);
    rom_info_dock->setWidget(rom_info_widget);

    addDockWidget(Qt::RightDockWidgetArea, rom_info_dock);
    addDockWidget(Qt::RightDockWidgetArea, file_list_dock);
    addDockWidget(Qt::RightDockWidgetArea, text_data_dock);

    load_rom = new QAction(QIcon::fromTheme("document-open", QIcon(":/icons/document-open.svg")),
                           tr("&Open ROM..."),
                           this);
    load_rom->setShortcut(QKeySequence::Open);

    exit_prog = new QAction(QIcon::fromTheme("application-exit", QIcon(":/icons/application-exit.svg")),
                            tr("&Exit"),
                            this);
    exit_prog->setShortcut(QKeySequence::Quit);

    file_menu = menuBar()->addMenu(tr("&File"));
    file_menu->addAction(load_rom);
    file_menu->addSeparator();
    file_menu->addAction(exit_prog);

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