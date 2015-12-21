import QtQuick 2.0
import Sailfish.Silica 1.0
import "../lib/utils.js" as Utils

Page {
    id: page
    //allowedOrientations: Orientation.All

    property string jsondata: ''
    property string path: ''
    property string barcodeType: "qr"
    property string barcodeEncoding: "iso-8859-1"
    property string barcodeContent: ""
    property string barcodeAltText: ""

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: body.height + Theme.paddingLarge * 2

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

                MouseArea {
                    anchors.fill: parent
                    enabled: settingsStore.barcodeTap
                    onClicked: pageStack.push(Qt.resolvedUrl("ShowCodeFullscreen.qml"), { barcodeContent: barcodeContent, barcodeEncoding: barcodeEncoding, barcodeType: barcodeType })
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
        Utils.checkFields(pass, style);
        frontFields.clear();
        getFields(pass, style, 'headerFields', frontFields);
        getFields(pass, style, 'primaryFields', frontFields);
        getFields(pass, style, 'secondaryFields', frontFields);
        getFields(pass, style, 'auxiliaryFields', frontFields);
        if (!('barcodes' in pass)) {
            pass.barcodes = [];
            if ('barcode' in pass) {
                pass.barcodes.push(pass.barcode);
            }
        }
        var validCode = false;
        for (var barcode = 0; barcode <= pass.barcodes.length; barcode++) {
            switch(pass.barcodes[barcode].format.substring(15).toLowerCase()) {
            case 'code128':
            case 'qr':
            case 'aztec':
            case 'pdf417':
                barcodeContent = 'message' in pass.barcodes[barcode] ? Qt.btoa(pass.barcodes[barcode].message) : '';
                barcodeEncoding = 'messageEncoding' in pass.barcodes[barcode] ?  pass.barcodes[barcode].messageEncoding: 'iso-8859-1';
                barcodeType = pass.barcodes[barcode].format.substring(15).toLowerCase();
                barcodeAltText = 'altText' in pass.barcodes[barcode] ? pass.barcodes[barcode].altText : '';
                validCode = true;
                break;
            }
            if (validCode)
                break;
        }
        backFields.clear();
        getFields(pass, style, 'backFields', backFields);
    }
}
