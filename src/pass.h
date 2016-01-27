#ifndef PASS_H
#define PASS_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QStringList>
#include <QFileInfo>

class Pass : public QObject
{
    Q_OBJECT
public:
    explicit Pass(QObject *parent = 0);
    explicit Pass(QString path, QString data);

    Q_PROPERTY(bool isValid READ isValid WRITE setIsValid NOTIFY isValidChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(int points READ points WRITE setPoints NOTIFY pointsChanged)
    Q_PROPERTY(QString jsondata READ jsondata WRITE setJsondata NOTIFY jsondataChanged)
    Q_PROPERTY(QString typeId READ typeId WRITE setTypeId NOTIFY typeIdChanged)
    Q_PROPERTY(bool updateable READ updateable WRITE setUpdateable NOTIFY updateableChanged)

    bool isValid() { return m_isValid; }
    void setIsValid(bool isValid) { m_isValid = isValid; emit isValidChanged(m_isValid); }
    QString name() { return m_name; }
    void setName(QString name) { m_name = name; emit nameChanged(m_name); }
    QString filename() { return m_filename; }
    void setFilename(QString filename) { m_filename = filename; emit filenameChanged(m_filename); }
    QString path() { return m_path; }
    void setPath(QString path) { m_path = path; emit pathChanged(m_path); }
    int points() { return m_points; }
    void setPoints(int points) { m_points = points; emit pointsChanged(points); }
    QString jsondata() { return m_jsondata; }
    void setJsondata(QString jsondata) { m_jsondata = jsondata; emit jsondataChanged(m_jsondata); }
    QString typeId() { return m_typeId; }
    void setTypeId(QString typeId) { m_typeId = typeId; emit typeIdChanged(m_typeId); }
    bool updateable() { return m_updateable; }
    void setUpdateable(bool updateable) { m_updateable = updateable; emit updateableChanged(m_updateable); }

signals:
    void isValidChanged(const bool);
    void nameChanged(const QString);
    void filenameChanged(const QString);
    void pathChanged(const QString);
    void pointsChanged(const int);
    void jsondataChanged(const QString);
    void typeIdChanged(const QString);
    void updateableChanged(const bool);

public slots:

private:
    bool m_isValid;
    QString m_name;
    QString m_filename;
    QString m_path;
    int m_points;
    QString m_jsondata;
    QString m_typeId;
    bool m_updateable;
};

#endif // PASS_H
