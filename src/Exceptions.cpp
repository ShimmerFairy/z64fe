/** \file Exceptions.cpp
 *
 *  \brief Implement all the exception classes
 *
 */

#include "Exceptions.hpp"

std::string Exception::what() {
    return "Some kind of general exception. Not only was there an error, someone didn't even use a specific one! Now you've got two problems.";
}

namespace X {
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
}