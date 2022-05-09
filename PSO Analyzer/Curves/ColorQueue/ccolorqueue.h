#ifndef CCOLORQUEUE_H
#define CCOLORQUEUE_H

#include <QColor>

class CColorQueue : public QColor
{
public:
    CColorQueue();
QColor nextColor();

private:
    QList<QColor> m_color_list;
    int m_color_index;

};

#endif // CCOLORQUEUE_H
