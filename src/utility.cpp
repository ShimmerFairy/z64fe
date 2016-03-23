/** \file utility.cpp
 *
 *  \brief Implements utility stuff
 *
 */

#include "utility.hpp"

#include <QString>

#include <sstream>

std::string sizeToIEC(size_t inbytes) {
    double smallsize = inbytes;
    size_t prefix = 0;
    std::stringstream res;

    while (smallsize >= 1024) {
        smallsize /= 1024;
        prefix ++;
    }

    res << smallsize << " ";

    switch (prefix) {
      case 0:
        res << "B";
        break;
      case 1:
        res << "kiB";
        break;
      case 2:
        res << "MiB";
        break;
      case 3:
        res << "GiB";
        break;
      case 4:
        res << "TiB";
        break;
      case 5:
        res << "PiB";
        break;
      case 6:
        res << "EiB";
        break;
      case 7:
        res << "ZiB";
        break;
      case 8:
        res << "YiB";
        break;
      default:
        res << "!!!";
        break;
    }

    return res.str();
}

std::string code_to_utf8(uint32_t codep) {
    // currently the lazy way, maybe want our own solution sometime??

    return QString(QChar(codep)).toStdString();
}