#include "mainwindow.h"
/*
void CreateCsv()
{
    QVector<QStringList> *csvData;
    csvData = new QVector<QStringList>;
    QStringList row;

    row.append("pump unit total flow");
    row.append("1");
    csvData->append(row);
    row.clear();
    row.append("pressure at inlet of pump unit");
    row.append("2");
    csvData->append(row);
    row.clear();

    QString filename;
    QFileDialog fileDialog(this);
    filename = fileDialog.getSaveFileName(this,"Save to file...",QDir::currentPath(),"*.csv");
    if(filename=="")
        return;

    QxtCsvModel *csvModel;
    csvModel = new QxtCsvModel(this);
    csvModel->toCSV(filename);
    csvModel->setSource(filename);
    csvModel->insertRows(0,csvData->size());
    csvModel->insertColumns(0,2);

    for(int i=0; i<csvData->size(); i++)
        for(int j=0; j<2; j++)
            csvModel->setText(i,j,csvData->at(i).at(j));

    csvModel->toCSV(filename);

    delete csvData;
    delete csvModel;
}
*/
