import QtQuick 2.0
import Sailfish.Silica 1.0
import io.thp.pyotherside 1.3


Page {
    id: page

    SilicaListView {
        anchors.fill: parent

        PullDownMenu {

            MenuItem {
                text: "Copyright"
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("Copyright.qml"));
                }
            }
        }

        model: ListModel {
            id: passList
            ListElement { name: ""; filename: ""; path: ""; jsondata: "" }
        }

        delegate: ListItem {
            id: entry

            Image {
                id: passIcon
                x: Theme.paddingLarge
                width: 64
                height: 64
                anchors.verticalCenter: parent.verticalCenter
                source: "image://python" + path + "/icon.png"
            }

            Label {
                text: name
                color: entry.highlighted ? Theme.highlightColor : Theme.primaryColor
                anchors.left: passIcon.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: Theme.paddingMedium
            }

            onClicked: {
                var properties = { path: path, jsondata: jsondata };
                pageStack.push(Qt.resolvedUrl("ShowPass.qml"), properties);
                pageStack.pushAttached(Qt.resolvedUrl("ShowBack.qml"), properties);
            }

            ListView.onAdd: AddAnimation {
                target: entry
            }

            ListView.onRemove: RemoveAnimation {
                target: entry
            }
        }

        VerticalScrollDecorator {}
    }

    BusyIndicator {
        id: busy
        anchors.centerIn: parent
        size: BusyIndicatorSize.Large
        running: true
    }

    Label {
        anchors.centerIn: parent
        text: qsTr("no passes found")
        color: Theme.highlightColor
        visible: passList.count == 0 && !busy.running
    }

    Python {
        id: py

        function scanHome() {

            function updatePassList(newpasses) {
                // remove vanished passes
                var pass = 0;
                var newpass = 0;
                while (pass < passList.count) {
                    var path = passList.get(pass).path;
                    var found = false;
                    for (newpass = 0; newpass < newpasses.length; newpass++) {
                        if (newpasses[newpass].path === path) {
                            found = true;
                            break;
                        }
                    }
                    if (found) {
                        pass++;
                    }
                    else {
                        passList.remove(pass);
                    }
                }
                // add new passes (and change ordering if necessary)
                for (newpass = 0; newpass < newpasses.length; newpass++) {
                    var newpath = newpasses[newpass].path;
                    try {
                        if (passList.get(newpass).path === newpath)
                            continue;
                    }
                    catch(e) {}
                    // look if it's somewhere else
                    var isat = -1;
                    for (pass = 0; pass < pass.length; pass++) {
                        if (passList.get(pass).path === newpath) {
                            isat = pass;
                            break;
                        }
                    }
                    if (isat !== -1) {
                        // yes: move it here
                        passList.move(isat, newpass);
                    }
                    else {
                        // no: insert it here
                        passList.insert(newpass, newpasses[newpass]);
                    }
                }
            }

            call("zipreader.scan_home", [], function(answers) {
                busy.running = false;
                var directories = answers[0];
                var passes = answers[1];
                // set directories on watchlist
                homeWatcher.updatePaths(directories);
                // get proper names
                var loadlist = [];
                for (var pass = 0; pass < passes.length; pass++) {
                    try {
                        var current = passes[pass];
                        var data = JSON.parse(current.data);
                        var name = "";
                        if ('description' in data && data.description !== '') {
                            name = data.description;
                        }
                        else {
                            name = current.name;
                            if (name.substring(name.length - 7) === '.pkpass')
                                name = name.substring(0, name.length - 7);
                        }
                        loadlist[loadlist.length] = {name: name, path: current.path, jsondata: current.data};
                    }
                    catch(e) {}
                }
                loadlist.sort(function(a, b) {
                    return a.name.localeCompare(b.name);
                });
                updatePassList(loadlist);
            });
        }

        Component.onCompleted: {
            passList.clear();
            addImportPath(Qt.resolvedUrl('..'));
            importModule('zipreader', function() {
                scanHome();
            });
            importModule('dtformat', null);
        }
    }

    Connections {
        target: homeWatcher
        onDirectoryChanged: {
            py.scanHome();
        }
    }
}
