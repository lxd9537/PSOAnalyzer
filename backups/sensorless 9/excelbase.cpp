#include "excelbase.h"
#include <QFile>
#include <QDebug>

ExcelBase::ExcelBase()
    : excel(nullptr)
    , books(nullptr)
    , book(nullptr)
    , sheets(nullptr)
    , sheet(nullptr)
{
}

ExcelBase::~ExcelBase()
{
    destory();
    if(excel)
    {
        if (!excel->isNull())
        {
            excel->dynamicCall("Quit()");
        }
    }
    TC_FREE(sheet );
    TC_FREE(sheets);
    TC_FREE(book  );
    TC_FREE(books );
    TC_FREE(excel );
}

void ExcelBase::construct()
{

    destory();
    excel = new QAxObject();
    excel->setControl("Excel.Application");
    excel->setProperty("Visible",false);
    if (excel->isNull())
    {
        excel->setControl("ET.Application");
    }
    if (!excel->isNull())
    {
        books = excel->querySubObject("Workbooks");
    }

}

void ExcelBase::destory()
{

    TC_FREE(sheet );
    TC_FREE(sheets);
    if (book != nullptr && ! book->isNull())
    {
        book->dynamicCall("Close(Boolean)", false);
    }
    TC_FREE(book );
    TC_FREE(books);
    if (excel != nullptr && !excel->isNull())
    {
        excel->dynamicCall("Quit()");
    }
    TC_FREE(excel);
    filename  = "";
    sheetName = "";

}

bool ExcelBase::create(const QString& filename)
{
    bool ret = false;

    construct();
    if (books != nullptr && ! books->isNull())
    {
        books->dynamicCall("Add");
        book   = excel->querySubObject("ActiveWorkBook");
        sheets = book ->querySubObject("WorkSheets"    );
        currentSheet();
        this->filename = filename;
        ret = true;
    }
    return ret;
}

bool ExcelBase::open(const QString& filename)
{
    bool ret = false;

    construct();
    if ( books != nullptr && ! books->isNull() )
    {
        book = books->querySubObject("Open(QString, QVariant)", filename, 0);
        ret = book != nullptr && ! book->isNull();
        if ( ret )
        {
            sheets = book->querySubObject("WorkSheets");
            this->filename = filename;
            currentSheet();
        }
    }
    return ret;
}

void ExcelBase::saveAs(const QString& filename)
{


    if ( book != nullptr && ! book->isNull() )
    {
        this->filename = filename;
        QString strPath = filename;
        strPath = strPath.replace('/','\\');
        //qDebug()<<strPath;
        book->dynamicCall("SaveAs(const QString&,)", strPath);
    }


}

void ExcelBase::saveAsPDF(const QString& filename)
{


    if ( book != nullptr && ! book->isNull() )
    {
        this->filename = filename;
        QString strPath = filename;
        strPath = strPath.replace('/','\\');
        //qDebug()<<strPath;
        book->dynamicCall("ExportAsFixedFormat(int,const QString&)",0,strPath);
    }


}
///
/// \brief 保存excel
///
void ExcelBase::save()
{


    if(filename.isEmpty())
        return;
    saveAs(filename);


}

void ExcelBase::close()
{


    destory();

}

void ExcelBase::kick()
{



    if (excel != nullptr && !excel->isNull())
    {
        excel->setProperty("Visible", true);
    }
    TC_FREE(sheet );
    TC_FREE(sheets);
    TC_FREE(book  );
    TC_FREE(books );
    TC_FREE(excel );
    destory();

}

QStringList ExcelBase::sheetNames()
{
    QStringList ret;


    if (sheets != nullptr && !sheets->isNull())
    {
        int sheetCount = sheets->property("Count").toInt();
        for (int i = 1; i <= sheetCount; i++)
        {
            QAxObject* sheet = sheets->querySubObject("Item(int)", i);
            if(nullptr== sheet || sheet->isNull())
                continue;
            ret.append(sheet->property("Name").toString());
            delete sheet;
        }
    }

    return ret;
}

QString ExcelBase::currentSheetName()
{

    return sheetName;
}

void ExcelBase::setVisible(bool value)
{


    if (excel != nullptr && !excel->isNull())
    {
        excel->setProperty("Visible", value);
    }

}

void ExcelBase::setCaption(const QString& value)
{


    if (excel != nullptr && !excel->isNull())
    {
        excel->setProperty("Caption", value);
    }
}

bool ExcelBase::addBook()
{
    bool ret = false;


    if (excel != nullptr && !excel->isNull())
    {
        TC_FREE(sheet );
        TC_FREE(sheets);
        TC_FREE(book  );
        TC_FREE(books );
        books = excel->querySubObject("WorkBooks");
        ret = books != nullptr && !books->isNull();
    }

    return ret;
}

bool ExcelBase::currentSheet()
{
    bool ret = false;

    TC_FREE(sheet);
    if (excel != nullptr && !excel->isNull())
    {
        TC_FREE(sheet);
        sheet = excel->querySubObject("ActiveWorkBook");
        ret = sheet != nullptr && !sheet->isNull();
        sheetName = ret ? sheet->property("Name").toString() : "";
    }

    return ret;
}

bool ExcelBase::setCurrentSheet(int index)
{
    bool ret = false;


    if (sheets != nullptr && !sheets->isNull())
    {
        TC_FREE(sheet);
        sheet = sheets->querySubObject("Item(int)", index);
        ret = sheet != nullptr && ! sheet->isNull();
        if(ret)
        {
            sheet->dynamicCall("Activate(void)");
        }
        sheetName = ret ? sheet->property("Name").toString() : "";
    }

    return ret;
}

int ExcelBase::sheetCount()
{
    int ret = 0;


    if (sheets != nullptr && ! sheets->isNull())
    {
        ret = sheets->property("Count").toInt();
    }

    return ret;
}

void ExcelBase::cellFormat(int row, int col, const QString& format)
{


    if (sheet != nullptr && ! sheet->isNull())
    {
        QAxObject* range = sheet->querySubObject("Cells(int, int)", row, col);
        range->setProperty("NumberFormatLocal", format);
        delete range;
    }
}

void ExcelBase::cellAlign(int row, int col, Alignment hAlign, Alignment vAlign)
{


    if (sheet != nullptr && !sheet->isNull())
    {
        QAxObject* range = sheet->querySubObject("Cells(int, int)", row, col);
        range->setProperty("HorizontalAlignment", hAlign);
        range->setProperty("VerticalAlignment",   vAlign);
        delete range;
    }
}

QVariant ExcelBase::read(int row, int col)
{
    QVariant ret;


    if (sheet != nullptr && ! sheet->isNull())
    {
        QAxObject* range = sheet->querySubObject("Cells(int, int)", row, col);
        //ret = range->property("Value");
        ret = range->dynamicCall("Value()");
        delete range;
    }
    return ret;
}

void ExcelBase::write(int row, int col, const QVariant& value)
{


    if (sheet != nullptr && ! sheet->isNull())
    {
        QAxObject* range = sheet->querySubObject("Cells(int, int)", row, col);
        range->setProperty("Value", value);
        delete range;
    }
}

bool ExcelBase::usedRange(int& rowStart, int& colStart, int& rowEnd, int& colEnd)
{
    bool ret = false;


    if (sheet != nullptr && ! sheet->isNull())
    {
        QAxObject* urange  = sheet->querySubObject("UsedRange");
        rowStart = urange->property("Row"   ).toInt();
        colStart = urange->property("Column").toInt();
        rowEnd   = urange->querySubObject("Rows"   )->property("Count").toInt();
        colEnd   = urange->querySubObject("Columns")->property("Count").toInt();
        delete urange;
        ret = true;
    }

    return ret;
}
///
/// \brief 读取整个sheet
/// \return
///
QVariant ExcelBase::readAll(QString cellRange)
{
    QVariant var;


    if (sheet != nullptr && ! sheet->isNull())
    {
        QAxObject *usedRange = sheet->querySubObject("Range(const QString&)",cellRange);
        if(nullptr== usedRange || usedRange->isNull())
        {
            return var;
        }
        var = usedRange->dynamicCall("Value");
        delete usedRange;
    }

    return var;
}
///
/// \brief 读取整个sheet的数据，并放置到cells中
/// \param cells
///
void ExcelBase::readAll(QList<QList<QVariant> > &cells,  QString cellRange)
{

    castVariant2ListListVariant(readAll(cellRange),cells);
}

///
/// \brief 写入一个表格内容
/// \param cells
/// \return 成功写入返回true
/// \see readAllSheet
///
bool ExcelBase::writeCurrentSheet(const QList<QList<QVariant> > &cells)
{


    if(cells.size() <= 0)
        return false;
    if(nullptr== sheet || sheet->isNull())
        return false;
    int row = cells.size();
    int col = cells.at(0).size();
    QString rangStr;
    convertToColName(col,rangStr);
    rangStr += QString::number(row);
    rangStr = "A1:" + rangStr;


    QAxObject *range = sheet->querySubObject("Range(const QString&)",rangStr);
    if(nullptr== range || range->isNull())
    {
        return false;
    }
    bool succ = false;
    QVariant var;
    castListListVariant2Variant(cells,var);
    succ = range->setProperty("Value", var);
    delete range;
    return succ;
}
///
/// \brief 把列数转换为excel的字母列号
/// \param data 大于0的数
/// \return 字母列号，如1->A 26->Z 27 AA
///
void ExcelBase::convertToColName(int data, QString &res)
{
    Q_ASSERT(data>0 && data<65535);
    int tempData = data / 26;
    if(tempData > 0)
    {
        int mode = data % 26;
        convertToColName(mode,res);
        convertToColName(tempData,res);
    }
    else
    {
        res=(to26AlphabetString(data)+res);
    }
}
///
/// \brief 数字转换为26字母
///
/// 1->A 26->Z
/// \param data
/// \return
///
QString ExcelBase::to26AlphabetString(int data)
{
    QChar ch = data + 0x40;//A对应0x41
    return QString(ch);
}
///
/// \brief QList<QList<QVariant> >转换为QVariant
/// \param cells
/// \return
///
void ExcelBase::castListListVariant2Variant(const QList<QList<QVariant> > &cells, QVariant &res)
{
    QVariantList vars;
    const int rows = cells.size();
    for(int i=0;i<rows;++i)
    {
        vars.append(QVariant(cells[i]));
    }
    res = QVariant(vars);
}
///
/// \brief 把QVariant转为QList<QList<QVariant> >
/// \param var
/// \param res
///
void ExcelBase::castVariant2ListListVariant(const QVariant &var, QList<QList<QVariant> > &res)
{
    QVariantList varRows = var.toList();
    if(varRows.isEmpty())
    {
        return;
    }
    const int rowCount = varRows.size();
    QVariantList rowData;
    for(int i=0;i<rowCount;++i)
    {
        rowData = varRows[i].toList();
        //qDebug()<<rowData.size();
        res.push_back(rowData);
    }
}
void ExcelBase::setCellBkColor(int row, int col, int colorIndex)
{
    if (sheet != nullptr && ! sheet->isNull())
    {
        QAxObject* range = sheet->querySubObject("Cells(int, int)", row, col);
        QAxObject* interior = range->querySubObject("Interior");
        interior->setProperty("ColorIndex",colorIndex);
        delete range;
    }
}
void ExcelBase::hideOneRow(int row)
{
    if (sheet != nullptr && ! sheet->isNull())
    {
        QAxObject* range = sheet->querySubObject("Rows(int,int)", row);
        range->setProperty("Hidden",true);
        delete range;
    }
}

void ExcelBase::deleteOneRow(int row)
{
    if (sheet != nullptr && ! sheet->isNull())
    {
        QAxObject* oRow = sheet->querySubObject("Rows(int,int)", row);
        if(oRow)
            oRow->dynamicCall("Delete");
        else {
            QMessageBox::warning(nullptr,"","2");
        }
        delete oRow;
    }
}

void ExcelBase::paste(int row,int col)
{
    if (sheet != nullptr && ! sheet->isNull())
    {
        QAxObject* range = sheet->querySubObject("Cells(int, int)", row, col);
        sheet->dynamicCall("Paste(QVariant,QVariant)",range->asVariant());
        delete range;
    }
}
