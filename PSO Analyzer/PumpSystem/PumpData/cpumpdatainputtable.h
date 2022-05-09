#ifndef CPUMPDATAINPUTTABLE_H
#define CPUMPDATAINPUTTABLE_H

#include <QTableWidget>
#include <QMenu>
#include <QVector>
#include <QHeaderView>

class CPumpDataInputTable : public QTableWidget
{
    Q_OBJECT

public:
    CPumpDataInputTable(QWidget *parent = nullptr);

signals:
    void deleteKey();
    void copyKey();
    void pasteKey();

private slots:
    void copyTable();
    void pasteTable();
    void onDeleteKey();

protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    QMenu *m_menu;


};

#endif // CPUMPDATAINPUTTABLE_H
