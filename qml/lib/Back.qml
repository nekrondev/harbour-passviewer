import QtQuick 2.0
import Sailfish.Silica 1.0
import 'utils.js' as Utils

Rectangle {
    id: root
    property string jsondata: ''
    property color backgroundColor: 'white'
    property color labelColor: 'black'
    property color textColor: 'black'
    property alias backFields: backFieldsModel

    height: body.height + Theme.paddingMedium * 2
    color: backgroundColor

    Column {
        id: body
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: Theme.paddingMedium
        spacing: Theme.paddingMedium

        Repeater {
            model: ListModel {
                id: backFieldsModel
                ListElement { title: ''; value: '' }
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
                    font.pixelSize: Theme.fontSizeExtraSmall
                    width: body.width
                    wrapMode: Text.Wrap
                }
            }
        }
    }

    onJsondataChanged: {
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
        if ('backgroundColor' in pass)
            backgroundColor = Utils.interpretColor(pass.backgroundColor);
        if ('labelColor' in pass)
            labelColor = Utils.interpretColor(pass.labelColor);
        if ('foregroundColor' in pass)
            textColor = Utils.interpretColor(pass.foregroundColor);
        var styles = ["boardingPass", "coupon", "eventTicket", "storeCard", "generic"];
        var style = '';
        for (var key = 0; key < styles.length; key++) {
            if (styles[key] in pass) {
                style = styles[key];
                break;
            }
        }
        Utils.checkFields(pass, style);
        backFields.clear();
        getFields(pass, style, 'backFields', backFields);
    }
}
