#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <sailfishapp.h>
#include <QGuiApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QDBusInterface>
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
    bool alreadyRunning = false;
    QStringList procs(QDir("/proc").entryList(QDir::Dirs | QDir::NoDotAndDotDot));
    for (auto proc = procs.cbegin(); proc != procs.cend(); ++proc) {
        bool isProc = false;
        int pid = proc->toInt(&isProc);  // only numerical subdirs are processes
        if (isProc && pid == getpid())   // don't check your own PID
            isProc = false;
        if (isProc) {
            QFile cmdline(QString("/proc/") + (*proc) + "/cmdline");
            if (cmdline.open(QFile::ReadOnly)) {
                if (QString(cmdline.readLine()).contains("harbour-passviewer"))
                    alreadyRunning = true;
                cmdline.close();
            }
        }
        if (alreadyRunning)
            break;
    }
    if (alreadyRunning) {
        // yes, it's running, so signal it and exit
        QDBusInterface other("ch.p2501.harbour_passviewer", "/ch/p2501/harbour_passviewer", "ch.p2501.harbour_passviewer");
        QString origin;
        if (argc == 2)
            origin = argv[1];
        other.call("openPass", origin);
        return 0;
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
    return app->exec();
}

