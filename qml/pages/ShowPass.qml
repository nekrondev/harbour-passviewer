import QtQuick 2.0
import Sailfish.Silica 1.0
import io.thp.pyotherside 1.3
import "../utils.js" as Utils

Page {
    id: page

    property string name: ""
    property string jsondata: ""
    property string path: ""
    property string relevantDate: ""
    property string labelColor: "black"
    property string foreColor: "black"
    property string barcodeType: "qr"
    property string barcodeEncoding: "iso-8859-1"
    property string barcodeContent: ""

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: background.height + Theme.paddingLarge * 2

        PullDownMenu {

            MenuItem {
                text: qsTr("Fullscreen Barcode")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("ShowCodeFullscreen.qml"), { barcodeContent: barcodeContent, barcodeEncoding: barcodeEncoding, barcodeType: barcodeType });
                }
            }

            MenuItem {
                text: qsTr("Simple View")
                onClicked: {
                    var properties = { path: path, jsondata: jsondata };
                    pageStack.push(Qt.resolvedUrl("ShowSimple.qml"), properties);
                }
            }

            MenuItem {
                text: qsTr("Create Calendar Entry")
                visible: relevantDate != ""
                onClicked: {
                    var now = new Date();
                    py.create_calendar_entry(name, relevantDate);
                }
            }
        }

        Rectangle {
            id: background
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: Theme.paddingLarge
            height: body.height + Theme.paddingMedium * 2
            color: 'white'

            Image {
                id: background_image
                anchors.top: parent.top
                anchors.left: parent.left
                width: background.width
                height: sourceSize.width != 0 ? sourceSize.height * background.width / sourceSize.width : 0
                source: "image://python" + path + "/background.png"
            }

            Column {
                id: body
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: Theme.paddingMedium
                spacing: Theme.paddingMedium

                Row {
                    spacing: Theme.paddingLarge

                    Image {
                        id: headerImage
                        source: "image://python" + path + "/logo.png"
                    }

                    Label {
                        id: logoText
                        text: ""
                        color: page.foreColor
                        font.pixelSize: Theme.fontSizeSmall
                        width: body.width - headerImage.width - headerRight.width - parent.spacing * 2
                    }

                    Row {
                        id: headerRight
                        spacing: Theme.paddingLarge

                        Repeater {
                            model: ListModel {
                                id: headerFields
                                ListElement { title: ""; value: "" }
                            }

                            Column {

                                Label {
                                    text: title
                                    color: page.labelColor
                                    font.pixelSize: Theme.fontSizeTiny
                                }

                                Label {
                                    text: value
                                    color: page.foreColor
                                    font.pixelSize: Theme.fontSizeTiny
                                }
                            }
                        }
                    }
                }

                Column {
                    id: boardingPrimary
                    visible: boardingFromTitle !== '' || boardingFromValue !== '' || boardingToTitle !== '' || boardingToValue !== ''

                    Row {
                        property real itemWidth: body.width / 2 - spacing

                        Column {
                            width: parent.itemWidth

                            Label {
                                id: boardingFromTitle
                                text: ""
                                color: page.labelColor
                                font.pixelSize: Theme.fontSizeExtraSmall
                            }

                            Label {
                                id: boardingFromValue
                                text: ""
                                color: page.foreColor
                                font.pixelSize: text.length <= 3 ? Theme.fontSizeHuge : Theme.fontSizeExtraLarge
                            }
                        }

                        Column {
                            width: parent.itemWidth

                            Label {
                                id: boardingToTitle
                                anchors.right: parent.right
                                text: ""
                                color: page.labelColor
                                font.pixelSize: Theme.fontSizeExtraSmall
                            }

                            Label {
                                id: boardingToValue
                                anchors.right: parent.right
                                text: ""
                                color: page.foreColor
                                font.pixelSize: text.length <= 3 ? Theme.fontSizeHuge : Theme.fontSizeExtraLarge
                            }
                        }
                    }
                }

                Column {
                    id: standardPrimary
                    visible: primaryTitle !== "" || primaryValue !== ""

                    Image {
                        source: "image://python" + path + "/strip.png"
                        width: body.width
                        height: sourceSize.width != 0 ? sourceSize.height * body.width / sourceSize.width : standardPrimaryRow.height

                        Row {
                            id: standardPrimaryRow
                            spacing: Theme.paddingLarge

                            Column {
                                width: body.width - thumbnailImage.width - parent.spacing

                                Label {
                                    id: primaryTitle
                                    text: ""
                                    color: page.labelColor
                                    font.pixelSize: Theme.fontSizeExtraSmall
                                }

                                Label {
                                    id: primaryValue
                                    width: parent.width
                                    text: ""
                                    color: page.foreColor
                                    font.pixelSize: Theme.fontSizeLarge
                                    wrapMode: Text.Wrap
                                }
                            }

                            Image {
                                id: thumbnailImage
                                source: "image://python" + path + "/thumbnail.png"
                            }
                        }
                    }
                }

                Flow {
                    width: body.width
                    spacing: Theme.paddingLarge

                    Repeater {
                        model: ListModel {
                            id: secondaryFields
                            ListElement { title: ""; value: "" }
                        }

                        Column  {

                            Label {
                                text: title
                                color: page.labelColor
                                font.pixelSize: Theme.fontSizeExtraSmall
                            }

                            Label {
                                text: value
                                color: page.foreColor
                                font.pixelSize: Theme.fontSizeSmall
                            }
                        }
                    }
                }

                Flow {
                    width: body.width
                    spacing: Theme.paddingLarge

                    Repeater {
                        model: ListModel {
                            id: tertiaryFields
                            ListElement { title: ""; value: "" }
                        }

                        Column  {

                            Label {
                                text: title
                                color: page.labelColor
                                font.pixelSize: Theme.fontSizeExtraSmall
                            }

                            Label {
                                text: value
                                color: page.foreColor
                                font.pixelSize: Theme.fontSizeSmall
                            }
                        }
                    }
                }

                Image {
                    source: "image://python" + path + "/footer.png"
                }

                Column {
                    width: parent.width

                    Rectangle {
                        visible: barcodeImage.width != 0
                        width: barcodeImage.width + 20;
                        height: barcodeImage.height + 20;
                        anchors.horizontalCenter: parent.horizontalCenter
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
                        id: barcodeAltText
                        text: ''
                        color: page.foreColor
                        font.pixelSize: Theme.fontSizeTiny
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }
            }
        }

        VerticalScrollDecorator {}
    }

    Python {
        id: py

        function format_date_time(isoDt, dateFormat, timeFormat, ignoreTimeZone) {
            return call_sync("dtformat.format_date_time", [isoDt, dateFormat, timeFormat, ignoreTimeZone]);
        }

        function create_calendar_entry(subject, isoDt) {
            call("ical.create_calendar_entry", [subject, isoDt], function(status) {
                if (status === "format")
                    notificator.errorNotification(qsTr("Format Error"), qsTr("Couldn't recognize date/time format"));
                if (status === "xdg-open")
                    notificator.errorNotification(qsTr("Unsupported"), qsTr("Please update your system or install calendar"));
            });
        }
    }

    Component.onCompleted: {
        function getFields(pass, style, fieldType, target) {
            var fields = [];
            if (fieldType in pass[style]) {
                for (var field = 0; field < pass[style][fieldType].length; field++) {
                    var data = pass[style][fieldType][field];
                    if ("dateStyle" in data && "timeStyle" in data) {
                        var dateFormat = data.dateStyle.substring(11).toLowerCase();
                        var timeFormat = data.timeStyle.substring(11).toLowerCase();
                        var ignoreTimeZone = false;
                        if ('ignoresTimeZone' in data)
                            ignoreTimeZone = data.ignoresTimeZone;
                        data.value = py.format_date_time(data.value, dateFormat, timeFormat, ignoreTimeZone);
                    }

                    target.append({ title: String(data.label), value: String(data.value) });
                }
            }
        }

        var pass = JSON.parse(jsondata);
        if ('relevantDate' in pass)
            relevantDate = pass.relevantDate;
        else
            relevantDate = '';
        if ('backgroundColor' in pass)
            background.color = Utils.interpretColor(pass.backgroundColor);
        if ('labelColor' in pass)
            page.labelColor = Utils.interpretColor(pass.labelColor);
        if ('foregroundColor' in pass)
            page.foreColor = Utils.interpretColor(pass.foregroundColor);
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
        headerFields.clear();
        getFields(pass, style, 'headerFields', headerFields)
        if (style === "boardingPass") {
            boardingFromTitle.text = pass.boardingPass.primaryFields[0].label;
            boardingFromValue.text = pass.boardingPass.primaryFields[0].value;
            boardingToTitle.text = pass.boardingPass.primaryFields[1].label;
            boardingToValue.text = pass.boardingPass.primaryFields[1].value;
            secondaryFields.clear();
            tertiaryFields.clear();
            getFields(pass, style, 'auxiliaryFields', secondaryFields);
            getFields(pass, style, 'secondaryFields', tertiaryFields);
        }
        else {
            primaryTitle.text = pass[style].primaryFields[0].label;
            primaryValue.text = pass[style].primaryFields[0].value;
            secondaryFields.clear();
            tertiaryFields.clear();
            getFields(pass, style, 'secondaryFields', secondaryFields);
            getFields(pass, style, 'auxiliaryFields', tertiaryFields);
        }
        if ('barcode' in pass) {
            if ('message' in pass.barcode)
                page.barcodeContent = pass.barcode.message;
            if ('messageEncoding' in pass.barcode)
                page.barcodeEncoding = pass.barcode.messageEncoding;
            if ('format' in pass.barcode)
                page.barcodeType = pass.barcode.format.substring(15).toLowerCase();
            if ('altText' in pass.barcode)
                barcodeAltText.text = pass.barcode.altText;
        }
    }
}
