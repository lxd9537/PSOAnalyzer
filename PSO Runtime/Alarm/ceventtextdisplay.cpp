#include "ceventtextdisplay.h"
#include <QtDebug>
#include <QDateTime>

CEventTextDisplay::CEventTextDisplay(QWidget *parent):QTextEdit(parent)
{
    m_doc = new QTextDocument(this);
    m_cursor =  new QTextCursor(m_doc);
    this->setDocument(m_doc);
    this->setReadOnly(true);


}
void CEventTextDisplay::InsertNewLine(const QString& text) {
    QString datetime = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss");

    if (m_doc->lineCount() > EVENT_TEXT_DISPLAY_MAX_LINES)
        for (int i=0; i<m_doc->lineCount() - EVENT_TEXT_DISPLAY_MAX_LINES; i++)
            DeleteLastLine();
    m_cursor->setPosition(0);
    if (!text.endsWith("\n"))
        m_cursor->insertText(datetime + "  " + text + "\n");
    else
        m_cursor->insertText(datetime + "  " + text);
}

void CEventTextDisplay::DeleteLastLine() {
    m_cursor->movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    m_cursor->movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
    m_cursor->select(QTextCursor::LineUnderCursor);
    m_cursor->removeSelectedText();
    m_cursor->deleteChar();
}

