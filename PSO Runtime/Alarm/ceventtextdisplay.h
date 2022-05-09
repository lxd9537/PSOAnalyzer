#ifndef CEVENTTEXTDISPLAY_H
#define CEVENTTEXTDISPLAY_H

#include <QTextEdit>

/* Event display configuration */
#define EVENT_TEXT_DISPLAY_MAX_LINES       (500)

class CEventTextDisplay : public QTextEdit
{
    Q_OBJECT
public:
    CEventTextDisplay(QWidget *parent=nullptr);
    void InsertNewLine(const QString& text);

private:
    QTextDocument *m_doc;
    QTextCursor *m_cursor;

    void DeleteLastLine();
};

#endif // CEVENTTEXTDISPLAY_H
