#ifndef RIP_H
#define RIP_H

#include <QObject>
#include <QHash>

class rip : public QObject
{
    Q_OBJECT
public:
    explicit rip(QObject *parent = nullptr);
    static rip* decode(const QString &line);
    static QString incode(const rip &packet);
    QHash<int, int> table;

signals:
};

#endif // RIP_H
