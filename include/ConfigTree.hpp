/** \file ConfigTree.hpp
 *
 *  \brief Declares a system for loading externally-saved info about ROMs.
 *
 */

#pragma once

#include "Config.hpp"

#include <string>
#include <fstream>
#include <vector>
#include <initializer_list>

/** \brief One node of a config tree
 */
class ConfigTreeNode {
  private:
    std::vector<ConfigTreeNode *> children;

    std::string keytxt;
    std::string valtxt;

  public:
    ConfigTreeNode(std::string kt, std::string vt = "");

    ConfigTreeNode(const ConfigTreeNode & copyctn);
    ConfigTreeNode(ConfigTreeNode && movectn);
    ConfigTreeNode & operator=(const ConfigTreeNode & copyctn);
    ConfigTreeNode & operator=(ConfigTreeNode && movectn);
    ~ConfigTreeNode();

    void addChild(ConfigTreeNode * ctn);

    std::string key() const;
    std::string value() const;

    bool isGroup() const;

    ConfigTreeNode * childMatchKey(std::string sk) const;
    ConfigTreeNode * childMatchValue(std::string sv) const;

    size_t numChildren() const;

    std::vector<ConfigTreeNode *>::iterator begin();
    std::vector<ConfigTreeNode *>::iterator end();
};

class ConfigTree {
  private:
    ConfigTreeNode * rootNode;

  public:
    ConfigTree();
    ConfigTree(std::ifstream & infile);

    ConfigTree(const ConfigTree & copyct);
    ConfigTree(ConfigTree && movect);
    ConfigTree & operator=(const ConfigTree & copyct);
    ConfigTree & operator=(ConfigTree && movect);
    ~ConfigTree();

    bool isEmpty();

    std::string getValue(std::string toplvl) const;
    std::string getValue(std::initializer_list<std::string> vp) const;

    std::string findKey(std::initializer_list<std::string> ingroup, std::string findval) const;
};

ConfigTree getConfigTree(Config::Version forVer);