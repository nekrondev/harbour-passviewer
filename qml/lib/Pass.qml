import QtQuick 2.0
import QtGraphicalEffects 1.0
import Sailfish.Silica 1.0
import io.thp.pyotherside 1.3
import 'utils.js' as Utils

Rectangle {
    id: root
    property string jsondata: ''
    property string path: ''
    property string relevantDate: ''
    property color backgroundColor: 'white'
    property color labelColor: 'black'
    property color textColor: 'black'
    property string logoText: ''
    property alias headerFields: headerFieldsModel
    property string boardingFromKey: ''
    property string boardingFromTitle: ''
    property string boardingFromValue: ''
    property string boardingToKey: ''
    property string boardingToTitle: ''
    property string boardingToValue: ''
    property string primaryKey: ''
    property string primaryTitle: ''
    property string primaryValue: ''
    property alias secondaryFields: secondaryFieldsModel
    property alias tertiaryFields: tertiaryFieldsModel
    property string barcodeType: ''
    property string barcodeEncoding: ''
    property string barcodeContent: ''
    property string barcodeAltText: ''

    height: body.height + Theme.paddingMedium * 2

    Rectangle {
        id: background
        anchors.fill: parent
        color: backgroundColor
    }

    Image {
        id: background_image
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        source: "image://python" + path + "/background.png"
    }

    FastBlur {
        anchors.fill: background_image
        source: background_image
        radius: 32
    }

    Column {
        id: body
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: Theme.paddingMedium
        spacing: Theme.paddingMedium

        Row {
            id: headerRow
            spacing: Theme.paddingLarge

            Image {
                id: logo
                width: sourceSize.width * 36 / sourceSize.height
                height: 36
                fillMode: Image.PreserveAspectFit
                source: "image://python" + path + "/logo.png"
            }

            Label {
                text: logoText
                color: textColor
                font.pixelSize: Theme.fontSizeSmall
                width: body.width - logo.width - headerFieldsRow.width - parent.spacing * 2
            }

            Row {
                id: headerFieldsRow
                spacing: Theme.paddingLarge

                Repeater {
                    model: ListModel {
                        id: headerFieldsModel
                        ListElement { field: ""; title: ""; value: "" }
                    }

                    Column {

                        Label {
                            text: title
                            color: labelColor
                            font.pixelSize: Theme.fontSizeTiny
                        }

                        Label {
                            text: value
                            color: textColor
                            font.pixelSize: Theme.fontSizeTiny
                        }
                    }
                }
            }
        }

        Row {
            id: boardingPrimary
            visible: boardingFromTitle != '' || boardingFromValue != '' || boardingToTitle != '' || boardingToValue != ''

            Column {
                width: body.width / 2 - parent.spacing

                Label {
                    text: boardingFromTitle
                    color: labelColor
                    font.pixelSize: Theme.fontSizeExtraSmall
                }

                Label {
                    text: boardingFromValue
                    color: textColor
                    font.pixelSize: text.length <= 3 ? Theme.fontSizeHuge : Theme.fontSizeExtraLarge
                }
            }

            Column {
                width: body.width / 2 - parent.spacing

                Label {
                    anchors.right: parent.right
                    text: boardingToTitle
                    color: labelColor
                    font.pixelSize: Theme.fontSizeExtraSmall
                }

                Label {
                    anchors.right: parent.right
                    text: boardingToValue
                    color: textColor
                    font.pixelSize: text.length <= 3 ? Theme.fontSizeHuge : Theme.fontSizeExtraLarge
                }
            }
        }

        Image {
            id: standardPrimary
            visible: primaryTitle != '' || primaryValue != ''
            source: "image://python" + path + "/strip.png"
            width: body.width
            height: sourceSize.width != 0 ? sourceSize.height * body.width / sourceSize.width : standardPrimaryRow.height

            Row {
                id: standardPrimaryRow
                spacing: Theme.paddingLarge

                Column {
                    width: body.width - thumbnailImage.width - parent.spacing

                    Label {
                        text: primaryTitle
                        color: labelColor
                        font.pixelSize: Theme.fontSizeExtraSmall
                    }

                    Label {
                        width: parent.width
                        text: primaryValue
                        color: textColor
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

        Flow {
            width: body.width
            spacing: Theme.paddingLarge

            Repeater {
                model: ListModel {
                    id: secondaryFieldsModel
                    ListElement { field: ""; title: ""; value: "" }
                }

                Column {

                    Label {
                        text: title
                        color: labelColor
                        font.pixelSize: Theme.fontSizeExtraSmall
                    }

                    Label {
                        text: value
                        color: textColor
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
                    id: tertiaryFieldsModel
                    ListElement { field: ""; title: ""; value: "" }
                }

                Column {

                    Label {
                        text: title
                        color: labelColor
                        font.pixelSize: Theme.fontSizeExtraSmall
                    }

                    Label {
                        text: value
                        color: textColor
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
            visible: barcodeType != '' && barcodeEncoding != '' && barcodeContent != ''

            Rectangle {
                width: barcodeImage.width + 20
                height: barcodeImage.height + 20
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
                    onClicked: pageStack.push(Qt.resolvedUrl("../pages/ShowCodeFullscreen.qml"), { barcodeContent: barcodeContent, barcodeEncoding: barcodeEncoding, barcodeType: barcodeType })
                }
            }

            Label {
                text: barcodeAltText
                color: textColor
                font.pixelSize: Theme.fontSizeTiny
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    Python {
        id: py

        function format_date_time(isoDt, dateFormat, timeFormat, ignoreTimeZone) {
            return call_sync("dtformat.format_date_time", [isoDt, dateFormat, timeFormat, ignoreTimeZone]);
        }

        function format_currency(value, currency) {
            return call_sync("curformat.format_currency", [value, currency]);
        }

        function get_pass_changes(pass) {
            call("updater.get_pass_changes", [pass], function(changes) {
                function escape(text) {
                    return text.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
                }

                function update_marks(model, changes) {
                    for (var field = 0; field < model.count; field++) {
                        var key = model.get(field).field;
                        var found = false;
                        for (var change = 0; change < changes.length; change++) {
                            if (changes[change] === key) {
                                found = true;
                                break;
                            }
                        }
                        if (found) {
                            if (model.get(field).value.substring(0,3) !== '<u>') {
                                model.get(field).value = '<u>' + escape(model.get(field).value) + '</u>';
                            }
                        }
                    }
                }

                function update_primary_marks(changes) {
                    for (var priChange = 0; priChange < changes.length; priChange++) {
                        switch (changes[priChange]) {
                        case boardingFromKey:
                            if (boardingFromValue.substring(0,3) !== '<u>')
                                boardingFromValue = '<u>' + escape(boardingFromValue) + '</u>';
                            break;
                        case boardingToKey:
                            if (boardingToValue.substring(0,3) !== '<u>')
                                boardingToValue = '<u>' + escape(boardingToValue) + '</u>';
                            break;
                        case primaryKey:
                            if (primaryValue.substring(0,3) !== '<u>')
                                primaryValue = '<u>' + escape(primaryValue) + '</u>';
                        }
                    }
                }

                update_marks(headerFields, changes);
                update_marks(secondaryFields, changes);
                update_marks(tertiaryFields, changes);
                update_primary_marks(changes);
            });
        }
    }

    onJsondataChanged: {
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
                    else if ("currencyCode" in data) {
                        var currencyCode = data.currencyCode.substring(0,3).toUpperCase();
                        data.value = py.format_currency(data.value, currencyCode);
                    }

                    target.append({ field: String(data.key), title: String(data.label), value: String(data.value) });
                }
            }
        }

        var pass = JSON.parse(jsondata);
        if ('relevantDate' in pass)
            relevantDate = pass.relevantDate;
        else
            relevantDate = '';
        if ('backgroundColor' in pass)
            backgroundColor = Utils.interpretColor(pass.backgroundColor);
        if ('labelColor' in pass)
            labelColor = Utils.interpretColor(pass.labelColor);
        if ('foregroundColor' in pass)
            textColor = Utils.interpretColor(pass.foregroundColor);
        if ('logoText' in pass)
            logoText = pass.logoText
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
            boardingFromKey = pass.boardingPass.primaryFields[0].key;
            boardingFromTitle = pass.boardingPass.primaryFields[0].label;
            boardingFromValue = pass.boardingPass.primaryFields[0].value;
            boardingToKey = pass.boardingPass.primaryFields[1].key;
            boardingToTitle = pass.boardingPass.primaryFields[1].label;
            boardingToValue = pass.boardingPass.primaryFields[1].value;
            secondaryFields.clear();
            tertiaryFields.clear();
            getFields(pass, style, 'auxiliaryFields', secondaryFields);
            getFields(pass, style, 'secondaryFields', tertiaryFields);
        }
        else {
            primaryKey = pass[style].primaryFields[0].key;
            primaryTitle = pass[style].primaryFields[0].label;
            primaryValue = pass[style].primaryFields[0].value;
            secondaryFields.clear();
            tertiaryFields.clear();
            getFields(pass, style, 'secondaryFields', secondaryFields);
            getFields(pass, style, 'auxiliaryFields', tertiaryFields);
        }
        py.get_pass_changes(pass);
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
                barcodeContent = 'message' in pass.barcodes[barcode] ? pass.barcodes[barcode].message : '';
                barcodeEncoding = 'messageEncoding' in pass.barcodes[barcode] ?  pass.barcodes[barcode].messageEncoding: 'iso-8859-1';
                barcodeType = pass.barcodes[barcode].format.substring(15).toLowerCase();
                barcodeAltText = 'altText' in pass.barcodes[barcode] ? pass.barcodes[barcode].altText : '';
                validCode = true;
                break;
            }
            if (validCode)
                break;
        }
    }
}
