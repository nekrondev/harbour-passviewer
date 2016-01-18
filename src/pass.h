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
    Q_PROPERTY(QString name MEMBER m_name NOTIFY nameChanged)
    Q_PROPERTY(QString path MEMBER m_path NOTIFY pathChanged)
    Q_PROPERTY(int points MEMBER m_points NOTIFY pointsChanged)
    Q_PROPERTY(QString jsondata READ jsondata WRITE setJsondata NOTIFY jsondataChanged)
    Q_PROPERTY(QString typeId MEMBER m_typeId NOTIFY typeIdChanged)
    Q_PROPERTY(bool updateable MEMBER m_updateable NOTIFY updateableChanged)

    bool isValid() { return m_isValid; }
    void setIsValid(bool isValid) { m_isValid = isValid; emit isValidChanged(m_isValid); }
    QString jsondata() { return m_jsondata; }
    void setJsondata(QString jsondata) { m_jsondata = jsondata; emit jsondataChanged(m_jsondata); }

signals:
    void isValidChanged(const bool);
    void nameChanged(const QString);
    void pathChanged(const QString);
    void pointsChanged(const int);
    void jsondataChanged(const QString);
    void typeIdChanged(const QString);
    void updateableChanged(const bool);

public slots:

private:
    bool m_isValid;
    QString m_name;
    QString m_path;
    int m_points;
    QString m_jsondata;
    QString m_typeId;
    bool m_updateable;
};

#endif // PASS_H
