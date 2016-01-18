import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page
    //allowedOrientations: Orientation.All

    property string name: ''
    property string jsondata: ''
    property string path: ''
    property bool updateable: false

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: pass.item.height + Theme.paddingLarge * 2

        PullDownMenu {

            MenuItem {
                text: qsTr("Fullscreen Barcode")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("ShowCodeFullscreen.qml"), { barcodeContent: pass.item.barcodeContent, barcodeEncoding: pass.item.barcodeEncoding, barcodeType: pass.item.barcodeType });
                }
            }

            MenuItem {
                text: qsTr("Simple View")
                onClicked: {
                    var properties = { path: pass.item.path, jsondata: pass.item.jsondata };
                    pageStack.push(Qt.resolvedUrl("ShowSimple.qml"), properties);
                }
            }

            MenuItem {
                text: qsTr("Create Calendar Entry")
                enabled: pass.item.relevantDate !== ""
                onClicked: {
                    passHandler.createCalendarEntry(name, pass.item.relevantDate);
                }
            }

            MenuItem {
                text: qsTr("Update")
                enabled: updateable
                onClicked: {
                    passHandler.updatePass(page.path);
                }
            }
        }

        Loader {
            id: pass
            width: 540 - Theme.paddingLarge * 2
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: Theme.paddingLarge
            source: Qt.resolvedUrl("../lib/Pass.qml")
            onLoaded: {
                item.path = path
                item.jsondata = jsondata
            }
        }

        VerticalScrollDecorator {}
    }

    Connections {
        target: passHandler
        onCalendarEntryFinished: {
            if (state === "format")
                notificator.errorNotification(qsTr("Format Error"), qsTr("Couldn't recognize date/time format"));
            if (state === "xdg-open")
                notificator.errorNotification(qsTr("Unsupported"), qsTr("Please update your system or install calendar"));
        }
    }
}
