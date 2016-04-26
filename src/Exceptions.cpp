/** \file Exceptions.cpp
 *
 *  \brief Implement all the exception classes
 *
 */

#include "Exceptions.hpp"

#include <sstream>
#include <iomanip>

std::string Exception::what() {
    return "Some kind of general exception. Not only was there an error, someone didn't even use a specific one! Now you've got two problems.";
}

namespace X {
    size_t Multi::size() const {
        return elist.size();
    }

    void Multi::addException(Exception * e) {
        elist.push_back(e);
    }

    std::string Multi::what() {
        std::stringstream res;

        res << elist.size() << " problem" << (elist.size() == 1 ? "" : "s");
        res << " found:\n\n";

        for (auto & i : elist) {
            res << i->what() << "\n\n";
        }

        return res.str();
    }

    InternalError::InternalError(std::string sp) : specifics(sp) { }

    std::string InternalError::what() {
        return "An internal error has occurred! " + specifics + " This is most likely an issue with the internal workings of the program, and not so much a problem on your end.";
    }

    NYI::NYI(std::string whatthing) : feature(whatthing) { }

    std::string NYI::what() {
        return feature + " is not yet implemented.";
    }

    BadROM::BadROM(std::string r) : reason(r) { }

    std::string BadROM::what() {
        return "This is a bad ROM! " + reason;
    }

    BadIndex::BadIndex(std::string wi) : whatidx(wi) { }

    std::string BadIndex::what() {
        return "Bad " + whatidx + " given.";
    }

    NoConfig::NoConfig(std::string t) : trying(t) { }

    std::string NoConfig::what() {
        return trying + " needs configuration data, but we couldn't load one for this ROM. Consider helping the effort to document in config data format!";
    }

    namespace ROM {
        std::string NoMagic::what() {
            return "Magic string not found; are you sure this is a Zelda ROM?";
        }
    }

    namespace Config {
        SyntaxError::SyntaxError(std::string e) : explain(e) { }

        std::string SyntaxError::what() {
            return "Error in parsing config file: " + explain;
        }

        BadCharacter::BadCharacter(char pc) : SyntaxError(std::string("Bad character \"") + pc + std::string("\" encountered.")) { }

        NotFound::NotFound(std::string t, std::string v) : type(t), value(v) { }

        std::string NotFound::what() {
            return "Unable to find " + type + " \"" + value + "\" in config data.";
        }

        NoSuchKey::NoSuchKey(std::string key, bool toplevel) :
            NotFound((toplevel ? "top-level key" : "key"), key) { }

        NoSuchValue::NoSuchValue(std::string val) : NotFound("value", val) { }
    }

    namespace Yaz0 {
        Decompress::Decompress(std::string r) : reason(r) { }

        std::string Decompress::what() {
            return "Error in decompressing file: " + reason;
        }
    }

    namespace Text {
        BadSequence::BadSequence(std::initializer_list<uint8_t> bs, std::string optr) : badseq(bs),
                                                                                        optreason(optr) { }
        std::string BadSequence::what() {
            std::stringstream r;

            r << "Error in decoding text: bad sequence ["
              << std::hex << std::uppercase << std::setfill('0');

            size_t idx = 0;
            for (auto & i : badseq) {
                r << std::setw(2) << i;

                if (++idx < badseq.size()) {
                    r << " ";
                }
            }

            r << "]";

            if (optreason != "") {
                r << " (" << optreason << ")";
            }

            r << ".";

            return r.str();
        }

        std::string HeaderError::what() {
            return "HE!!!";
        }
    }
}