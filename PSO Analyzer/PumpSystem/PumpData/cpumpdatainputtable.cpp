#include "cpumpdatainputtable.h"
#include <QApplication>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QDebug>
#include <QKeyEvent>

CPumpDataInputTable::CPumpDataInputTable(QWidget * parent)
    :QTableWidget(parent)
{
    QFont font("Microsoft YaHei", 11);

    this->setFont(font);
    this->setColumnCount(4);
    this->setRowCount(5);

    QStringList header;
    header<<"流量(m3/h)"<<"扬程(m)"<<"功率(kW)"<<"NPSHr";
    this->setHorizontalHeaderLabels(header);
    this->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    this->horizontalHeader()->setFont(font);

    m_menu = new QMenu(this);
    m_menu->addAction("复制", this, &CPumpDataInputTable::copyTable);
    m_menu->addAction("粘贴", this, &CPumpDataInputTable::pasteTable);
    connect(this, &CPumpDataInputTable::deleteKey, this, &CPumpDataInputTable::onDeleteKey);
    connect(this, &CPumpDataInputTable::copyKey, this, &CPumpDataInputTable::copyTable);
    connect(this, &CPumpDataInputTable::pasteKey, this, &CPumpDataInputTable::pasteTable);
}

void CPumpDataInputTable::contextMenuEvent(QContextMenuEvent *event) {
    m_menu->exec(QCursor::pos());
    event->accept();
}
void CPumpDataInputTable::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete) {
        emit deleteKey();
    } else if (event->key() == Qt::Key_C) {
        if (QGuiApplication::keyboardModifiers() == Qt::ControlModifier)
        emit copyKey();
    } else if (event->key() == Qt::Key_V) {
        if (QGuiApplication::keyboardModifiers() == Qt::ControlModifier)
        emit pasteKey();
    }
    QWidget::keyPressEvent(event);
}

void CPumpDataInputTable::copyTable() {
    /* get range */
    QList<QTableWidgetSelectionRange> range_list = this->selectedRanges();
    if (range_list.isEmpty())
        return;
    int left = range_list.first().leftColumn();
    int right = range_list.first().rightColumn();
    int top = range_list.first().topRow();
    int bottom = range_list.first().bottomRow();
    foreach(const QTableWidgetSelectionRange &range, range_list) {
        left = qMin(range.leftColumn(), left);
        right = qMax(range.rightColumn(), right);
        top = qMin(range.topRow(), top);
        bottom = qMax(range.bottomRow(), bottom);
    }

    /* get text */
    QList<QTableWidgetItem*> selItems = this->selectedItems();
    QString sel_str;
    for (int i = top; i <= bottom; i++) {
        for (int j = left; j <= right; j++) {
            if (selItems.contains(item(i,j)))
                sel_str += item(i,j)->text();
            else
                sel_str += "";
            if (j<right)
                sel_str += "\t";
        }
        if (i<bottom)
            sel_str += "\n";
    }
    QClipboard *clip_board = QApplication::clipboard();
    clip_board->setText(sel_str);
}

void CPumpDataInputTable::pasteTable() {
    /* get range */
    QList<QTableWidgetSelectionRange> range_list = this->selectedRanges();
    if (range_list.isEmpty())
        return;
    int left = range_list.first().leftColumn();
    int top = range_list.first().topRow();
    foreach(const QTableWidgetSelectionRange &range, range_list) {
        left = qMin(range.leftColumn(), left);
        top = qMin(range.topRow(), top);
    }

    /* paste text */
    QClipboard *clip_board = QApplication::clipboard();
    QString paste_str = clip_board->text();
    if (paste_str.endsWith('\n'))
        paste_str.remove(paste_str.count()-1, 1);

    QList<QString> row_list = paste_str.split("\n");
    int paste_rows = row_list.size();
    for (int i=top; i < qMin(paste_rows + top, this->rowCount()); i++) {
        QList<QString> col_list = row_list.at(i - top).split("\t");
        int paste_cols = col_list.size();
        for (int j=left; j<qMin(paste_cols + left, this->columnCount()); j++) {
            if (item(i,j) == nullptr) {
                QTableWidgetItem *new_item = new QTableWidgetItem;
                new_item->setText(col_list.at(j - left));
                this->setItem(i,j,new_item);
            } else {
                item(i,j)->setText(col_list.at(j - left));
            }
        }
    }
}

void CPumpDataInputTable::onDeleteKey() {
    qDebug()<<"delete";
    QList<QTableWidgetSelectionRange> range_list = this->selectedRanges();
    foreach(const QTableWidgetSelectionRange &range, range_list) {
        for (int i=range.topRow(); i<=range.bottomRow(); i++) {
            for (int j=range.leftColumn(); j<=range.rightColumn(); j++) {
                if (item(i,j) == nullptr) {
                    QTableWidgetItem *new_item = new QTableWidgetItem;
                    new_item->setText("");
                    this->setItem(i,j,new_item);
                } else {
                    item(i,j)->setText("");
                }
            }
        }
    }
}
