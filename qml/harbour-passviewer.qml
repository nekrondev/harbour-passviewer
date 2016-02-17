import QtQuick 2.0
import Sailfish.Silica 1.0
import "pages"

ApplicationWindow
{
    id: appWindow
    signal openPass(string origin)
    property string topIcon: ""
    property string topPath: ""
    initialPage: Component { FirstPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")

    function passClicked(origin) {
        openPass(origin);
    }
}


