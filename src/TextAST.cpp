/** \file TextAST.cpp
 *
 *  \brief Implementation of stuff
 *
 */

#include "TextAST.hpp"

#include <sstream>

namespace TextAST {
    Fragment::Fragment(std::string L) : ftype(Type::Literal), strval(L) { }
    Fragment::Fragment(Color C) : ftype(Type::Color), colval(C) { }
    Fragment::Fragment(Button B) : ftype(Type::Button), btnval(B) { }
    Fragment::Fragment(Type T, uint32_t V) : ftype(T), intval(V) { }

    Type Fragment::getType() const { return ftype; }

    template<>
    Color Fragment::getValue() const {
        if (ftype != Type::Color) {
            throw X::Text::WrongVariant(ftype, Type::Color);
        }

        return colval;
    }

    template<>
    Button Fragment::getValue() const {
        if (ftype != Type::Button) {
            throw X::Text::WrongVariant(ftype, Type::Button);
        }

        return btnval;
    }

    template<>
    std::string Fragment::getValue() const {
        if (ftype != Type::Literal) {
            throw X::Text::WrongVariant(ftype, Type::Literal);
        }

        return strval;
    }

    template<>
    uint32_t Fragment::getValue() const {
        if (ftype == Type::Literal || ftype == Type::Button || ftype == Type::Color) {
            throw X::Text::WrongVariant(ftype);
        }

        return intval;
    }

    bool Fragment::tryMoreText(std::string txt) {
        if (ftype != Type::Literal) {
            return false;
        }

        strval += txt;
        return true;
    }

    void Line::push(Fragment np) {
        pieces.push_back(np);
    }

    void Line::addMoreText(std::string txt) {
        if (pieces.size() == 0 || !pieces.back().tryMoreText(txt)) {
            pieces.emplace_back(txt);
        }
    }

    std::vector<Fragment>::iterator Line::begin() { return pieces.begin(); }
    std::vector<Fragment>::iterator Line::end() { return pieces.end(); }

    void Box::push(Line nl) {
        lines.push_back(nl);
    }

    Line & Box::curline() { return lines.back(); }

    size_t Box::size() const { return lines.size(); }

    std::vector<Line>::iterator Box::begin() { return lines.begin(); }
    std::vector<Line>::iterator Box::end() { return lines.end(); }
}

namespace X {
    namespace Text {
        WrongVariant::WrongVariant(TextAST::Type g) : got(g), expect_specific(false) { }
        WrongVariant::WrongVariant(TextAST::Type g, TextAST::Type e) : got(g), expect(e),
                                                                       expect_specific(true) { }

        std::string WrongVariant::what() {
            std::stringstream msg;

            msg << "You requested type " << static_cast<int>(got);

            if (expect_specific) {
                msg << ", while expecting type " << static_cast<int>(expect) << ".";
            } else {
                msg << ", while expecting something else (could be lots of things).";
            }

            return msg.str();
        }
    }
}