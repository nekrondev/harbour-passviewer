#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <sailfishapp.h>
#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <unistd.h>

#include "settingsstore.h"
#include "barcodeimageprovider.h"
#include "zipfileimageprovider.h"
#include "homewatcher.h"
#include "notificator.h"
#include "datetimeformat.h"
#include "currencyformat.h"
#include "passhandler.h"
#include "passdb.h"


int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    QScopedPointer<QQuickView> view(SailfishApp::createView());

    // check if another instance is running
    QString piddir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    if (!QDir(piddir).exists())
        QDir().mkpath(piddir);
    QFile pidfile(piddir + "/pid");
    if (pidfile.open(QFile::ReadOnly)) {
        QString pid(pidfile.readLine());
        pidfile.close();
        if (!QDir(QString("/proc/") + pid).exists())
            pidfile.remove();
    }
    // yes: signal it
    if (pidfile.exists()) {
        if (argc == 2) {
            QString command("dbus-send --dest=org.harbour.passviewer --type=method_call /org/harbour/passviewer org.harbour.passviewer.openPass string:'");
            QString passFile(argv[1]);
            passFile.replace("'", "'\\''");
            command.append(passFile + "'");
            system(command.toUtf8());
        }
        return 0;
    }
    // no: start normally
    if (pidfile.open(QFile::WriteOnly)) {
        pidfile.write(QString::number(getpid()).toUtf8());
        pidfile.close();
    }

    SettingsStore settingsStore;
    view->rootContext()->setContextProperty("settingsStore", &settingsStore);

    view->engine()->addImageProvider("barcode", new BarcodeImageProvider());

    view->engine()->addImageProvider("zipimage", new ZipFileImageProvider());

    HomeWatcher homeWatcher;
    view->rootContext()->setContextProperty("homeWatcher", &homeWatcher);

    Notificator notificator;
    view->rootContext()->setContextProperty("notificator", &notificator);

    DateTimeFormat dateTimeFormat;
    view->rootContext()->setContextProperty("dateTimeFormat", &dateTimeFormat);

    CurrencyFormat currencyFormat;
    view->rootContext()->setContextProperty("currencyFormat", &currencyFormat);

    PassHandler passHandler;
    view->rootContext()->setContextProperty("passHandler", &passHandler);

    PassDB passDB;
    view->rootContext()->setContextProperty("passDB", &passDB);

    view->setSource(SailfishApp::pathTo("qml/harbour-passviewer.qml"));

    view->show();
    int retcode = app->exec();
    pidfile.remove();
    return retcode;
}

