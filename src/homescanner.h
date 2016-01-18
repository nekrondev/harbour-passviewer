#ifndef HOMESCANNER_H
#define HOMESCANNER_H

#include <QObject>
#include <QQmlEngine>
#include <QList>
#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>

#include "zipfile.h"
#include "pass.h"

class HomeScanner : public QObject
{
    Q_OBJECT
public:
    explicit HomeScanner(QObject *parent = 0);

signals:
    void passesFound(QList<QObject*> list, QStringList paths, bool update);

public slots:
    void scanHome(bool update = false);
};

#endif // HOMESCANNER_H
