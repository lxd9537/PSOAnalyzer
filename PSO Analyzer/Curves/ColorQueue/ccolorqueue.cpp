#include "ccolorqueue.h"

CColorQueue::CColorQueue()
{
    m_color_list.append(QRgb(0x3daeda));
    m_color_list.append(QRgb(0x13c316));
    m_color_list.append(QRgb(0x0c2673));
    m_color_list.append(QRgb(0xffed24));
    m_color_list.append(QRgb(0x2fa3b4));

    m_color_list.append(QRgb(0x2685bf));
    m_color_list.append(QRgb(0x3e703e));
    m_color_list.append(QRgb(0x5f3dba));
    m_color_list.append(QRgb(0x36d98d));
    m_color_list.append(QRgb(0xc913a7));
    m_color_index = 0;
}
QColor CColorQueue::nextColor() {
    if (m_color_index + 1 >= m_color_list.count()) {
        m_color_index = 0;
    } else {
        m_color_index += 1;
    }
    return m_color_list.at(m_color_index);
}
