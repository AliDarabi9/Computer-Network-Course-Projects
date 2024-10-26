#ifndef LSP_H
#define LSP_H

#include <QObject>
#include <QHash>

class lsp : public QObject
{
    Q_OBJECT
public:
    explicit lsp(QObject *parent = nullptr);
    static lsp* decode(const QString &line);
    static QString incode(const lsp &packet);
    QHash<int, int> table;
    QVector<int> visitedNodeIDs;

signals:
};

#endif // LSP_H
