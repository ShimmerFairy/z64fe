/** \file HexViewer.hpp
 *
 *  \brief A basic file viewer, displaying in hex
 *
 */

#pragma once

#include "ROM.hpp"

#include <QAbstractTableModel>
#include <QMainWindow>
#include <QTableView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QMenuBar>
#include <QAction>
#include <QDialog>

#include <map>

class HexFileModel : public QAbstractTableModel {
  protected:
    ROM::File myfile;

  public:
    HexFileModel(ROM::File mf);

    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    int columnCount(const QModelIndex & parent = QModelIndex()) const override;

    QVariant data(const QModelIndex & idx, int role = Qt::DisplayRole) const override;
    QVariant headerData(int sect, Qt::Orientation orient, int role = Qt::DisplayRole) const override;
};

class HexFileTextModel : public HexFileModel {
  private:
    const std::map<uint8_t, QString> abbrtbl {
        { 0x00, "NUL"  },
        { 0x01, "SOH"  },
        { 0x02, "STX"  },
        { 0x03, "ETX"  },
        { 0x04, "EOT"  },
        { 0x05, "ENQ"  },
        { 0x06, "ACK"  },
        { 0x07, "BEL"  },
        { 0x08, "BS"   },
        { 0x09, "HT"   },
        { 0x0A, "LF"   },
        { 0x0B, "VT"   },
        { 0x0C, "FF"   },
        { 0x0D, "CR"   },
        { 0x0E, "SO"   },
        { 0x0F, "SI"   },
        { 0x10, "DLE"  },
        { 0x11, "DC1"  },
        { 0x12, "DC2"  },
        { 0x13, "DC3"  },
        { 0x14, "DC4"  },
        { 0x15, "NAK"  },
        { 0x16, "SYN"  },
        { 0x17, "ETB"  },
        { 0x18, "CAN"  },
        { 0x19, "EM"   },
        { 0x1A, "SUB"  },
        { 0x1B, "ESC"  },
        { 0x1C, "FS"   },
        { 0x1D, "GS"   },
        { 0x1E, "RS"   },
        { 0x1F, "US"   },
        { 0x7F, "DEL"  },
        { 0x80, "PAD"  },
        { 0x81, "HOP"  },
        { 0x82, "BPH"  },
        { 0x83, "NBH"  },
        { 0x84, "IND"  },
        { 0x85, "NEL"  },
        { 0x86, "SSA"  },
        { 0x87, "ESA"  },
        { 0x88, "HTS"  },
        { 0x89, "HTJ"  },
        { 0x8A, "VTS"  },
        { 0x8B, "PLD"  },
        { 0x8C, "PLU"  },
        { 0x8D, "RI"   },
        { 0x8E, "SS2"  },
        { 0x8F, "SS3"  },
        { 0x90, "DCS"  },
        { 0x91, "PU1"  },
        { 0x92, "PU2"  },
        { 0x93, "STS"  },
        { 0x94, "CCH"  },
        { 0x95, "MW"   },
        { 0x96, "SPA"  },
        { 0x97, "EPA"  },
        { 0x98, "SOS"  },
        { 0x99, "SGCI" },
        { 0x9A, "SCI"  },
        { 0x9B, "CSI"  },
        { 0x9C, "ST"   },
        { 0x9D, "OSC"  },
        { 0x9E, "PM"   },
        { 0x9F, "APC"  }};

  public:
    HexFileTextModel(ROM::File mf);

    QVariant data(const QModelIndex & idx, int role = Qt::DisplayRole) const override;
};

class HexViewer : public QMainWindow {
  private:
    ROM::File fcopy;

    HexFileModel hfm;
    HexFileTextModel hftm;
    QTableView hexside;
    QTableView txtside;

    QHBoxLayout * hbl;

    QWidget * dumwidg;

    QMenu * fileMenu;
    QAction * saveItem;

    QMenu * toolsMenu;
    QAction * optionsItem;

  private slots:
    void doOptions();
    void saveFile();

  public:
    HexViewer(ROM::File mf);
};

class HexViewOpts : public QDialog {
  private:
    QVBoxLayout * grps;

    QGroupBox * unprint_opts;
    QVBoxLayout * unprint_box;

    QRadioButton * caret_opt;
    QRadioButton * abbr_opt;
    QRadioButton * colordot_opt;

  private slots:
    void chooseUnprintCaret(bool on);
    void chooseUnprintAbbr(bool on);
    void chooseUnprintDot(bool on);

  public:
    enum class UnprintKind {
        CARET,
        ABBR,
        DOT,
    };

    HexViewOpts(QWidget * parent = nullptr);
};