/** \file MainWindow.cpp
 *
 *  \brief Implements the main window.
 *
 */

#include "MainWindow.hpp"
#include "utility.hpp"
#include "HexViewer.hpp"
#include "TextViewer.hpp"
#include "Exceptions.hpp"

#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QHeaderView>
#include <QSettings>
#include <QProgressDialog>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

MainWindow::MainWindow() {
    guiAssembleWindow();
}

void MainWindow::openROM() {

    if (childWindows.size() > 0) {
        if (QMessageBox::question(this, tr("Open windows remaining"),
                                  tr("Some windows working on the current ROM are still open. Would you like to close them and choose a new ROM?"))
            == QMessageBox::No) {
            return;
        } else {
            for (auto & i : childWindows) {
                delete i;
            }

            childWindows.clear();
        }
    }

    QSettings qs;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open ROM"),
                                                    qs.value("main/lastfile", QString()).toString(),
                                                    tr("N64 ROM Files (*.z64 *.n64);;All files (*)"));
    if (!fileName.isEmpty()) {
        qs.setValue("main/lastfile", fileName);
        processROM(fileName.toStdString());
    }
}

void MainWindow::saveROM() {
    QSettings qs;
    QString defName = qs.value("main/lastfile", QString()).toString();

    if (defName != QString()) {
        defName += ".swapped";
    }

    QString saveTo = QFileDialog::getSaveFileName(this, tr("Save ROM"),
                                                  defName,
                                                  tr("N64 ROM Files (*.z64 *.n64);;All files (*)"));

    std::string savname = saveTo.toStdString();

    std::ofstream ofile(savname, std::ios_base::binary);

    if (!ofile) {
        QMessageBox::critical(this, "Z64Fe",
                              tr("Can't open %1 for saving. Not saving.").arg(savname.c_str()));
        return;
    }

    std::vector<uint8_t> romdata = the_rom.getData();

    ofile.write(reinterpret_cast<char*>(romdata.data()), romdata.size());

    if (!ofile) {
        QMessageBox::warning(this, "Z64Fe",
                             tr("Error occurred while writing file. Written ROM may not be complete."));
    }
}

void MainWindow::processROM(std::string fileName) {
    std::ifstream file(fileName, std::ios_base::binary | std::ios_base::ate);

    if (!file) {
        QMessageBox::critical(this, "Z64Fe",
                              tr("Can't open file %1").arg(fileName.c_str()));
        return;
    }

    size_t fsize = file.tellg();
    file.seekg(0);

    QProgressDialog progbox("Seeking magic word...", "Cancel", 0, fsize * 4, this);
    progbox.setWindowModality(Qt::WindowModal);

    progbox.setValue(0);

    // now we need to find the magic word, "zelda@" (or "ezdl@a" for byteswapped
    // roms). Hopefully someday we'll be able to intuit how the N64 handles a
    // ROM.

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

    progbox.setValue(fsize);

    if (foundPos == rom_npos) {
        progbox.cancel();
        QMessageBox::critical(this, tr("Error in reading ROM"),
                              tr("Could not find magic word; are you sure this a Z64 ROM?"));
        return;
    }

    progbox.setLabelText(tr("Copying ROM to memory..."));

    file.seekg(0);

    // get raw data to pass to ROM object.
    std::vector<uint8_t> raws;

    for (size_t i = 0; i < fsize; i++) {
        raws.push_back(file.get());

        if (i != 0 && i % 0x1000 == 0) {
            progbox.setValue(progbox.value() + 0x1000);
        }
    }

    // if we have an existing model, start the reset operation
    if (the_rom_model != nullptr) {
        the_rom_model->startResetting();
    }

    the_rom = ROM::ROM(raws);

    if (isflipped) {
        progbox.setLabelText(tr("Byteswapping ROM..."));
        the_rom.byteSwap();
    }

    progbox.setValue(progbox.value() + fsize);

    progbox.setLabelText(tr("Assembling list of files..."));

    size_t numfiles;

    try {
        numfiles = the_rom.bootstrapTOC(foundPos + 0x30);
    } catch(Exception & e) {
        progbox.cancel();
        QMessageBox::critical(this, tr("Error in reading ROM"),
                              tr("An error occurred while processing the ROM: %1").arg(e.what().c_str()));
        std::exit(1);
    }

    progbox.setValue(fsize * 4);

    // if we haven't loaded a ROM before, set up everything we need
    if (the_rom_model == nullptr) {
        the_rom_model = new ROMFileModel(&the_rom);
        filesView->setModel(the_rom_model);

        filesView->setSelectionMode(QAbstractItemView::SingleSelection);
        filesView->setSelectionBehavior(QAbstractItemView::SelectRows);
    } else {
        the_rom_model->endResetting();
    }

    filesView->resizeColumnsToContents();

    // now we can show the real screen (no change if not on intro screen)
    setCentralWidget(dummy);

    // connect table view's signal
    connect(filesView->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::chooseFile);

    hexviewbtn->setEnabled(true);

    if (isflipped) {
        savebs->setText(tr("&Save Byteswapped ROM"));
        savebs->setEnabled(true);
    } else {
        savebs->setText(tr("Not Initially Byteswapped"));
        savebs->setEnabled(false);
    }

    // now to set up ROM info labels
    rnameval->setText(the_rom.get_rname().c_str());
    rcodeval->setText(the_rom.get_rcode().c_str());
    rsizeval->setText(sizeToIEC(the_rom.size()).c_str());
    rversionval->setText(Config::vDisplayStr(the_rom.getVersion()).c_str());

    guiNewROM_TextTab();

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

    if (curfile.record().isCompressed()) {
        decompviewbtn->setEnabled(true);
    } else {
        decompviewbtn->setEnabled(false);
    }
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

void MainWindow::analyzeTextTbl() {
    if (the_midx.size() == 0) {
        try {
            the_midx = TextAST::analyzeMsgTbl(the_rom);
            tNumberValue->setText(QString("%1").arg(the_midx.begin()->second.size()));

            std::stringstream r;

            size_t j = 0;

            for (auto & i : the_midx) {
                j++;
                r << Config::langString(i.first);

                if (j < the_midx.size()) {
                    r << ", ";
                }
            }

            tLangsValue->setText(r.str().c_str());
        } catch (Exception & e) {
            QMessageBox::critical(this, tr("Analysis Error"),
                                  tr("%1").arg(e.what().c_str()));
        }
    }
}

void MainWindow::openTextViewer() {
    analyzeTextTbl();

    TextViewer * newview = new TextViewer(the_rom, the_midx);

    connect(newview, &TextViewer::destroyed, this, &MainWindow::rmWindow);
    childWindows.push_back(newview);

    newview->show();
}

void MainWindow::rmWindow(QObject * item) {
    auto pntat = std::find(childWindows.begin(), childWindows.end(), item);
    if (pntat != childWindows.end()) {
        childWindows.erase(pntat);
    }
}


void MainWindow::close() {
    for (auto & i : childWindows) {
        delete i;
    }

    QMainWindow::close();
}