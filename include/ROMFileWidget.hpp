/** \file ROMFileWidget.hpp
 *
 *  \brief Declares a widget for a list of files and associated operations.
 *
 *  The widget declared in this file is intended to be a dock widget for
 *  scrolling through files.
 *
 */

#pragma once

#include "ROM.hpp"
#include "ROMFileModel.hpp"

#include <QWidget>
#include <QGridLayout>
#include <QTableView>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>

class ROMFileWidget : public QWidget {
    Q_OBJECT

  private:
    ROM::ROM * the_rom;
    ROMFileModel * the_model;

    QGridLayout * wlay;

    QTableView * filelist;

    QLabel * vloc_key;
    QLabel * vloc_val;
    QLabel * vsize_key;
    QLabel * vsize_val;
    QLabel * ploc_key;
    QLabel * ploc_val;
    QLabel * psize_key;
    QLabel * psize_val;
    QLabel * comp_key;
    QLabel * comp_val;
    QLabel * empty_key;
    QLabel * empty_val;

    QCheckBox * want_dec;

    QPushButton * view_hex;
    QPushButton * save_file;

  private slots:
    /** \brief Slot for selecting an item in the list
     */
    void selectFile(const QModelIndex & cur, const QModelIndex & old);

    void saveFile();

  public slots:
    /** \brief Slot for when a new ROM is chosen
     */
    void changeROM(ROM::ROM * nr);

  public:
    /** \brief Constructs this widget
     *
     *  Assembles the graphical parts of the GUI, and connects signals and slots
     *  together accordingly.
     *
     */
    ROMFileWidget(QWidget * parent = nullptr);
};