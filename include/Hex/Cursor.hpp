/** \file Hex/Cursor.hpp
 *
 *  \brief Declares the cursor used by the hex widget
 *
 */

#pragma once

#include <cstddef>
#include <utility>

namespace Hex {
    /** \brief Represents the cursor in a hex view
     *
     *  This class represents the cursor in a \c Hex::Widget, equivalent to the
     *  text cursor in a text editor.
     *
     *  Note that this cursor assumes to be in overwrite mode by default, so any
     *  editing capabilities won't grow/shrink the data.
     *
     *  This cursor supports being one past the end of the data, for the
     *  purposes of appending data in insert mode. This position is also
     *  supported by overwrite mode, to allow highlighting the last byte of
     *  data.
     *
     */
    class Cursor {
      public:
        /** \brief The cursor's current mode
         *
         *  This enum indicates whether the Cursor is operating in overwrite or
         *  insert mode.
         *
         */
        enum class Mode {
            Overwrite,
            Insert,
        };

        /** \brief Indicates which side of the widget the cursor's on
         *
         *  This enum tells which side of the hex viewer widget the cursor is
         *  currently on. This primarily affects how editing the data works
         *  (i.e. whether you're typing ISO-8859-1 chars or hex digits).
         *
         */
        enum class Side {
            Hex,
            Text,
        };

      private:
        size_t pos = 0;  ///< Where the cursor is in the data.
        size_t mark = 0; ///< For region highlighting, which byte to start from.
        size_t datasize; ///< Size of data which cursor belongs to.
        size_t lpp;      ///< Number of lines/rows per page.

        bool region = false; ///< Indicates highlighting (as opposed to magic values).

        Mode whatmode = Mode::Overwrite; ///< Current cursor mode.
        Side whatside = Side::Hex;       ///< Current editor side.

      public:
        /** \brief Makes a Cursor for some data.
         *
         *  This constructor takes the size of the data being cursored over, as
         *  well as the number of rows to be seen per page, for various sizing
         *  calculations done by the Cursor.
         *
         *  \param[in] d The size of the data, in bytes.
         *
         *  \param[in] l The number of rows per page (i.e. maximum amount of
         *               data visible at any one time in units of 0x10 bytes).
         *
         */
        Cursor(size_t d, size_t l);


        /** \brief Set a new page size for the data.
         *
         *  To be used when resizing the widget to which this Cursor belongs. It
         *  updates the number of rows that can be seen on-screen at any one
         *  time, needed for various calculations.
         *
         *  \param[in] newlpp The new number of rows per page.
         *
         */
        void newPageSize(size_t newlpp);


        /** \brief Begins data highlighting in the Cursor.
         *
         *  Sets the "mark point" (to semi-borrow emacs terminology) to begin
         *  highlighting data.
         *
         */
        void highlightOn();

        /** \brief Stops data highlighting in the Cursor.
         *
         *  This stops highlighting, and should visually get rid of any
         *  highlit region.
         *
         */
        void highlightOff();

        /** \brief Queries the state of highlighting in the Cursor.
         *
         *  Tells you whether highlighting is on or off.
         *
         *  \returns Boolean indicating if the Cursor is currently highlighting.
         *
         */
        bool isHighlighting();


        void nextByte();
        void prevByte();


        void nextLine();
        void prevLine();


        void nextPage();
        void prevPage();

        void goToStart();
        void goToEnd();

        bool onPage(size_t startAddr) const;

        /** \brief Gives the current selected range of bytes.
         *
         *  This returns the range of bytes the cursor has currently selected.
         *
         *  If the cursor is <b>currently highlighting</b>, the returned range
         *  is the highlit range. The first number is never greater than the
         *  second, even if highlighting is being done "backwards" (selecting
         *  up/leftwards of the "mark point").
         *
         *  If the cursor is <b>not highlighting</b>, the returned range will
         *  always be a zero-width range at the cursor's current position.
         *
         *  \returns A \c std::pair object whose items specify the range. \c
         *           first is the first character, and \c second is one past the
         *           last character.
         *
         *  \warning The second item in the returned pair <b>is exclusive</b>;
         *           that is, it's the equivalent to iterators' \c end()
         *           iterator. Among other consequences, highlighting from a
         *           byte in a "backwards" direction (towards the beginning)
         *           will \em not include that byte.
         *
         */
        std::pair<size_t, size_t> selectedRange() const;


        Mode getMode() const;
        Side getSide() const;

        void setMode(Mode nm);
        void setSide(Side ns);

        size_t row() const;
        size_t col() const;

        bool eof() const;
    };
}