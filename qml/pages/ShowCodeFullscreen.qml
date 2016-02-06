import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page
    allowedOrientations: Orientation.All

    property string barcodeType: "qr"
    property string barcodeEncoding: "iso-8859-1"
    property string barcodeContent: ""

    Rectangle {
        anchors.fill: parent
        anchors.leftMargin: Theme.horizontalPageMargin
        anchors.rightMargin: Theme.horizontalPageMargin
        anchors.topMargin: Theme.paddingLarge
        anchors.bottomMargin: Theme.paddingLarge
        color: 'white'

        Image {
            anchors.fill: parent
            anchors.margins: Theme.paddingLarge
            smooth: false
            fillMode: Image.PreserveAspectFit
            source: "image://barcode/" + barcodeType + "/" + barcodeEncoding + "/" + barcodeContent;
        }
    }
}
