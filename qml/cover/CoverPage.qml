import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {

    Image {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: Theme.paddingLarge
        width: 116  // 2 times the recommended pass icon size
        height: 116
        source: topIcon != "" ? topIcon : "image://theme/harbour-passviewer"
    }

    CoverActionList {
        enabled: topIcon != ""

        CoverAction {
            iconSource: "image://theme/icon-cover-play"
            onTriggered: {
                if (topPath !== "" && topData !== "") {
                    var properties = { path: topPath, jsondata: topData };
                    pageStack.pop(pageStack.find(function(page){return page.uid === "firstPage"}), PageStackAction.Immediate);
                    pageStack.push(Qt.resolvedUrl("../pages/ShowPass.qml"), properties, PageStackAction.Immediate);
                    pageStack.pushAttached(Qt.resolvedUrl("../pages/ShowBack.qml"), properties);
                    appWindow.activate();
                }
            }
        }
    }
}


