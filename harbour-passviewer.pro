# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = harbour-passviewer

CONFIG += sailfishapp

SOURCES += src/harbour-passviewer.cpp \
    src/zint/qr.c \
    src/zint/common.c \
    src/zint/reedsol.c \
    src/zint/aztec.c \
    src/zint/pdf417.c \
    src/zint/large.c \
    src/zint/library.c \
    src/zint/bmp.c \
    src/barcodeimageprovider.cpp \
    src/homewatcher.cpp \
    src/settingsstore.cpp

OTHER_FILES += qml/harbour-passviewer.qml \
    qml/cover/CoverPage.qml \
    qml/pages/FirstPage.qml \
    rpm/harbour-passviewer.spec \
    rpm/harbour-passviewer.yaml \
    translations/*.ts \
    harbour-passviewer.desktop \
    qml/zipreader.py \
    qml/pages/ShowPass.qml \
    qml/pages/ShowBack.qml \
    qml/pages/ShowSimple.qml \
    qml/utils.js \
    qml/pages/ShowCodeFullscreen.qml \
    qml/pages/Copyright.qml \
    qml/dtformat.py \
    qml/pages/Settings.qml \
    rpm/harbour-passviewer.changes

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += translations/harbour-passviewer-de.ts

HEADERS += \
    src/zint/qr.h \
    src/zint/common.h \
    src/zint/sjis.h \
    src/zint/reedsol.h \
    src/zint/aztec.h \
    src/zint/pdf417.h \
    src/zint/large.h \
    src/zint/zint.h \
    src/zint/maxipng.h \
    src/zint/font.h \
    src/barcodeimageprovider.h \
    src/homewatcher.h \
    src/settingsstore.h

QT += positioning
