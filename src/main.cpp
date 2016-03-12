/** \file main.cpp
 *
 *  \brief Entry point for the program, unsurprisingly.
 *
 */

#include "MainWindow.hpp"

#include <QApplication>

int main(int argc, char ** argv) {
    QApplication qa(argc, argv);

    QCoreApplication::setOrganizationName("ShimmerFairy");
    QCoreApplication::setApplicationName("Z64Fe");
    QCoreApplication::setApplicationVersion("0.0.0");

    MainWindow mw;

    mw.show();

    return qa.exec();
}