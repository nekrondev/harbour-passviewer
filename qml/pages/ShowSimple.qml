import QtQuick 2.0
import Sailfish.Silica 1.0
import "../utils.js" as Utils

Page {
    id: page

    property string jsondata: ''
    property string path: ''
    property string barcodeType: "qr"
    property string barcodeEncoding: "iso-8859-1"
    property string barcodeContent: ""
    property string barcodeAltText: ""

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: body.height + Theme.paddingMedium * 2

        PullDownMenu {

            MenuItem {
                text: qsTr("Fullscreen Barcode")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("ShowCodeFullscreen.qml"), { barcodeContent: barcodeContent, barcodeEncoding: barcodeEncoding, barcodeType: barcodeType });
                }
            }
        }

        Column {
            id: body
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: Theme.paddingLarge
            spacing: Theme.paddingMedium

            Image {
                source: "image://python" + path + "/logo.png"
            }

            Label {
                id: logoText
                text: ''
                color: Theme.highlightColor
            }

            Repeater {
                model: ListModel {
                    id: frontFields
                    ListElement { title: ''; value: '' }
                }

                Column {

                    Label {
                        text: title
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.highlightColor
                        x: Theme.paddingLarge
                    }

                    Label {
                        text: value
                        color: Theme.highlightColor
                    }
                }
            }

            Image {
                source: "image://python" + path + "/footer.png"
            }

            Rectangle {
                visible: barcodeImage.width != 0
                width: barcodeImage.width + 20
                height: barcodeImage.height + 20
                color: 'white'

                Image {
                    id: barcodeImage
                    anchors.centerIn: parent
                    width: sourceSize.width !== 0 ? Utils.barcodeSize(sourceSize.width, sourceSize.height, body.width)[0] : 0
                    height: sourceSize.height !== 0 ? Utils.barcodeSize(sourceSize.width, sourceSize.height, body.width)[1] : 0
                    smooth: false
                    fillMode: Image.PreserveAspectFit
                    source: "image://barcode/" + barcodeType + "/" + barcodeEncoding + "/" + barcodeContent;
                }
            }

            Label {
                text: barcodeAltText
                color: Theme.highlightColor
            }

            Repeater {
                model: ListModel {
                    id: backFields
                    ListElement { title: ''; value: '' }
                }

                Column {

                    Label {
                        text: title
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.highlightColor
                        x: Theme.paddingLarge
                    }

                    Label {
                        text: value
                        font.pixelSize: Theme.fontSizeExtraSmall
                        color: Theme.highlightColor
                        width: body.width
                        wrapMode: Text.Wrap
                    }
                }
            }
        }

        VerticalScrollDecorator {}
    }

    Component.onCompleted: {
        function getFields(pass, style, fieldType, target) {
            var fields = [];
            if (fieldType in pass[style]) {
                for (var field = 0; field < pass[style][fieldType].length; field++) {
                    var data = pass[style][fieldType][field];
                    target.append({ title: String(data.label), value: String(data.value) });
                }
            }
        }

        var pass = JSON.parse(jsondata);
        if ('logoText' in pass)
            logoText.text = pass.logoText
        var styles = ["boardingPass", "coupon", "eventTicket", "storeCard", "generic"];
        var style = '';
        for (var key = 0; key < styles.length; key++) {
            if (styles[key] in pass) {
                style = styles[key];
                break;
            }
        }
        frontFields.clear();
        getFields(pass, style, 'headerFields', frontFields);
        getFields(pass, style, 'primaryFields', frontFields);
        getFields(pass, style, 'secondaryFields', frontFields);
        getFields(pass, style, 'auxiliaryFields', frontFields);
        if ('barcode' in pass) {
            if ('message' in pass.barcode)
                page.barcodeContent = pass.barcode.message;
            if ('messageEncoding' in pass.barcode)
                page.barcodeEncoding = pass.barcode.messageEncoding;
            if ('format' in pass.barcode)
                page.barcodeType = pass.barcode.format.substring(15).toLowerCase();
            if ('altText' in pass.barcode)
                page.barcodeAltText = pass.barcode.altText;
        }
        backFields.clear();
        getFields(pass, style, 'backFields', backFields);
    }
}
