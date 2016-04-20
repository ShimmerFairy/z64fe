/** \file MainWindow.hpp
 *
 *  \brief This file declares the main window of the application, and as such is
 *  likely to hold only the one class.
 *
 */

#pragma once

#include "ROMFileWidget.hpp"
#include "ROMInfoWidget.hpp"

#include <QMainWindow>
#include <QMdiArea>
#include <QDockWidget>

#include <cstdint>
#include <vector>
#include <string>

/** \brief The main window of the application.
 *
 *  This class is the main window you see in the application. Currently we use a
 *  MDI interface to neatly handle all the things you might do in the program;
 *  using subwindows gives us both the flexibility of top-level windows with the
 *  inherently more-interconnected nature of child widgets (at least visually,
 *  but most likely functionally too).
 *
 *  In the past, before this MDI interface, and in the possible future where
 *  there's a choice between MDI or SDI, this main window would instead hold the
 *  file list, and a tabbed section allowing the viewing of various kinds of
 *  info.
 *
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

  private:
    QMdiArea * main_portal; ///< The area where child windows appear

    QDockWidget * file_list_dock; ///< Holds the file list and operations on those files
    QDockWidget * rom_info_dock;  ///< Holds info on the ROM and any operations on said ROM

    ROMFileWidget * file_list_widget; ///< Actual widget for file_list_dock
    ROMInfoWidget * rom_info_widget;  ///< Actual widget for rom_info_dock

    QMenu * file_menu; ///< The "File" option on the menu bar
    QMenu * help_menu; ///< The "Help" option on the menu bar

    QToolBar * actions_toolbar; ///< The main toolbar of the window

    QAction * load_rom;  ///< Action for loading a ROM file
    QAction * exit_prog; ///< Action for exiting the program

    QAction * about_this; ///< Action displaying about this program.
    QAction * about_qt;   ///< Info about the Qt toolkit we're using.

    ROM::ROM * the_rom; ///< ROM file currently in use (this class owns the pointer)

  private slots:
    /** \brief Qt slot for opening a ROM
     *
     *  This slot performs the opening of a ROM file. Specifically, it will
     *  first ask for a file to open (via the standard dialog box), and then it
     *  will perform the parts of the loading operation it needs to do. (The
     *  actual ROM processing is handled elsewhere.)
     *
     */
    void openROM();

    void makeHexWindow(ROM::File rf);
    void makeTextWindow();

    void aboutMe();

  protected:
    /** \brief Reimplementation of Qt close event
     *
     *  This lets us handle the process for closing the application in a central
     *  manner, whether closed through the \c exit_prog action, or via the
     *  window manager (e.g. the "X" in your titlebar).
     *
     */
    virtual void closeEvent(QCloseEvent * ev) override;

  signals:
    void romChanged(ROM::ROM * tr);

  public:
    /** \brief Constructs the window to prepare for being shown.
     *
     *  This function simply does all the preparatory work in setting up the GUI
     *  (since we don't use .ui files or QtQuick or whatever for constructing
     *  GUIs)
     *
     */
    MainWindow();
};