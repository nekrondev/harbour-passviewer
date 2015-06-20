import QtQuick 2.0
import Sailfish.Silica 1.0
import QtPositioning 5.2
import io.thp.pyotherside 1.3


Page {
    id: page

    property string uid: "firstPage"

    SilicaListView {
        anchors.fill: parent

        PullDownMenu {

            MenuItem {
                text: qsTr("Copyright")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("Copyright.qml"));
                }
            }

            MenuItem {
                text: qsTr("Settings")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("Settings.qml"));
                }
            }
        }

        model: ListModel {
            id: passList
            ListElement { name: ""; filename: ""; path: ""; points: -1; jsondata: ""; typeId: "" }
        }

        delegate: ListItem {
            id: entry
            contentHeight: passIcon.height

            Image {
                id: passIcon
                x: Theme.paddingLarge
                width: 87  // 1.5 times the recommended icon size
                height: 87
                anchors.verticalCenter: parent.verticalCenter
                source: "image://python" + path + "/icon.png"
            }

            Label {
                text: name
                width: parent.width - passIcon.width - Theme.paddingLarge * 2 - Theme.paddingMedium
                truncationMode: TruncationMode.Fade
                color: entry.highlighted ? Theme.highlightColor : points >= 0 ? Theme.primaryColor : Theme.secondaryColor
                anchors.left: passIcon.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: Theme.paddingMedium
            }

            menu: ContextMenu {

                MenuItem {
                    text: qsTr("Show")
                    onClicked: {
                        var properties = { path: path, jsondata: jsondata };
                        pageStack.push(Qt.resolvedUrl("ShowPass.qml"), properties);
                        pageStack.pushAttached(Qt.resolvedUrl("ShowBack.qml"), properties);
                    }
                }

                MenuItem {
                    text: qsTr("Delete")
                    onClicked: {
                        var delPath = path;
                        deleteRemorse.execute(entry, qsTr("Deleting"), function(){ py.removePass(delPath) });
                    }
                }
            }

            onClicked: {
                var properties = { name: name, path: path, jsondata: jsondata };
                pageStack.push(Qt.resolvedUrl("ShowPass.qml"), properties);
                pageStack.pushAttached(Qt.resolvedUrl("ShowBack.qml"), properties);
            }

            ListView.onAdd: AddAnimation {
                target: entry
            }

            ListView.onRemove: RemoveAnimation {
                target: entry
            }

            RemorseItem {
                id: deleteRemorse
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
        text: qsTr("No passes found")
        color: Theme.highlightColor
        visible: passList.count == 0 && !busy.running
    }

    Timer {
        id: checkTimer
        interval: 60000
        repeat: true
        onTriggered: checkPassList()
    }

    PositionSource {
        id: locator
        property bool precise: false
        active: settingsStore.checkDistance
        updateInterval: 60000
        preferredPositioningMethods: (precise || !settingsStore.useHere) ? PositionSource.AllPositioningMethods : PositionSource.NonSatellitePositioningMethods
        onPositionChanged: checkPassList()
    }

    Python {
        id: py

        function scanHome() {
            function nameFromData(data, defName) {
                try {
                    var styles = ["boardingPass", "coupon", "eventTicket", "storeCard", "generic"];
                    var style = '';
                    for (var key = 0; key < styles.length; key++) {
                        if (styles[key] in data) {
                            style = styles[key];
                            break;
                        }
                    }
                    if (style === "boardingPass")
                        return data.boardingPass.primaryFields[0].value + " → " + data.boardingPass.primaryFields[1].value;
                    else
                        return data[style].primaryFields[0].label + " " + data[style].primaryFields[0].value;
                }
                catch(e) {
                    if (defName.substring(defName.length - 7) === ".pkpass")
                        defName = defName.substring(0, defName.length - 7);
                    return defName;
                }
            }

            checkTimer.stop();
            call("zipreader.scan_home", [], function(answers) {
                busy.running = false;
                var directories = answers[0];
                var passes = answers[1];
                // set directories on watchlist
                homeWatcher.updatePaths(directories);
                // handle the passes
                var loadlist = [];
                for (var pass = 0; pass < passes.length; pass++) {
                    try {
                        var current = passes[pass];
                        // get proper names
                        var data = undefined;
                        try {
                            data = JSON.parse(current.data);
                        }
                        catch(e) {}
                        var name = nameFromData(data, current.name);
                        // and the pass type id
                        var typeId = "";
                        try {
                            typeId = data.passTypeIdentifier;
                        }
                        catch(e) {}
                        loadlist[loadlist.length] = {name: name, path: current.path, points: -1, jsondata: current.data, typeId: typeId};
                    }
                    catch(e) {}
                }
                loadlist.sort(page.comparePasses);
                page.updatePassList(loadlist);
                page.checkPassList();
                if (settingsStore.checkTime)
                    checkTimer.start();
            });
        }

        function removePass(path) {
            call("zipreader.remove_pass", [path], null);
        }

        Component.onCompleted: {
            passList.clear();
            addImportPath(Qt.resolvedUrl('..'));
            importModule('zipreader', function() {
                scanHome();
            });
            importModule('dtformat', null);
            importModule('ical', null);
        }
    }

    Connections {
        target: homeWatcher
        onDirectoryChanged: {
            py.scanHome();
        }
    }

    Connections {
        target: settingsStore
        onCheckTimeChanged: checkPassList()
        onHoursBeforeChanged: checkPassList()
        onHoursAfterChanged: checkPassList()
        onCheckDistanceChanged: checkPassList()
        onMaxDistanceChanged: checkPassList()
        onOverrideDistanceChanged: checkPassList()
    }

    Connections {
        target: notificator
        onNotificationClicked: {
            for (var pass = 0; pass < passList.count; pass++) {
                if (passList.get(pass).path === path) {
                    var properties = { path: passList.get(pass).path, jsondata: passList.get(pass).jsondata };
                    pageStack.pop(page, PageStackAction.Immediate);
                    pageStack.push(Qt.resolvedUrl("ShowPass.qml"), properties, PageStackAction.Immediate);
                    pageStack.pushAttached(Qt.resolvedUrl("ShowBack.qml"), properties);
                    appWindow.activate();
                }
            }
        }
    }

    function isActive(jsondata) {
        var points = -1;
        var data = JSON.parse(jsondata);
        if (settingsStore.checkTime && "relevantDate" in data) {
            try {
                var targetTime = new Date(data.relevantDate);
            }
            catch(e) {
                return -1; // faulty pass
            }
            var now = new Date();
            var timeDiff = targetTime - now;
            if (timeDiff >= 0 && timeDiff <= settingsStore.hoursBefore * 3600000) {
                points = timeDiff / 1000;
            }
            else if (timeDiff < 0 && Math.abs(timeDiff) <= settingsStore.hoursAfter * 3600000) {
                points = Math.abs(timeDiff) / 1000;
            }
        }
        var close = false;
        if (points < 0 && settingsStore.checkDistance && locator.valid && "locations" in data && locator.position.latitudeValid && locator.position.longitudeValid) {
            var here = locator.position.coordinate;
            for (var location = 0; location < data.locations.length; location++) {
                try {
                    var there = QtPositioning.coordinate(data.locations[location].latitude, data.locations[location].longitude);
                    var posDiff = here.distanceTo(there);
                    var maxDistance = settingsStore.maxDistance;
                    if (settingsStore.overrideDistance && "maxDistance" in data)
                        maxDistance = data.maxDistance;
                    if (posDiff <= maxDistance && (points == -1 || points > posDiff))
                        points = posDiff + 36000;
                    if (posDiff <= maxDistance + 1000)
                        close = true;
                }
                catch(e) {}
            }
        }
        return([points, close]);
    }

    function comparePasses(a, b) {
        // sort active passes to the top
        if (a.points >= 0 && b.points < 0)
            return -1;
        if (b.points >= 0 && a.points < 0)
            return 1;
        // if both are active, check who's more active
        if (a.points > 0 && b.points > 0 && a.points !== b.points)
            return a.points - b.points;
        // group by pass type ID
        if (a.typeId !== b.typeId)
            return a.typeId.localeCompare(b.typeId);
        // otherwise order by name
        return a.name.localeCompare(b.name);
    }

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
            for (pass = 0; pass < passList.count; pass++) {
                if (passList.get(pass).path === newpath) {
                    isat = pass;
                    break;
                }
            }
            if (isat !== -1) {
                // yes: move it here
                passList.move(isat, newpass, 1);
            }
            else {
                // no: insert it here
                passList.insert(newpass, newpasses[newpass]);
            }
        }
        if (passList.count > 0 && passList.get(0).points >= 0) {
            topIcon = "image://python" + passList.get(0).path + "/icon.png";
            topPath = passList.get(0).path;
            topData = passList.get(0).jsondata;
        }
        else {
            topIcon = "";
            topPath = "";
            topData = "";
        }
    }

    function checkPassList() {
        // check if passes have been activated or deactivated
        var changed = false;
        var close = false;
        for (var pass = 0; pass < passList.count; pass++) {
            var active = isActive(passList.get(pass).jsondata);
            if (active[0] > -1 && passList.get(pass).points === -1)
                notificator.addNotification(passList.get(pass).path, passList.get(pass).name);
            if (active[0] === -1 && passList.get(pass).points > -1)
                notificator.removeNotification(passList.get(pass).path);
            if (active[0] !== passList.get(pass).points) {
                passList.setProperty(pass, "points", active[0]);
                changed = true;
            }
            if (active[1])
                close = true;
        }
        if (changed) {
            // reorder passes
            var passes = [];
            for (pass = 0; pass < passList.count; pass++) {
                passes[passes.length] = passList.get(pass);
            }
            passes.sort(comparePasses);
            updatePassList(passes);
        }
        if (locator.precise !== close)
            locator.precise = close;
    }
}
