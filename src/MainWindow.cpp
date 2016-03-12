/** \file MainWindow.cpp
 *
 *  \brief Implements the main window.
 *
 */

#include "MainWindow.hpp"
#include "utility.hpp"

#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QHeaderView>
#include <QSettings>

#include <iostream>
#include <fstream>
#include <algorithm>

MainWindow::MainWindow() {
    dummy = new QWidget;
    qvb = new QVBoxLayout;
    filesView = new QTableView;
    qhb = new QHBoxLayout;
    rominfo = new QGroupBox(tr("ROM Info"));
    rigrid = new QGridLayout;

    rnamekey = new QLabel(tr("ROM Name:"));
    rnameval = new QLabel;
    rcodekey = new QLabel(tr("ROM Code:"));
    rcodeval = new QLabel;
    rsizekey = new QLabel(tr("ROM Size:"));
    rsizeval = new QLabel;

    fileinfo = new QGroupBox(tr("File Info"));
    the_rom_model = nullptr;

    rigrid->addWidget(rnamekey, 0, 0, Qt::AlignRight);
    rigrid->addWidget(rnameval, 0, 1, Qt::AlignLeft);
    rigrid->addWidget(rcodekey, 1, 0, Qt::AlignRight);
    rigrid->addWidget(rcodeval, 1, 1, Qt::AlignLeft);
    rigrid->addWidget(rsizekey, 2, 0, Qt::AlignRight);
    rigrid->addWidget(rsizeval, 2, 1, Qt::AlignLeft);

    rominfo->setLayout(rigrid);

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
    connect(hexviewbtn, &QPushButton::clicked, this, &MainWindow::openRawView);

    decompviewbtn = new QPushButton(tr("&Decompress and View Raw"));
    connect(decompviewbtn, &QPushButton::clicked, this, &MainWindow::decompAndOpen);

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

    qhb->addWidget(rominfo);
    qhb->addWidget(fileinfo);

    qvb->addWidget(filesView);
    qvb->addLayout(qhb);

    dummy->setLayout(qvb);

    setCentralWidget(dummy);
    setWindowTitle(tr("File Viewer"));

    // make window create status bar
    statusBar();

    // set up menus and such
    fileMenu = menuBar()->addMenu(tr("&File"));


    // The freedesktop icon name standard is a perfectly good one, and Qt has
    // continually passed up the opportunity to make it or something else usable
    // as a platform-neutral way to specify typical icons, so failures of this
    // choice on the part of Windows or OSX will be automatically WONTFIX on our
    // end.
    actOpen = new QAction(QIcon::fromTheme("document-open"), tr("&Open ROM..."), this);
    actOpen->setShortcuts(QKeySequence::Open);
    actOpen->setStatusTip(tr("Open an Ocarina of Time or Majora's Mask ROM."));

    connect(actOpen, &QAction::triggered, this, &MainWindow::openROM);

    actQuit = new QAction(QIcon::fromTheme("application-exit"), tr("&Quit"), this);
    actQuit->setShortcuts(QKeySequence::Quit);
    actQuit->setStatusTip(tr("Exit this program."));

    connect(actQuit, &QAction::triggered, this, &MainWindow::close);

    fileMenu->addAction(actOpen);
    fileMenu->addSeparator();
    fileMenu->addAction(actQuit);
}

void MainWindow::openROM() {
    QSettings qs;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open ROM"),
                                                    qs.value("main/lastfile", QString()).toString(),
                                                    tr("N64 ROM Files (*.z64 *.n64);;All files (*)"));
    if (!fileName.isEmpty()) {
        qs.setValue("main/lastfile", fileName);
        processROM(fileName.toStdString());
    }
}

void MainWindow::processROM(std::string fileName) {
    std::ifstream file;

    // this is so we (hopefully) get an explanation from std on why a file fails
    // to open
    file.exceptions(std::ifstream::failbit);

    try {
        file.open(fileName, std::ios_base::binary | std::ios_base::ate);
    } catch (std::exception & e) {
        QMessageBox::critical(this, "Z64Fe",
                              tr("Can't open file %1:\n%2.").arg(fileName.c_str()).arg(e.what()));
        return;
    }

    file.exceptions(std::ifstream::goodbit); // and now we're done with that

    size_t fsize = file.tellg();
    file.seekg(0);

    // now we need to find the magic word, "zelda@" (or "ezdl@a" for byteswapped
    // roms). Hopefully someday we'll be able to intuit how the N64 handles a
    // ROM.

    statusBar()->showMessage(tr("Seeking magic word..."));

    // we do this search by looking for a 'z', and once we find it an 'e', 'l',
    // etc. if it fails at any point, we start the search again with the
    // character that failed (in case _it_ is a 'z'). Also applies for ezdl@a.

    const size_t rom_npos = -1;
    size_t foundPos = rom_npos;
    bool isflipped = false;

    const std::string magic = "zelda@";
    const std::string more_magic = "ezdl@a";

    while (file) {
        if (file.peek() == 'z') {
            foundPos = file.tellg();

            for (size_t i = 0; i < magic.size(); i++) {
                if (file.get() != magic[i]) {
                    foundPos = rom_npos;
                    break;
                }
            }

            if (foundPos != rom_npos) {
                break;
            }
        } else if (file.peek() == 'e') {
            foundPos = file.tellg();

            for (size_t i = 0; i < more_magic.size(); i++) {
                if (file.get() != more_magic[i]) {
                    foundPos = rom_npos;
                    break;
                }
            }

            if (foundPos != rom_npos) {
                isflipped = true;
                break;
            }
        } else {
            // didn't work, just consume and move on
            file.get();
        }
    }

    statusBar()->clearMessage();

    if (foundPos == rom_npos) {
        QMessageBox::critical(this, tr("Error in reading ROM"),
                              tr("Could not find magic word; are you sure this a Z64 ROM?"));
        return;
    }

    statusBar()->showMessage(tr("Found magic word at 0x%1").arg(foundPos, 0, 16));

    file.seekg(0);

    // get raw data to pass to ROM object.
    std::vector<uint8_t> raws;

    for (size_t i = 0; i < fsize; i++) {
        raws.push_back(file.get());
    }

    // if we have an existing model, start the reset operation
    if (the_rom_model != nullptr) {
        the_rom_model->startResetting();
    }

    the_rom = ROM(raws);

    if (isflipped) {
        statusBar()->showMessage(tr("Byteswapping ROM..."));
        the_rom.byteSwap();
    }

    statusBar()->showMessage(tr("Assembling list of files..."));

    size_t numfiles;

    numfiles = the_rom.bootstrapTOC(foundPos + 0x30);

    // if we haven't loaded a ROM before, set up everything we need
    if (the_rom_model == nullptr) {
        the_rom_model = new ROMFileModel(&the_rom);
        filesView->setModel(the_rom_model);

        filesView->setSelectionMode(QAbstractItemView::SingleSelection);
        filesView->setSelectionBehavior(QAbstractItemView::SelectRows);
    } else {
        the_rom_model->endResetting();
    }

    // connect table view's signal
    connect(filesView->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::chooseFile);

    // now to set up ROM info labels
    rnameval->setText(the_rom.get_rname().c_str());
    rcodeval->setText(the_rom.get_rcode().c_str());
    rsizeval->setText(sizeToIEC(the_rom.size()).c_str());

    statusBar()->showMessage(tr("Found %1 files in the list.").arg(numfiles), 5000);
}

void MainWindow::chooseFile(const QModelIndex & cur, const QModelIndex & /*old*/) {
    curfile = the_rom.fileAt(cur.row());

    fplocval->setText(QString("0x%1").arg(
                         QString("%1").arg(
                             curfile.record().pstart, 8, 16, QChar('0'))
                         .toUpper()));

    fpsizeval->setText(sizeToIEC(curfile.record().psize()).c_str());

    fvlocval->setText(QString("0x%1").arg(
                         QString("%1").arg(
                             curfile.record().vstart, 8, 16, QChar('0'))
                         .toUpper()));

    fvsizeval->setText(sizeToIEC(curfile.record().vsize()).c_str());

    fcmprval->setText(curfile.record().isCompressed() ? "yes" : "no");

    femptyval->setText((curfile.record().isMissing() || curfile.size() == 0) ? "yes" : "no");
}

QFrame * MainWindow::makeGridLine(Qt::Orientation orient) {
    if (orient == Qt::Horizontal) {
        QFrame * qline = new QFrame();
        qline->setFrameShape(QFrame::HLine);
        qline->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        return qline;
    } else {
        QFrame * qline = new QFrame();
        qline->setFrameShape(QFrame::VLine);
        qline->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        return qline;
    }
}

void MainWindow::openRawView() {
    HexViewer * newview = new HexViewer(curfile);

    connect(newview, &QTableView::destroyed, this, &MainWindow::rmWindow);
    childWindows.push_back(newview);

    newview->show();
}

void MainWindow::decompAndOpen() {
    HexViewer * newview = new HexViewer(curfile.decompress());

    connect(newview, &QTableView::destroyed, this, &MainWindow::rmWindow);
    childWindows.push_back(newview);

    newview->show();
}

void MainWindow::rmWindow(QObject * item) {
    auto pntat = std::find(childWindows.begin(), childWindows.end(), item);
    if (pntat != childWindows.end()) {
        childWindows.erase(pntat);
    }
}
