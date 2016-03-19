/** \file TextIDModel.cpp
 */

#include "TextIDModel.hpp"

TextIDModel::TextIDModel(ROM::File idset, Config::Version theV) {
    // how we run through the file will depend now on what region we're in, and
    // the game of course.

    if (getGame(theV) == Config::Game::Ocarina) {
        if (getRegion(theV) == Config::Region::NTSC) {
            // this format has japanese offsets, then ID 0xFFFF, then english,
            // then 0xFFFF, then DONE!

            auto idx = idset.begin() + codeFileTables.at(theV).at(Config::CodeTbl::Messages);

            while (be_u16(idx) != 0xFFFF) {
                uint16_t off = be_u16(idx); idx += 2;

                // ignore useless-for-now two bytes
                idx += 2;

                uint32_t addr = be_u32(idx); idx += 4;

                // remove bank number, since we don't really need it
                addr = addr & 0x00FFFFFF;

                id_maps[Config::Language::JP][off] = addr;
            }

            // skip over address-less ID FFFF
            idx += 8;

            while (be_u16(idx) != 0xFFFF) {
                uint16_t off = be_u16(idx); idx += 2;

                // useless bytes
                idx += 2;

                uint32_t addr = be_u32(idx); idx += 4;

                addr = addr & 0x00FFFFFF;

                id_maps[Config::Language::EN][off] = addr;
            }
        } else if (getRegion(theV) == Config::Region::PAL) {
            // here, it's english offsets, then ID 0xFFFF, then German, then
            // address 0x0000, then French, then 0x0000, then done!

            // since the German and French lists don't explicitly give IDs, we
            // need to keep a list of the English IDs so we can keep them
            // straight in the next languages

            std::vector<uint16_t> idorder;

            auto idx = idset.begin() + codeFileTables.at(theV).at(Config::CodeTbl::Messages);

            while (be_u16(idx) != 0xFFFF) {
                idorder.push_back(be_u16(idx)); idx += 2;

                // ignore next two bytes
                idx += 2;

                uint32_t addr = be_u32(idx); idx += 4;

                addr = addr & 0x00FFFFFF;

                id_maps[Config::Language::EN][idorder.back()] = addr;
            }

            idx += 8; // skip over "bad" ID

            size_t de_i = 0;

            // now for german
            while (be_u32(idx) != 0x00000000) {
                id_maps[Config::Language::DE][idorder[de_i]] = be_u32(idx);
                idx += 4;
                de_i++;
            }

            idx += 4; // skip over empty address

            size_t fr_i = 0;

            // now, french
            while (be_u32(idx) != 0x00000000) {
                id_maps[Config::Language::FR][idorder[fr_i]] = be_u32(idx);
                idx += 4;
                fr_i++;
            }
        } else {
            QMessageBox::critical(this, tr("Text Viewer error"),
                                  tr("Bad region detected for ROM. No text to show."));
        }
    } else {
        QMessageBox::critical(this, tr("NYI"),
                              tr("Majora's Mask not yet supported for this feature."));
    }
}


Qt::ItemFlags TextIDModel::flags(const QModelIndex & index) const {
    if (!index.isValid()) {
        return 0;
    }

    return QAbstractItemModel::flags(index);
}

QVariant TextIDModel::data(const QModelIndex & index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (index.internalId() == 0) {
        return Config::langString((id_maps.begin() + index.row())->first).c_str();
    } else {
        return QString("ID 0x%1").arg(
            QString("%1").arg(
                ((id_maps.begin() + index.internalId() - 1)->second.begin() + index.row())->first,
                4,
                16,
                QChar('0')).toUpper());
    }
}

QVariant TextIDModel::headerData(int sect, Qt::Orientation orient, int role) const {
    if (role != Qt::DisplayRole || orient != Qt::Horizontal || sect > 0) {
        return QVariant();
    }

    return "ID";
}

QModelIndex TextIDModel::index(int row, int col, const QModelIndex & parent) const {
    if (!parent.isValid()) {
        return createIndex(row, col, 0);
    }

    return createIndex(row, col, parent.row() + 1);
}

QModelIndex TextIDModel::parent(const QModelIndex & index) const {
    if (!index.isValid() || index.internalId() == 0) {
        return QModelIndex();
    }

    return createIndex(index.internalId() - 1, index.col(), 0);
}

int TextIDModel::rowCount(const QModelIndex & parent) const {
    if (parent.column() > 0) {
        return 0;
    }

    if (!parent.isValid()) {
        return id_maps.size();
    }

    return (id_maps.begin() + parent.internalId() - 1)->second.size();
}

int TextIDModel::columnCount(const QModelIndex & /*parent*/) const {
    return 1;
}