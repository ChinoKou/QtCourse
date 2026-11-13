#include "codeeditor.h"

#include <QFont>
#include <QFontMetrics>
#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QTextBlock>

CodeEditor::CodeEditor(QWidget* parent) : QPlainTextEdit(parent) {
    m_lineNumberArea = new LineNumberArea(this);

    QFont editorFont(QStringLiteral("Consolas"), 11);
    editorFont.setStyleHint(QFont::Monospace);
    setFont(editorFont);
    setTabStopDistance(fontMetrics().horizontalAdvance(QLatin1Char(' ')) * 4);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

int CodeEditor::lineNumberAreaWidth() const {
    if (!m_showLineNumbers) {
        return 0;
    }
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    const int space = 20 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void CodeEditor::setLineNumbersVisible(bool visible) {
    m_showLineNumbers = visible;
    m_lineNumberArea->setVisible(visible);
    updateLineNumberAreaWidth(0);
}

bool CodeEditor::areLineNumbersVisible() const {
    return m_showLineNumbers;
}

void CodeEditor::setDarkMode(bool dark) {
    m_darkMode = dark;
    highlightCurrentLine();
    m_lineNumberArea->update();
}

void CodeEditor::updateLineNumberAreaWidth(int) {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect& rect, int dy) {
    if (!m_showLineNumbers) {
        return;
    }
    if (dy != 0) {
        m_lineNumberArea->scroll(0, dy);
    }
    else {
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());
    }
    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

void CodeEditor::resizeEvent(QResizeEvent* event) {
    QPlainTextEdit::resizeEvent(event);
    const QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine() {
    QList<QTextEdit::ExtraSelection> extraSelections;
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        const QColor lineColor = m_darkMode ? QColor(30, 41, 59) : QColor(239, 246, 255);
        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }
    setExtraSelections(extraSelections);
    m_lineNumberArea->update();
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent* event) {
    if (!m_showLineNumbers) {
        return;
    }
    QPainter painter(m_lineNumberArea);
    const QColor bgColor = m_darkMode ? QColor(24, 29, 38) : QColor(248, 250, 252);
    const QColor borderColor = m_darkMode ? QColor(42, 49, 64) : QColor(226, 232, 240);
    const QColor activeColor = m_darkMode ? QColor(96, 165, 250) : QColor(37, 99, 235);
    const QColor mutedColor = m_darkMode ? QColor(100, 116, 139) : QColor(148, 163, 184);

    painter.fillRect(event->rect(), bgColor);
    painter.setPen(borderColor);
    painter.drawLine(m_lineNumberArea->width() - 1, event->rect().top(),
                     m_lineNumberArea->width() - 1, event->rect().bottom());

    const int currentBlock = textCursor().blockNumber();
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            const QString number = QString::number(blockNumber + 1);
            QFont f = painter.font();
            const bool isCurrent = (blockNumber == currentBlock);
            f.setBold(isCurrent);
            painter.setFont(f);
            painter.setPen(isCurrent ? activeColor : mutedColor);
            painter.drawText(0, top, m_lineNumberArea->width() - 10, fontMetrics().height(),
                             Qt::AlignRight | Qt::AlignVCenter, number);
        }
        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}
