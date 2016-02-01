import QtQuick 2.0
import Sailfish.Silica 1.0
import "pages"

ApplicationWindow
{
    id: appWindow
    property string topIcon: ""
    property string topName: ""
    property string topPath: ""
    property string topData: ""
    property bool topUpdateable: false
    initialPage: Component { FirstPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
}


