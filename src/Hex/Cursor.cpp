/** \file Hex/Cursor.cpp
 *
 *  \brief Implements the hex cursor
 *
 */

#include "Hex/Cursor.hpp"

#include <algorithm>

namespace Hex {
    Cursor::Cursor(size_t d, size_t l) : datasize(d), lpp(l) { }

    void Cursor::newPageSize(size_t newlpp) {
        lpp = newlpp;
    }

    void Cursor::highlightOn() {
        if (region == false) {
            region = true;
            mark = pos;
        }
    }

    void Cursor::highlightOff() {
        region = false;
    }

    bool Cursor::isHighlighting() { return region; }

    void Cursor::nextByte() {
        if (pos < datasize) {
            pos++;
        }
    }

    void Cursor::prevByte() {
        if (pos > 0) {
            pos--;
        }
    }

    void Cursor::nextLine() {
        if (pos + 0x10 < datasize) {
            pos += 0x10;
        } else {
            pos = datasize;
        }
    }

    void Cursor::prevLine() {
        if (pos == datasize) {
            pos--;
        }

        if (pos > 0x10) {
            pos -= 0x10;
        } else {
            pos = 0;
        }
    }

    void Cursor::nextPage() {
        if (pos + lpp * 0x10 < datasize) {
            pos += lpp * 0x10;
        } else {
            pos = datasize;
        }
    }

    void Cursor::prevPage() {
        if (pos == datasize) {
            pos--;
        }

        if (pos > lpp * 0x10) {
            pos -= lpp * 0x10;
        } else {
            pos = 0;
        }
    }

    void Cursor::goToStart() {
        pos = 0;
    }

    void Cursor::goToEnd() {
        pos = datasize;
    }

    bool Cursor::onPage(size_t startAddr) const {
        size_t endAddr = std::min(startAddr + lpp * 0x10, datasize);

        return (startAddr <= pos) && (pos < endAddr || (pos == endAddr && endAddr == datasize));
    }

    std::pair<size_t, size_t> Cursor::selectedRange() const {
        std::pair<size_t, size_t> res;

        if (region) {
            res.first  = std::min(pos, mark);
            res.second = std::max(pos, mark);
        } else {
            res.first = res.second = pos;
        }

        return res;
    }

    Cursor::Mode Cursor::getMode() const { return whatmode; }
    Cursor::Side Cursor::getSide() const { return whatside; }

    void Cursor::setMode(Mode nm) { whatmode = nm; }
    void Cursor::setSide(Side ns) { whatside = ns; }

    size_t Cursor::row() const {
        return pos >> 4;
    }

    size_t Cursor::col() const {
        return pos & 0x0F;
    }

    bool Cursor::eof() const {
        return pos == datasize;
    }
}