#ifndef QXTCSVMODEL_H
#define QXTCSVMODEL_H

#include <QAbstractTableModel>
#include <QVariant>
#include <QIODevice>
#include <QChar>
#include <QString>
#include <QStringList>
#include <QModelIndex>
#include <QFile>
class QTextCodec;

class QxtCsvModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    QxtCsvModel(QObject *parent = nullptr);
    ~QxtCsvModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant& data, int role = Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role = Qt::DisplayRole);
    void setHeaderData(const QStringList& data);
    QString text(int row, int column) const;
    void setText(int row, int column, const QString& value);
    QString headerText(int column) const;
    void setHeaderText(int column, const QString& value);

    bool insertRow(int row, const QModelIndex& parent = QModelIndex());
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
    bool removeRow(int row, const QModelIndex& parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
    bool insertColumn(int col, const QModelIndex& parent = QModelIndex());
    bool insertColumns(int col, int count, const QModelIndex& parent = QModelIndex());
    bool removeColumn(int col, const QModelIndex& parent = QModelIndex());
    bool removeColumns(int col, int count, const QModelIndex& parent = QModelIndex());

    void setSource(QIODevice *file, bool withHeader = false, QChar separator = ',', QStringConverter::Encoding *codec = nullptr);
    void setSource(const QString filename, bool withHeader = false, QChar separator = ',', QStringConverter::Encoding *codec = nullptr);

    void toCSV(QIODevice *file, bool withHeader = false, QChar separator = ',', QStringConverter::Encoding *codec = nullptr) const;
    void toCSV(const QString filename, bool withHeader = false, QChar separator = ',', QStringConverter::Encoding *codec = nullptr) const;

    enum QuoteOption { NoQuotes = 0, SingleQuote = 1, DoubleQuote = 2, BothQuotes = 3,
                       NoEscape = 0, TwoQuoteEscape = 4, BackslashEscape = 8, 
                       AlwaysQuoteOutput = 16, DefaultQuoteMode = BothQuotes | BackslashEscape | AlwaysQuoteOutput };
    Q_DECLARE_FLAGS(QuoteMode, QuoteOption)

    QuoteMode GetQuoteMode() const;
    void setQuoteMode(QuoteMode mode);

    Qt::ItemFlags flags(const QModelIndex& index) const;

private:
    QList<QStringList> csvData;
    QStringList header;
    int maxColumn;
    QuoteMode quoteMode;
};

#endif
