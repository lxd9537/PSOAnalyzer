#include "qxtcsvmodel.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

QxtCsvModel::QxtCsvModel(QObject *parent) : QAbstractTableModel(parent)
{}
QxtCsvModel::~QxtCsvModel()
{}

int QxtCsvModel::rowCount(const QModelIndex& parent) const
{
    if (parent.row() != -1 && parent.column() != -1) return 0;
    return csvData.count();
}

int QxtCsvModel::columnCount(const QModelIndex& parent) const
{
    if (parent.row() != -1 && parent.column() != -1) return 0;
    return maxColumn;
}

QVariant QxtCsvModel::data(const QModelIndex& index, int role) const
{
    if(index.parent() != QModelIndex()) return QVariant();
    if(role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::UserRole)
    {
        if(index.row() < 0 || index.column() < 0 || index.row() >= rowCount())
            return QVariant();
        const QStringList& row = csvData[index.row()];
        if(index.column() >= row.length())
            return QVariant();
        return row[index.column()];
    }
    return QVariant();
}

QVariant QxtCsvModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(section < header.count() && orientation == Qt::Horizontal && (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::UserRole))
        return header[section];
    else
        return header[0];
}

void QxtCsvModel::setSource(const QString filename, bool withHeader, QChar separator, QTextCodec* codec)
{
    QFile src(filename);
    setSource(&src, withHeader, separator, codec);
}

void QxtCsvModel::setSource(QIODevice *file, bool withHeader, QChar separator, QTextCodec* codec)
{
    bool headerSet = !withHeader;
   if(!file->isOpen())
        file->open(QIODevice::ReadOnly);
    if(withHeader)
        maxColumn = 0;
    else
        maxColumn = header.size();
    csvData.clear();
    QStringList row;
    QString field;
    QChar quote;
    QChar ch, buffer(0);
    bool readCR = false;
    QTextStream stream(file);
    if(codec) {
        stream.setCodec(codec);
    } else {
        stream.setAutoDetectUnicode(true);
    }
    while(!stream.atEnd()) {
        if(buffer != QChar(0)) {
            ch = buffer; 
            buffer = QChar(0);
        } else {
            stream >> ch;
        }
        if(ch == '\n' && readCR) 
            continue;
        else if(ch == '\r')
            readCR = true;
        else
            readCR = false;
        if(ch != separator && (ch.category() == QChar::Separator_Line || ch.category() == QChar::Separator_Paragraph || ch.category() == QChar::Other_Control)) {
            row << field;
            field.clear();
            if(!row.isEmpty()) {
                if(!headerSet) {
                    header = row;
                    headerSet = true;
                } else {
                    csvData.append(row);
                }
                if(row.length() > maxColumn) {
                    maxColumn = row.length();
                }
            }
            row.clear();
        } else if((quoteMode & DoubleQuote && ch == '"') || (quoteMode & SingleQuote && ch == '\'')) {
            quote = ch;
            do {
                stream >> ch;
                if(ch == '\\' && quoteMode & BackslashEscape) {
                    stream >> ch;
                } else if(ch == quote) {
                    if(quoteMode & TwoQuoteEscape) {
                        stream >> buffer;
                        if(buffer == quote) {
                            buffer = QChar(0);
                            field.append(ch);
                            continue;
                        }
                    }
                    break;
                }
                field.append(ch);
            } while(!stream.atEnd());
        } else if(ch == separator) {
            row << field;
            field.clear();
        } else {
            field.append(ch);
        }
    }
    if(!field.isEmpty()) 
        row << field;
    if(!row.isEmpty()) {
        if(!headerSet)
            header = row;
        else
            csvData.append(row);
    }
    file->close();
}

/*!
  Sets the horizontal headers of the model to the values provided in \a data.
 */
void QxtCsvModel::setHeaderData(const QStringList& data)
{
    header = data;
    emit headerDataChanged(Qt::Horizontal, 0, data.count());
}


bool QxtCsvModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
    if(orientation != Qt::Horizontal) return false;
    if(role != Qt::DisplayRole && role != Qt::EditRole) return false;
    if(section < 0) return false;
    while(section > header.size()) {
        header << QString();
    }
    header[section] = value.toString();
    emit headerDataChanged(Qt::Horizontal, section, section);
    return true;
}

bool QxtCsvModel::setData(const QModelIndex& index, const QVariant& data, int role)
{
    if (index.parent() != QModelIndex()) return false;

    if(role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::UserRole) {
        if(index.row() >= rowCount() || index.column() >= columnCount() || index.row() < 0 || index.column() < 0) return false;
        QStringList& row = csvData[index.row()];
        while(row.length() <= index.column())
            row << QString();
        row[index.column()] = data.toString();
        emit dataChanged(index, index);
        return true;
    }
    return false;
}


bool QxtCsvModel::insertRow(int row, const QModelIndex& parent)
{
    return insertRows(row, 1, parent);
}

bool QxtCsvModel::insertRows(int row, int count, const QModelIndex& parent)
{
    if (parent != QModelIndex() || row < 0) return false;
    emit beginInsertRows(parent, row, row + count);

    if(row >= rowCount()) {
        for(int i = 0; i < count; i++) csvData << QStringList();
    } else {
        for(int i = 0; i < count; i++) csvData.insert(row, QStringList());
    }
    emit endInsertRows();
    return true;
}

bool QxtCsvModel::removeRow(int row, const QModelIndex& parent)
{
    return removeRows(row, 1, parent);
}


bool QxtCsvModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (parent != QModelIndex() || row < 0) return false;
    if (row >= rowCount()) return false;
    if (row + count >= rowCount()) count = rowCount() - row;
    emit beginRemoveRows(parent, row, row + count);

    for (int i = 0;i < count;i++)
        csvData.removeAt(row);
    emit endRemoveRows();
    return true;
}


bool QxtCsvModel::insertColumn(int col, const QModelIndex& parent)
{
    return insertColumns(col, 1, parent);
}


bool QxtCsvModel::insertColumns(int col, int count, const QModelIndex& parent)
{
    if (parent != QModelIndex() || col < 0) return false;
    beginInsertColumns(parent, col, col + count - 1);

    for(int i = 0; i < rowCount(); i++) {
        QStringList& row = csvData[i];
        while(col >= row.length()) row.append(QString());
        for(int j = 0; j < count; j++) {
            row.insert(col, QString());
        }
    }
    for(int i = 0; i < count ;i++)
        header.insert(col, QString());
    maxColumn += count;
    endInsertColumns();
    return true;
}


bool QxtCsvModel::removeColumn(int col, const QModelIndex& parent)
{
    return removeColumns(col, 1, parent);
}


bool QxtCsvModel::removeColumns(int col, int count, const QModelIndex& parent)
{
    if (parent != QModelIndex() || col < 0) return false;
    if (col >= columnCount()) return false;
    if (col + count >= columnCount()) count = columnCount() - col;
    emit beginRemoveColumns(parent, col, col + count);

    QString before, after;
    for(int i = 0; i < rowCount(); i++) {
        for(int j = 0; j < count; j++) {
            csvData[i].removeAt(col);
        }
    }
    for(int i = 0; i < count; i++)
        header.removeAt(col);
    emit endRemoveColumns();
    return true;
}

static QString qxt_addCsvQuotes(QxtCsvModel::QuoteMode mode, QString field)
{
    bool addDoubleQuotes = ((mode & QxtCsvModel::DoubleQuote) && field.contains('"'));
    bool addSingleQuotes = ((mode & QxtCsvModel::SingleQuote) && field.contains('\''));

    bool quoteField = (mode & QxtCsvModel::AlwaysQuoteOutput) || addDoubleQuotes || addSingleQuotes;
    if(quoteField && !addDoubleQuotes && !addSingleQuotes) {
        if(mode & QxtCsvModel::DoubleQuote)
            addDoubleQuotes = true;
        else if(mode & QxtCsvModel::SingleQuote)
            addSingleQuotes = true;
    } 
    if(mode & QxtCsvModel::BackslashEscape) {
        if(addDoubleQuotes) 
            return '"' + field.replace("\\", "\\\\").replace("\"", "\\\"") + '"';
        if(addSingleQuotes)
            return '\'' + field.replace("\\", "\\\\").replace("'", "\\'") + '\'';
    } else {
        if(addDoubleQuotes) 
            return '"' + field.replace("\"", "\"\"") + '"';
        if(addSingleQuotes)
            return '\'' + field.replace("'", "''") + '\'';
    }
    return field;
}

/*!
  Outputs the content of the model as a CSV file to the device \a dest using \a codec.

  Fields in the output file will be separated by \a separator. Set \a withHeader to true
  to output a row of headers at the top of the file.
 */ 
void QxtCsvModel::toCSV(QIODevice* dest, bool withHeader, QChar separator, QTextCodec* codec) const
{

    int row, col, rows, cols;
    QString data;

    rows = rowCount();
    cols = columnCount();

    if(!dest->isOpen())
        dest->open(QIODevice::WriteOnly | QIODevice::Truncate);

    QTextStream stream(dest);

    if(codec) stream.setCodec(codec);

    if(withHeader)
    {
        data = "";
        for(col = 0; col < cols; ++col) {
            if(col > 0) data += separator;
            data += qxt_addCsvQuotes(quoteMode, header.at(col)); //at(int index) returns the item at index position
        }
        data += '\r';
        data += '\n';
        stream << data;
    }
    for(row = 0; row < rows; ++row)
    {
        const QStringList& rowData = csvData[row];
        data = "";
        for(col = 0; col < cols; ++col) {
            if(col > 0) data += separator;
            if(col < rowData.length())
                data += qxt_addCsvQuotes(quoteMode, rowData.at(col));
            else
                data += qxt_addCsvQuotes(quoteMode, QString());
        }
        data += '\r';
        data += '\n';
        stream << data;
    }
    stream << flush;
    dest->close();
}

void QxtCsvModel::toCSV(const QString filename, bool withHeader, QChar separator, QTextCodec* codec) const
{
    QFile dest(filename);
    toCSV(&dest, withHeader, separator, codec);
}

Qt::ItemFlags QxtCsvModel::flags(const QModelIndex& index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

QxtCsvModel::QuoteMode QxtCsvModel::GetQuoteMode() const
{
    return quoteMode;
}

void QxtCsvModel::setQuoteMode(QuoteMode mode)
{
    quoteMode = mode;
}

void QxtCsvModel::setText(int row, int column, const QString& value)
{
    setData(index(row, column), value);
}

QString QxtCsvModel::text(int row, int column) const
{
    return data(index(row, column)).toString();
}

void QxtCsvModel::setHeaderText(int column, const QString& value)
{
    setHeaderData(column, Qt::Horizontal, value);
}

QString QxtCsvModel::headerText(int column) const
{
    return headerData(column, Qt::Horizontal).toString();
}
