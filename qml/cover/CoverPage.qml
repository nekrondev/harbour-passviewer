import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {

    Image {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: Theme.paddingLarge
        width: Theme.iconSizeExtraLarge
        height: width
        source: topIcon != "" ? topIcon : "image://theme/harbour-passviewer"
    }

    CoverActionList {
        enabled: topIcon != ""

        CoverAction {
            iconSource: "image://theme/icon-cover-play"
            onTriggered: {
                if (topPath !== "") {
                    passClicked(topPath);
                }
            }
        }
    }
}


