/**
 * @file hex_editor.hpp
 * @author Mente Binaria
 * @brief hex editor viewing all loaded elf binary
 * @version 1.5.0
 * @date 2022-01-25
 *
 * @copyright Copyright (c) 2022
 *
 * @note This library is a fork from virinext`s QHexView
 *       https://github.com/virinext/QHexView
 */

#pragma once

#include <QByteArray>
#include <QAbstractScrollArea>
#include <QScrollBar>

#include "include/datastructs/erros.hpp"


class HexView : public QAbstractScrollArea
{
    Q_OBJECT

private:
    QByteArray m_BufferHex;

    int m_posAddr,
        m_posHex,
        m_posAscii,
        m_charWidth,
        m_charHeight,
        m_selectBegin,
        m_selectEnd,
        m_selectInit,
        m_cursorPos,
        m_bytesPerLine,
        m_statusExitWin;

    void confScrollBar();
    void updatePositions();
    void resetSelection();
    void resetSelection(std::size_t );
    void setSelection(int );
    void ensureVisible();
    void setCursorPos(int );

    QSize fullSize() const;
    std::size_t cursorPos(const QPoint &);

protected:
    void paintEvent(QPaintEvent *);
    void keyPressEvent(QKeyEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);

public:
    HexView(QWidget * = nullptr);
    ~HexView();
    int LoadBinary(const QString &);
    int CallDialog();
    void Clear();
};
