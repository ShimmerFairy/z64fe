/** \file TextRender.hpp
 *
 *  \brief Renders text in the game in a reasonable approximation to how it'd
 *  look in-game.
 *
 */

#pragma once

#include "TextAST.hpp"

#include <QFrame>

#include <vector>

class TextRender : public QFrame {
    Q_OBJECT

  private:
    std::vector<TextAST> parts;

  protected:
    void paintEvent(QPaintEvent * ev) override;

  public slots:
    void newText(std::vector<TextAST> np);

  public:
    TextRender();
};