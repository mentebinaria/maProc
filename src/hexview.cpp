#include <QSize>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QClipboard>
#include <QFile>
#include <QPainter>
#include <QScrollArea>
#include <QIcon>
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QMessageBox>

#include "include/hexview.hpp"

#define AREA_SIZE size()
#define MIN_HEXCHARS 47
#define MIN_BYTES_LINE 16
#define ADR_HEX 10
#define ADR_LENGTH 10
#define ASCII_HEX 16
#define UPDATE viewport()->update();

#define CHAR_VALID(caracter) ((caracter < 0x20) || (caracter > 0x7e)) ? caracter = '.' : caracter;
#define SET_BACKGROUND_MARK(pos)                   \
    if (pos >= m_selectBegin && pos < m_selectEnd) \
        painter.setBackground(QBrush(QColor(10, 122, 122))), painter.setBackgroundMode(Qt::OpaqueMode);

/**
 * @brief Construct a new Editor Hex:: Editor Hex object
 *
 * @param parent
 */
HexView::HexView(QWidget *p_parent) : QAbstractScrollArea(p_parent)
{
    // initialize members class
#if QT_VERSION >= 0x051100
    m_charWidth = fontMetrics().width(QLatin1Char('9'));
#else
    m_charWidth = fontMetrics().horizontalAdvance(QLatin1Char('9'));
#endif

    m_BufferHex = nullptr;
    m_cursorPos = 0;
    m_charHeight = fontMetrics().height();
    m_posAddr = 0;
    m_posHex = ADR_LENGTH * m_charWidth + ADR_HEX;
    m_posAscii = m_posHex + MIN_HEXCHARS * m_charWidth + ASCII_HEX;
    m_bytesPerLine = MIN_BYTES_LINE;

    // window config
    int calc = m_posAscii + (MIN_BYTES_LINE * m_charWidth);
    setMinimumWidth(calc);
    setMinimumHeight(calc);

    setBackgroundRole(QPalette::Dark);
}

/**
 * @brief Destroy the Editor Hex:: Editor Hex object
 *
 */
HexView::~HexView()
{
    m_BufferHex.clear();
}

/**
 * @brief custom scroll area, designer used for hex editor, paintp_Event will run automatically
 *
 * @param p_event
 */
void HexView::paintEvent(QPaintEvent *p_event)
{
    QPainter painter(viewport());

    updatePositions();
    confScrollBar();

    // scroll bar
    int firstLineIdx = verticalScrollBar()->value();
    int lastLineIdx = firstLineIdx + AREA_SIZE.height() / m_charHeight;
    QByteArray ScrollBuffer = m_BufferHex.mid(firstLineIdx * m_bytesPerLine, (lastLineIdx - firstLineIdx) * m_bytesPerLine);

    for (int lineIdx = firstLineIdx, yPos = m_charHeight; lineIdx < lastLineIdx; lineIdx += 1, yPos += m_charHeight)
    {
        // ascii position
        for (int xPosAscii = m_posAscii, i = 0; ((lineIdx - firstLineIdx) * m_bytesPerLine + i) < ScrollBuffer.size() && (i < m_bytesPerLine); i++, xPosAscii += m_charWidth)
        {
            char caracter = ScrollBuffer[(lineIdx - firstLineIdx) * (uint)m_bytesPerLine + i];
            CHAR_VALID(caracter);

            int pos = ((lineIdx * m_bytesPerLine + i) * 2);
            SET_BACKGROUND_MARK(pos);

            painter.drawText(xPosAscii, yPos, QString(caracter));

            painter.setBackground(painter.brush());
            painter.setBackgroundMode(Qt::OpaqueMode);
        }

        // binary position
        for (int xPos = m_posHex, i = 0; i < m_bytesPerLine && ((lineIdx - firstLineIdx) * m_bytesPerLine + i) < ScrollBuffer.size(); i++, xPos += 3 * m_charWidth)
        {
            int pos = ((lineIdx * m_bytesPerLine + i) * 2);
            SET_BACKGROUND_MARK(pos);

            QString val = QString::number((ScrollBuffer.at((lineIdx - firstLineIdx) * m_bytesPerLine + i) & 0xF0) >> 4, 16);
            painter.drawText(xPos, yPos, val);

            val = QString::number((ScrollBuffer.at((lineIdx - firstLineIdx) * m_bytesPerLine + i) & 0xF), 16);
            painter.drawText(xPos + m_charWidth, yPos, val);

            painter.setBackground(painter.brush());
            painter.setBackgroundMode(Qt::OpaqueMode);
        }
        
        // address offset
        QString address = QString("%1").arg(lineIdx * m_bytesPerLine, 10, 16, QChar('0'));
        painter.drawText(m_posAddr, yPos, address);
    }
}

/**
 * @brief will treat it as copy, select, select all, etc.
 *
 * @param p_event
 */
void HexView::keyPressEvent(QKeyEvent *p_event)
{
    bool setVisible = false;

    if (p_event->matches(QKeySequence::MoveToNextChar))
    {
        setCursorPos(m_cursorPos++);
        resetSelection(m_cursorPos);
        setVisible = true;
    }
    if (p_event->matches(QKeySequence::MoveToPreviousChar))
    {
        setCursorPos(m_cursorPos--);
        resetSelection(m_cursorPos);
        setVisible = true;
    }
    if (p_event->matches(QKeySequence::MoveToEndOfLine))
    {
        setCursorPos(m_cursorPos | ((m_bytesPerLine * 2) - 1));
        resetSelection(m_cursorPos);
        setVisible = true;
    }
    if (p_event->matches(QKeySequence::MoveToStartOfLine))
    {
        setCursorPos(m_cursorPos | (m_cursorPos % (m_bytesPerLine * 2)));
        resetSelection(m_cursorPos);
        setVisible = true;
    }
    if (p_event->matches(QKeySequence::MoveToPreviousLine))
    {
        setCursorPos(m_cursorPos - m_bytesPerLine * 2);
        resetSelection(m_cursorPos);
        setVisible = true;
    }
    if (p_event->matches(QKeySequence::MoveToNextLine))
    {
        setCursorPos(m_cursorPos + m_bytesPerLine * 2);
        resetSelection(m_cursorPos);
        setVisible = true;
    }

    if (p_event->matches(QKeySequence::MoveToNextPage))
    {
        setCursorPos(m_cursorPos + (verticalScrollBar()->height() / m_charHeight - 1) * 2 * m_bytesPerLine);
        resetSelection(m_cursorPos);
        setVisible = true;
    }
    if (p_event->matches(QKeySequence::MoveToPreviousPage))
    {
        setCursorPos(m_cursorPos - (verticalScrollBar()->height() / m_charHeight - 1) * 2 * m_bytesPerLine);
        resetSelection(m_cursorPos);
        setVisible = true;
    }
    if (p_event->matches(QKeySequence::MoveToEndOfDocument))
    {
        setCursorPos(m_BufferHex.size() * 2);
        resetSelection(m_cursorPos);
        setVisible = true;
    }
    if (p_event->matches(QKeySequence::MoveToStartOfDocument))
    {
        setCursorPos(0);
        resetSelection(m_cursorPos);
        setVisible = true;
    }

    if (p_event->matches(QKeySequence::SelectAll))
    {
        resetSelection(0);
        setSelection(2 * m_BufferHex.size() + 1);
        setVisible = true;
    }
    if (p_event->matches(QKeySequence::SelectNextChar))
    {
        std::size_t pos = m_cursorPos + 1;
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }
    if (p_event->matches(QKeySequence::SelectPreviousChar))
    {
        std::size_t pos = m_cursorPos - 1;
        setSelection(pos);
        setCursorPos(pos);
        setVisible = true;
    }
    if (p_event->matches(QKeySequence::SelectEndOfLine))
    {
        std::size_t pos = m_cursorPos - (m_cursorPos % (2 * m_bytesPerLine)) + (2 * m_bytesPerLine);
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }
    if (p_event->matches(QKeySequence::SelectStartOfLine))
    {
        std::size_t pos = m_cursorPos - (m_cursorPos % (2 * m_bytesPerLine));
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }
    if (p_event->matches(QKeySequence::SelectPreviousLine))
    {
        std::size_t pos = m_cursorPos - (2 * m_bytesPerLine);
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }
    if (p_event->matches(QKeySequence::SelectNextLine))
    {
        std::size_t pos = m_cursorPos + (2 * m_bytesPerLine);
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }

    if (p_event->matches(QKeySequence::SelectNextPage))
    {
        std::size_t pos = m_cursorPos + (((verticalScrollBar()->height() / m_charHeight) - 1) * 2 * m_bytesPerLine);
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }
    if (p_event->matches(QKeySequence::SelectPreviousPage))
    {
        std::size_t pos = m_cursorPos - (((verticalScrollBar()->height() / m_charHeight) - 1) * 2 * m_bytesPerLine);
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }
    if (p_event->matches(QKeySequence::SelectEndOfDocument))
    {
        std::size_t pos = 0;
        pos = m_BufferHex.size() * 2;
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }
    if (p_event->matches(QKeySequence::SelectStartOfDocument))
    {
        std::size_t pos = 0;
        setCursorPos(pos);
        setSelection(pos);
        setVisible = true;
    }

    if (p_event->matches(QKeySequence::Copy))
    {
        QString res;
        int idx = 0;
        int copyOffset = 0;

        QByteArray ScrollBuffer = m_BufferHex.mid(m_selectBegin / 2, (m_selectEnd - m_selectBegin) / 2 + 1);
        if (m_selectBegin % 2)
        {
            res += QString::number((ScrollBuffer.at((idx++) / 2) & 0xF), 16) += " ";
            idx++;
            copyOffset = 1;
        }

        int selectedSize = m_selectEnd - m_selectBegin;
        for (; idx < selectedSize; idx += 2)
        {
            if (ScrollBuffer.size() > (copyOffset + idx) / 2)
            {
                QString val = QString::number((ScrollBuffer.at((copyOffset + idx) / 2) & 0xF0) >> 4, 16);
                if (++idx < selectedSize)
                    val += QString::number((ScrollBuffer.at((copyOffset + idx) / 2) & 0xF), 16) += " ";

                res += val;

                if ((idx / 2) % m_bytesPerLine == (m_bytesPerLine - 1))
                    res += "\n";
            }
        }
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(res);
    }

    if (setVisible)
        ensureVisible();
}

/**
 * @brief when moved the pressed mouse will set my cursor to the current position
 *
 * @param p_event
 */
void HexView::mouseMoveEvent(QMouseEvent *p_event)
{
    std::size_t actPos = cursorPos(p_event->pos());
    if (actPos != INT_MAX)
    {
        setCursorPos(actPos);
        setSelection(actPos);
    }

    UPDATE
}

void HexView::mousePressEvent(QMouseEvent *p_event)
{
    std::size_t cPos = cursorPos(p_event->pos());

    if ((QApplication::keyboardModifiers() & Qt::ShiftModifier) && p_event->button() == Qt::LeftButton)
        setSelection(cPos);
    else
        resetSelection(cPos);

    setCursorPos(cPos);

    UPDATE
}

void HexView::confScrollBar()
{
    QSize widgetSize = fullSize();
    verticalScrollBar()->setPageStep(AREA_SIZE.height() / m_charHeight);
    verticalScrollBar()->setRange(0, (widgetSize.height() - AREA_SIZE.height()) / m_charHeight + 1);
}

void HexView::updatePositions()
{
    int serviceSymbolsWidth = ADR_LENGTH * m_charWidth + ADR_HEX + ASCII_HEX;

    m_bytesPerLine = (width() - serviceSymbolsWidth) / (4 * m_charWidth) - 1; // 4 symbols per byte

    m_posAddr = 0;
    m_posHex = ADR_LENGTH * m_charWidth + ADR_HEX;
    m_posAscii = m_posHex + (m_bytesPerLine * 3 - 1) * m_charWidth + ASCII_HEX;
}

void HexView::resetSelection()
{
    m_selectBegin = m_selectInit;
    m_selectEnd = m_selectInit;
}

void HexView::resetSelection(std::size_t p_pos)
{
    m_selectInit = p_pos;
    m_selectBegin = p_pos;
    m_selectEnd = p_pos;
}

void HexView::setSelection(int p_pos)
{
    if (p_pos >= m_selectInit)
    {
        m_selectEnd = p_pos;
        m_selectBegin = m_selectInit;
    }
    else
    {
        m_selectBegin = p_pos;
        m_selectEnd = m_selectInit;
    }
}

void HexView::ensureVisible()
{
    int firstLineIdx = verticalScrollBar()->value();
    int lastLineIdx = firstLineIdx + AREA_SIZE.height() / m_charHeight;

    int cursorY = m_cursorPos / (2 * m_bytesPerLine);

    if (cursorY < firstLineIdx)
        verticalScrollBar()->setValue(cursorY);
    if (cursorY >= lastLineIdx)
        verticalScrollBar()->setValue(cursorY - AREA_SIZE.height() / m_charHeight + 1);
}

/**
 * @brief set cursor position actual
 *
 * @param pos
 */
void HexView::setCursorPos(int p_pos)
{
    int maxPos = m_BufferHex.size() * 2;
    (m_BufferHex.size() % m_bytesPerLine) ? maxPos++ : maxPos;
    (p_pos > maxPos) ? p_pos = maxPos : p_pos;

    m_cursorPos = p_pos;
}

/**
 * @brief get size width and height
 *
 * @return QSize
 */
QSize HexView::fullSize() const
{
    int width = m_posAscii + (m_bytesPerLine * m_charWidth);
    int height = m_BufferHex.size() / m_bytesPerLine;
    (m_BufferHex.size() % m_bytesPerLine) ? height++ : height;

    height *= m_charHeight;

    return QSize(width, height);
}

/**
 * @brief set cursor position
 *
 * @param pos
 * @return std::size_t
 */
std::size_t HexView::cursorPos(const QPoint &p_pos)
{
    std::size_t posActual = std::numeric_limits<std::size_t>::max();

    if ((p_pos.x() >= m_posHex) && (p_pos.x() < (m_posHex + (m_bytesPerLine * 3 - 1) * m_charWidth)))
    {
        int x = (p_pos.x() - m_posHex) / m_charWidth;
        (x % 3) == 0 ? x = (x / 3) * 2 : x = ((x / 3) * 2) + 1;

        int firstLineIdx = verticalScrollBar()->value();
        int y = (p_pos.y() / m_charHeight) * 2 * m_bytesPerLine;
        posActual = x + y + firstLineIdx * m_bytesPerLine * 2;
    }

    return posActual;
}

/**
 * @brief open file and read binary,
 * will load the entire binary into a buffer
 *
 * @param p_fpath name file to open and read
 */
int HexView::LoadBinary(const QString &p_fpath)
{
    int status_exit = OPEN_FAIL;
    QFile qFile;

    qFile.setFileName(p_fpath);

    qFile.open(QFile::ReadOnly);
    if (qFile.isOpen())
    {
        status_exit = OPEN_SUCCESS;

        m_BufferHex = qFile.readAll();

        (m_BufferHex.size() != 0) ? status_exit = READ_SUCCESS : status_exit = READ_FAIL;

        qFile.close();
    }
    else
        throw std::runtime_error("Falied to open file " + p_fpath.toStdString() + " not possible read bin");

    resetSelection(0);

    return status_exit;
}

/**
 * @brief call window dialog Hex Editor, if buffer not loaded, return ERROR_RETURN, else SUCESS_RETURN
 * call window if loaded buffer
 *
 * @return int
 */
int HexView::CallDialog()
{
    m_statusExitWin = EMPTY;

    if (!m_BufferHex.isEmpty())
    {
        m_statusExitWin = FULL;
        show();
    }
    else
        throw std::runtime_error("Buffer empty, pass binary to view");

    return m_statusExitWin;
}

/**
 * @brief clean view hex
 *
 */
void HexView::Clear()
{
    m_BufferHex.clear();
    verticalScrollBar()->setValue(0);

    UPDATE
}