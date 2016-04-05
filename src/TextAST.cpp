/** \file TextAST.cpp
 *
 *  \brief Implementation of stuff
 *
 */

#include "TextAST.hpp"
#include "endian.hpp"

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


    BoxKind OoT_BoxKind(uint8_t num) {
        switch (num) {
          case 0:
            return BoxKind::BlackBox;
            break;

          case 1:
            return BoxKind::WoodenBox;
            break;

          case 2:
            return BoxKind::BlueBox;
            break;

          case 3:
            return BoxKind::OcarinaInput;
            break;

          case 4:
            return BoxKind::Nothing;
            break;

          case 5:
            return BoxKind::NothingAndBlackFont;
            break;

          default:
            return BoxKind::Nothing;
            break;
        }
    }

    BoxKind MM_BoxKind(uint8_t num) {
        switch (num) {
          case 0x00:
          case 0x06:
            return BoxKind::BlackBox;
            break;

          case 0x01:
            return BoxKind::WoodenBox;
            break;

          case 0x02:
            // allegedly a different kind of blue box?
            return BoxKind::BlueBox;
            break;

          case 0x03:
            return BoxKind::OcarinaInput;
            break;

          case 0x05:
            return BoxKind::NothingAndBlackFont;
            break;

//          case 0x06:
//            return BoxKind::InNotebook;
//            break;

          case 0x08:
            return BoxKind::BlueBox;
            break;

          case 0x09:
            return BoxKind::RedBox;
            break;

          case 0x0B:
            return BoxKind::NothingAndTextTop;
            break;

          case 0x0D:
            return BoxKind::Notebook;
            break;

          case 0x0F:
            return BoxKind::RedBox;
            break;

          case 0x04:
          case 0x07:
          case 0x0A:
          case 0x0C:
          case 0x0E:
          default:
            return BoxKind::Nothing;
            break;
        }
    }

    BoxYPos OoT_BoxYPos(uint8_t num) {
        switch (num) {
          case 0:
            return BoxYPos::TopOrBottom;
            break;

          case 1:
            return BoxYPos::Top;
            break;

          case 2:
            return BoxYPos::Middle;
            break;

          case 3:
          default:
            return BoxYPos::Bottom;
            break;
        }
    }

    BoxYPos MM_BoxYPos(uint8_t /*num*/) {
        // for now, just return that we want on bottom
        return BoxYPos::Bottom;
    }

    MessageIndex analyzeMsgTbl(ROM::ROM & therom) {
        MessageIndex text_ids;

        if (Config::getGame(therom.getVersion()) == Config::Game::Ocarina) {
            std::vector<uint8_t> codefile;
            ROM::File cf = therom.fileAtName("code");

            if (cf.record().isCompressed()) {
                cf = cf.decompress();
            }

            codefile = cf.getData();

            size_t msgoff = std::stoul(therom.configKey({"codeData", "TextMsgTable"}), nullptr, 0);

            auto iter = codefile.begin() + msgoff;


            if (Config::getRegion(therom.getVersion()) == Config::Region::NTSC) {
                // get japanese, then ID 0xFFFF, then english, then ID 0xFFFF

                // first, japanese
                while (be_u16(iter) != 0xFFFF) {
                    uint16_t id = be_u16(iter);
                    iter += 2;

                    MsgInfo mi;

                    uint8_t infobyte = *iter;
                    mi.kind = OoT_BoxKind(infobyte >> 4);
                    mi.where = OoT_BoxYPos(infobyte & 0x0F);

                    iter += 2; // skip over infobyte and useless byte

                    // removing the top byte because it's a bank number, and we
                    // don't use banks here.
                    mi.address = be_u32(iter) & 0x00FFFFFF;
                    iter += 4;

                    // add to list of japanese IDs
                    text_ids[Config::Language::JP][id] = mi;
                }

                // read past the invalid 0xFFFF ID
                iter += 8;

                // next, english
                while (be_u16(iter) != 0xFFFF) {
                    uint16_t id = be_u16(iter);
                    iter += 2;

                    MsgInfo mi;

                    uint8_t infobyte = *iter;
                    mi.kind = OoT_BoxKind(infobyte >> 4);
                    mi.where = OoT_BoxYPos(infobyte & 0x0F);

                    iter += 2; // skip over infobyte and useless byte

                    mi.address = be_u32(iter) & 0x00FFFFFF;
                    iter += 4;

                    // add to list of japanese IDs
                    text_ids[Config::Language::EN][id] = mi;
                }
            } else if (Config::getRegion(therom.getVersion()) == Config::Region::PAL) {
                // get english, ID 0xFFFF, german addresses, null address,
                // french addresses, null address.

                // Since in this case the other languages don't get the whole ID
                // entry, just addresses listed in order, we'll need to keep a
                // list of IDs and associated info as they come, so we can
                // correctly associate them with the other languages.

                std::vector<uint16_t> idlist;
                std::vector<MsgInfo> milist;

                // First up is English
                while (be_u16(iter) != 0xFFFF) {
                    idlist.push_back(be_u16(iter));
                    iter += 2;

                    MsgInfo mi;

                    uint8_t infobyte = *iter;
                    mi.kind = OoT_BoxKind(infobyte >> 4);
                    mi.where = OoT_BoxYPos(infobyte & 0x0F);

                    iter += 2; // skip over infobyte and useless byte

                    mi.address = be_u32(iter) & 0x00FFFFFF;
                    iter += 4;

                    milist.push_back(mi);

                    // add to list of japanese IDs
                    text_ids[Config::Language::EN][idlist.back()] = mi;
                }

                // skip the fake ID
                iter += 8;

                size_t i = 0; // for pulling correct ID

                // now for German
                while (be_u32(iter) != 0) {
                    MsgInfo mi = milist[i];
                    mi.address = be_u32(iter) & 0x00FFFFFF;

                    text_ids[Config::Language::DE][idlist[i]] = mi;
                    iter+=4;
                    i++;
                }

                // skip empty address
                iter += 4;

                i = 0;

                // finally, French
                while (be_u32(iter) != 0) {
                    MsgInfo mi = milist[i];
                    mi.address = be_u32(iter) & 0x00FFFFFF;

                    text_ids[Config::Language::FR][idlist[i]] = mi;
                    iter+=4;
                    i++;
                }
            } else {
                throw X::InternalError("Somehow got an impossible region for Ocarina of Time.");
            }
        } else if (Config::getGame(therom.getVersion()) == Config::Game::Majora) {
            // note: since the message info is associated with the text itself,
            // we'll use special MsgInfo items saying as much
            if (Config::getRegion(therom.getVersion()) == Config::Region::EU) {
                // for european MM roms, the addresses are in separate files,
                // which thankfully makes this easy

                // the given files should never be compressed, for a
                // well-behaved rom, so we'll assume they aren't.
                std::vector<uint8_t> curfile = therom.fileAtName("nes_message_table").getData();

                for (auto i = curfile.begin(); i != curfile.end(); i += 8) {
                    text_ids[Config::Language::EN][be_u16(i)] = MsgInfo(be_u32(i + 4) & 0x00FFFFFF);
                }

                curfile = therom.fileAtName("ger_message_table").getData();

                for (auto i = curfile.begin(); i != curfile.end(); i += 8) {
                    text_ids[Config::Language::DE][be_u16(i)] = MsgInfo(be_u32(i + 4) & 0x00FFFFFF);
                }

                curfile = therom.fileAtName("fra_message_table").getData();

                for (auto i = curfile.begin(); i != curfile.end(); i += 8) {
                    text_ids[Config::Language::FR][be_u16(i)] = MsgInfo(be_u32(i + 4) & 0x00FFFFFF);
                }

                curfile = therom.fileAtName("esp_message_table").getData();

                for (auto i = curfile.begin(); i != curfile.end(); i += 8) {
                    text_ids[Config::Language::ES][be_u16(i)] = MsgInfo(be_u32(i + 4) & 0x00FFFFFF);
                }
            } else {
                std::vector<uint8_t> codefile;
                ROM::File cf = therom.fileAtName("code");

                if (cf.record().isCompressed()) {
                    cf = cf.decompress();
                }

                codefile = cf.getData();

                size_t msgoff = std::stoul(therom.configKey({"codeData", "TextMsgTable"}), nullptr, 0);

                auto iter = codefile.begin() + msgoff;

                Config::Language whatlang;

                if (Config::getRegion(therom.getVersion()) == Config::Region::JP) {
                    whatlang = Config::Language::JP;
                } else if (Config::getRegion(therom.getVersion()) == Config::Region::US) {
                    whatlang = Config::Language::EN;
                } else {
                    throw X::InternalError("Impossible region obtained for Majora's Mask game.");
                }

                while (be_u16(iter) != 0xFFFF) {
                    text_ids[whatlang][be_u16(iter)] = be_u32(iter + 4) & 0x00FFFFFF;
                    iter += 8;
                }
            }
        } else {
            throw X::InternalError("Somehow got an impossible game from the version info (did this section get missed in some big changes?).");
        }

        return text_ids;
    }
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