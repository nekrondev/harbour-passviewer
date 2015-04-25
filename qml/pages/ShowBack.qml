import QtQuick 2.0
import Sailfish.Silica 1.0
import "../utils.js" as Utils

Page {
    id: page

    property string jsondata: ''
    property string path: ''
    property string labelColor: 'black'
    property string foreColor: 'black'

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: background.height + Theme.paddingLarge * 2

        PullDownMenu {

            MenuItem {
                text: qsTr("Simple View")
                onClicked: {
                    var properties = { path: path, jsondata: jsondata };
                    pageStack.push(Qt.resolvedUrl("ShowSimple.qml"), properties);
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

            Column {
                id: body
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: Theme.paddingMedium
                spacing: Theme.paddingMedium

                Repeater {
                    model: ListModel {
                        id: backFields
                        ListElement { title: ''; value: '' }
                    }

                    Column {

                        Label {
                            text: title
                            color: page.labelColor
                            font.pixelSize: Theme.fontSizeExtraSmall
                        }

                        Label {
                            text: value
                            color: page.foreColor
                            font.pixelSize: Theme.fontSizeExtraSmall
                            width: body.width
                            wrapMode: Text.Wrap
                        }
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
        if ('backgroundColor' in pass)
            background.color = Utils.interpretColor(pass.backgroundColor);
        if ('labelColor' in pass)
            page.labelColor = Utils.interpretColor(pass.labelColor);
        if ('foregroundColor' in pass)
            page.foreColor = Utils.interpretColor(pass.foregroundColor);
        var styles = ["boardingPass", "coupon", "eventTicket", "storeCard", "generic"];
        var style = '';
        for (var key = 0; key < styles.length; key++) {
            if (styles[key] in pass) {
                style = styles[key];
                break;
            }
        }
        backFields.clear();
        getFields(pass, style, 'backFields', backFields);
    }
}
