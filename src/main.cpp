/** \file main.cpp
 *
 *  \brief Entry point for the program, unsurprisingly.
 *
 */

#include "MainWindow.hpp"

#include <QApplication>

int main(int argc, char ** argv) {
    QApplication qa(argc, argv);

    MainWindow mw;

    mw.show();

    return qa.exec();
}