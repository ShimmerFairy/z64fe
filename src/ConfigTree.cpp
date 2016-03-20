/** \file ConfigTree.cpp
 *
 *  \brief Implement stuff
 *
 */

#include "projectinfo.hpp"
#include "ConfigTree.hpp"
#include "Exceptions.hpp"

#include <QString>
#include <QStandardPaths>
#include <QFileInfo>

#include <sstream>
#include <algorithm>
#include <iostream>

ConfigTreeNode::ConfigTreeNode(std::string kt, std::string vt) : keytxt(kt), valtxt(vt) { }

ConfigTreeNode::ConfigTreeNode(const ConfigTreeNode & copyctn) : keytxt(copyctn.keytxt),
                                                                 valtxt(copyctn.valtxt) {
    for (auto & i : copyctn.children) {
        addChild(new ConfigTreeNode(*i));
    }
}

ConfigTreeNode::ConfigTreeNode(ConfigTreeNode && movectn) {
    keytxt = std::move(movectn.keytxt);
    valtxt = std::move(movectn.valtxt);
    children = std::move(movectn.children);
}

ConfigTreeNode & ConfigTreeNode::operator=(const ConfigTreeNode & copyctn) {
    if (&copyctn == this) {
        return *this;
    }

    keytxt = copyctn.keytxt;
    valtxt = copyctn.valtxt;

    for (auto & i : children) {
        delete i;
    }

    children.clear();

    for (auto & i : copyctn.children) {
        addChild(new ConfigTreeNode(*i));
    }

    return *this;
}

ConfigTreeNode & ConfigTreeNode::operator=(ConfigTreeNode && movectn) {
    if (&movectn == this) {
        return *this;
    }

    keytxt = std::move(movectn.keytxt);
    valtxt = std::move(movectn.valtxt);

    for (auto & i : children) {
        delete i;
    }

    children = std::move(movectn.children);

    return *this;
}

ConfigTreeNode::~ConfigTreeNode() {
    for (auto & i : children) {
        delete i;
    }
}

void ConfigTreeNode::addChild(ConfigTreeNode * ctn) {
    children.push_back(ctn);
}

std::string ConfigTreeNode::key() const { return keytxt; }
std::string ConfigTreeNode::value() const { return valtxt; }

bool ConfigTreeNode::isGroup() const { return valtxt == ""; }

ConfigTreeNode * ConfigTreeNode::childMatchKey(std::string sk) const {
    auto findres = std::find_if(children.begin(), children.end(),
                                [&](ConfigTreeNode * c) {
                                    return c->keytxt == sk;
                                });

    if (findres == children.end()) {
        return nullptr;
    } else {
        return *findres;
    }
}

ConfigTreeNode * ConfigTreeNode::childMatchValue(std::string sv) const {
    auto findres = std::find_if(children.begin(), children.end(),
                                [&](ConfigTreeNode * c) {
                                    return c->valtxt == sv;
                                });

    if (findres == children.end()) {
        return nullptr;
    } else {
        return *findres;
    }
}

size_t ConfigTreeNode::numChildren() const { return children.size(); }

std::vector<ConfigTreeNode *>::iterator ConfigTreeNode::begin() { return children.begin(); }
std::vector<ConfigTreeNode *>::iterator ConfigTreeNode::end() { return children.end(); }

ConfigTree::ConfigTree() : rootNode(nullptr) { }

ConfigTree::ConfigTree(std::ifstream & infile) {
    rootNode = new ConfigTreeNode("_");

    std::vector<ConfigTreeNode *> nodestack; // for proper parsing/ASTing of nesting

    nodestack.push_back(rootNode);

    // config files are line-based, so we'll use getline to get each individual
    // statement.

    std::string curline;

    while (std::getline(infile, curline)) {
        std::stringstream linestream(curline);
        linestream >> std::ws;

        if (linestream.str() == "" || linestream.peek() == '#') {
            continue;
        }

        if (linestream.peek() == '}') {
            if (nodestack.size() < 2) {
                throw X::Config::SyntaxError("Unexpected closing brace");
            }

            linestream.get(); // now actually take the brace

            // now to make sure there isn't garbage after the close brace
            while (std::stringstream::traits_type::not_eof(linestream.peek())) {
                if (linestream.peek() != '#' && !std::isspace(linestream.peek())) {
                    throw X::Config::SyntaxError(std::string("Garbage after closing brace: '") + static_cast<char>(linestream.peek()) + std::string("'"));
                } else if (linestream.peek() == '#') {
                    // make sure that we stop when a comment happens, so we
                    // don't pick up the comment itself as garbage
                    break;
                }

                linestream.get();
            }

            nodestack.pop_back();
            continue;
        }

        std::string akey;

        while (std::stringstream::traits_type::not_eof(linestream.peek())
               && linestream.peek() != '='
               && linestream.peek() != '{'
               && linestream.peek() != '#'
               && !std::isspace(linestream.peek())) {
            akey.push_back(linestream.get());
        }

        linestream >> std::ws;

        if (linestream.peek() == '{') { // start of group
            ConfigTreeNode * nn = new ConfigTreeNode(akey);
            nodestack.back()->addChild(nn);
            nodestack.push_back(nn);
            nn = nullptr;
        } else if (linestream.peek() == '=') { // kv pair
            std::string aval;

            linestream.get();
            linestream >> std::ws;

            while (std::stringstream::traits_type::not_eof(linestream.peek()) && linestream.peek() != '#') {
                aval.push_back(linestream.get());
            }

            // we use an empty value internally to indicate a group; besides, an
            // empty value shouldn't be useful in the first place.
            if (aval == "") {
                throw X::Config::SyntaxError("Empty value not allowed!");
            }

            nodestack.back()->addChild(new ConfigTreeNode(akey, aval));
        } else {
            throw X::Config::BadCharacter(linestream.peek());
        }
    }

    if (nodestack.size() > 1) {
        std::cerr << "WARNING: Missing braces at end of config file; properties may be misplaced.\n";
    }
}

ConfigTree::ConfigTree(const ConfigTree & copyct) : rootNode(new ConfigTreeNode(*(copyct.rootNode))) { }

ConfigTree::ConfigTree(ConfigTree && movect) : rootNode(movect.rootNode) {
    movect.rootNode = nullptr;
}

ConfigTree & ConfigTree::operator=(const ConfigTree & copyct) {
    if (&copyct == this) {
        return *this;
    }

    delete rootNode;

    rootNode = new ConfigTreeNode(*(copyct.rootNode));

    return *this;
}

ConfigTree & ConfigTree::operator=(ConfigTree && movect) {
    if (&movect == this) {
        return *this;
    }

    delete rootNode;

    rootNode = movect.rootNode;

    movect.rootNode = nullptr;

    return *this;
}

ConfigTree::~ConfigTree() {
    delete rootNode;
}

bool ConfigTree::isEmpty() { return rootNode == nullptr; }

std::string ConfigTree::getValue(std::string toplvl) const {
    ConfigTreeNode * respair = rootNode->childMatchKey(toplvl);

    if (respair == nullptr) {
        throw X::Config::NoSuchKey(toplvl, true);
    }

    return respair->value();
}

std::string ConfigTree::getValue(std::initializer_list<std::string> vp) const {
    ConfigTreeNode * cur_res = rootNode;

    for (auto & i : vp) {
        cur_res = cur_res->childMatchKey(i);

        if (cur_res == nullptr) {
            throw X::Config::NoSuchKey(i);
        }
    }

    return cur_res->value();
}

std::string ConfigTree::findKey(std::initializer_list<std::string> ingroup, std::string findval) const {
    ConfigTreeNode * cur_res = rootNode;

    for (auto & i : ingroup) {
        cur_res = cur_res->childMatchKey(i);

        if (cur_res == nullptr) {
            throw X::Config::NoSuchKey(i);
        }
    }

    cur_res = cur_res->childMatchValue(findval);

    if (cur_res == nullptr) {
        throw X::Config::NoSuchValue(findval);
    }

    return cur_res->key();
}

ConfigTree getConfigTree(Config::Version forVer) {
    QString getfrom = QStandardPaths::locate(QStandardPaths::AppDataLocation,
                                             (vFileStr(forVer) + ".cfg").c_str());

    if (getfrom == "") {
        getfrom = QString("%1/%2").arg(PInfo::DEVSHAREPATH.c_str()).arg((vFileStr(forVer) + ".cfg").c_str());

        if (!QFileInfo(getfrom).exists()) {
            return ConfigTree();
        }
    }

    std::ifstream cfile(getfrom.toStdString());

    if (!cfile) {
        return ConfigTree();
    }

    return ConfigTree(cfile);
}