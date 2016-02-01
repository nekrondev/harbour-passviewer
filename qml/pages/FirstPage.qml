import QtQuick 2.0
import Sailfish.Silica 1.0
import QtPositioning 5.2
import org.nemomobile.dbus 2.0


Page {
    id: page
    allowedOrientations: Orientation.All

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
            ListElement { name: ""; path: ""; points: -1; jsondata: ""; typeId: ""; updateable: false }
        }

        delegate: ListItem {
            id: entry
            contentHeight: passIcon.height + Theme.paddingSmall * 2

            Image {
                id: passIcon
                x: Theme.horizontalPageMargin
                width: 87  // 1.5 times the recommended icon size
                height: 87
                anchors.verticalCenter: parent.verticalCenter
                source: "image://zipimage" + path + "/icon.png"
            }

            Label {
                text: name
                textFormat: Text.PlainText
                width: parent.width - passIcon.width - Theme.horizontalPageMargin * 2 - Theme.paddingMedium
                truncationMode: TruncationMode.Fade
                color: entry.highlighted ? Theme.highlightColor : points != -1 ? Theme.primaryColor : Theme.secondaryColor
                anchors.left: passIcon.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: Theme.paddingMedium
            }

            menu: ContextMenu {

                MenuItem {
                    text: qsTr("Show")
                    onClicked: {
                        var properties = { name: name, path: path, jsondata: jsondata, updateable: updateable };
                        pageStack.push(Qt.resolvedUrl("ShowPass.qml"), properties);
                        pageStack.pushAttached(Qt.resolvedUrl("ShowBack.qml"), properties);
                    }
                }

                MenuItem {
                    text: qsTr("Update")
                    visible: updateable
                    onClicked: {
                        passHandler.updatePass(path);
                    }
                }

                MenuItem {
                    text: qsTr("Delete")
                    onClicked: {
                        var delPath = path;
                        deleteRemorse.execute(entry, qsTr("Deleting"), function(){
                            passHandler.removePass(delPath);
                            for (var entry = 0; entry < passList.count; entry++) {
                                if (passList.get(entry).path === delPath) {
                                    passList.remove(entry);
                                    break;
                                }
                            }
                        });
                    }
                }
            }

            onClicked: {
                var properties = { name: name, path: path, jsondata: jsondata, updateable: updateable };
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

    Component.onCompleted: {
        // initial pass scan
        passList.clear();
        homeWatcher.scanHome();
    }

    Connections {
        target: homeWatcher
        onPassesFound: {
            // check for vanished passes...
            var removePasses = [];
            for (var oldpass = 0; oldpass < passList.count; oldpass++) {
                var found = false;
                for (var newpass = 0; newpass < list.length; newpass++) {
                    if (passList.get(oldpass).path === list[newpass].path) {
                        found = true;
                        break;
                    }
                }
                if (!found)
                    removePasses.push(passList.get(oldpass).path);
            }
            // ...and remove them
            for (var toRemove = 0; toRemove < removePasses.length; toRemove++)
                removePass(removePasses[toRemove]);
            // calculate the points sort the list, and update GPS precision
            var close = false;
            for (var pass = 0; pass < list.length; pass++) {
                if (calcPoints(list[pass]))
                    close = true;
            }
            list.sort(comparePasses);
            if (locator.precise !== close)
                locator.precise = close;
            // update the pass list
            updatePasses(list);
            // on the first run: stop the busy animation and show the pass called in the CLI (if given)
            if (busy.running) {
                busy.running = false;
                if (Qt.application.arguments.length === 2)
                    dbus.openPass(Qt.application.arguments[1]);
            }
            // report a successful update and redraw the pass, if it's shown
            if (update) {
                notificator.bannerNotification(qsTr("pass update successful"), "");
                if (pageStack.depth > 1)
                    dbus.openPass(pageStack.nextPage().path);
            }
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
        target: passHandler
        onUpdateFinished: {
            switch (state) {
            case "not updateable":
                notificator.bannerNotification(qsTr("pass not updateable"), "");
                break;
            case "no new version":
                notificator.bannerNotification(qsTr("no new version for pass"), "");
                break;
            case "update failed":
                notificator.bannerNotification(qsTr("pass update failed"), "");
                break;
            case "ok":
                homeWatcher.scanHome(true);
            }
        }
    }

    DBusAdaptor {
        id: dbus
        service: "ch.p2501.harbour_passviewer"
        iface: "ch.p2501.harbour_passviewer"
        path: "/ch/p2501/harbour_passviewer"
        xml: '<interface name="ch.p2501.harbour_passviewer">' +
             '  <method name="openPass">' +
             '    <arg name="origin" type="s" direction="in"/>' +
             '  </method>' +
             '</interface>'
        function openPass(origin) {
            // bring the app to the foreground
            appWindow.activate();
            // get the canonical path
            origin = passHandler.getCanonicalPath(origin);
            // look for a matching pass
            for (var pass = 0; pass < passList.count; pass++) {
                if (passList.get(pass).path === origin) {
                    // found one: let's show it
                    var properties = { name: passList.get(pass).name, path: passList.get(pass).path, jsondata: passList.get(pass).jsondata, updateable: passList.get(pass).updateable };
                    pageStack.pop(page, PageStackAction.Immediate);
                    pageStack.push(Qt.resolvedUrl("ShowPass.qml"), properties, PageStackAction.Immediate);
                    pageStack.pushAttached(Qt.resolvedUrl("ShowBack.qml"), properties);
                }
            }
        }
    }

    function updatePasses(newpasses) {
        // inserts, updates or moves the passes in the model
        var oldpoints = -1;
        for (var pass = 0; pass < newpasses.length; pass++) {
            if (pass < passList.count && passList.get(pass).path === newpasses[pass].path) {
                // update
                oldpoints = passList.get(pass).points;
                passList.set(pass, newpasses[pass]);
            }
            else {
                // check if it's further down
                var moved = false;
                for(var oldpass = pass + 1; oldpass < passList.count; oldpass++) {
                    if (passList.get(oldpass).path === newpasses[pass].path) {
                        // move and update
                        oldpoints = passList.get(oldpass).points;
                        passList.move(oldpass, pass, 1);
                        passList.set(pass, newpasses[pass]);
                        moved = true;
                        break;
                    }
                }
                if (!moved)
                    passList.insert(pass, newpasses[pass]);  // new pass
            }
            // update pass notifications
            if (newpasses[pass].points !== oldpoints) {
                if (newpasses[pass].points !== -1)
                    notificator.addNotification(newpasses[pass].path, newpasses[pass].name, '');
                else
                    notificator.removeNotification(newpasses[pass].path);
            }
        }
        // if the topmost pass is active, show it on the cover
        if (passList.count > 1 && passList.get(0).points !== -1) {
            topIcon = "image://zipimage" + passList.get(0).path + "/icon.png";
            topName = passList.get(0).name;
            topPath = passList.get(0).path;
            topData = passList.get(0).jsondata;
            topUpdateable = passList.get(0).updateable;
        }
        else {
            topIcon = "";
            topName = "";
            topPath = "";
            topData = "";
            topUpdateable = "";
        }
    }

    function getPass(path) {
        // gets the pass with the given path
        for (var pass = 0; pass < passList.count; pass++) {
            if (passList.get(pass).path === path)
                return pass;
        }
        return null;
    }

    function removePass(path) {
        // removes the pass with the given path
        var pass = getPass(path);
        if (pass !== null) {
            notificator.removeNotification(passList.get(pass).path);
            passList.remove(pass);
        }
    }

    function calcPoints(pass) {
        // calculates the relevancy points of a pass and says whether we're close to target coordinates
        /* Lower numbers are more relevant, but -1 means "not active".
           This is because "null" is not allowed in models. */
        pass.points = -1;
        var data = JSON.parse(pass.jsondata);
        var close = false;
        if (settingsStore.checkTime && "relevantDate" in data) {
            // close to target time?
            try {
                var targetTime = new Date(data.relevantDate);
            }
            catch(e) {
                notificator.removeNotification(pass.path);
                return false;  // faulty pass
            }
            var now = new Date();
            var timeDiff = targetTime - now;  // time difference in milliseconds
            if (timeDiff >= 0 && timeDiff <= settingsStore.hoursBefore * 3600000) {
                pass.points = timeDiff / 1000;
            }
            else if (timeDiff < 0 && Math.abs(timeDiff) <= settingsStore.hoursAfter * 3600000) {
                pass.points = Math.abs(timeDiff) / 1000;
            }
        }
        if (pass.points === -1 && settingsStore.checkDistance && "locations" in data && locator.valid && locator.position.latitudeValid && locator.position.longitudeValid) {
            // close to one of the target destinations?
            var here = locator.position.coordinate;
            try {
                for (var location = 0; location < data.locations.length; location++) {
                    var there = QtPositioning.coordinate(data.locations[location].latitude, data.locations[location].longitude);
                    var posDiff = here.distanceTo(there);  // distance in meter
                    var maxDistance = settingsStore.maxDistance;
                    if (settingsStore.overrideDistance && "maxDistance" in data)
                        maxDistance = data.maxDistance;
                    if (posDiff <= maxDistance && (pass.points === -1 || pass.points > posDiff))
                        pass.points = posDiff;
                    if (posDiff <= maxDistance + 1000)
                        close = true;  // not relevant yet, but somewhat close
                }
            }
            catch (e) {
                notificator.removeNotification(pass.path);
                return false;  // faulty pass
            }
            if (pass.points !== -1)
                pass.points += 36000; // close to target time is more relevant than close to destination
        }
        return close;
    }

    function comparePasses(a, b) {
        // sort active passes to the top
        // "smaller" passes get sorted upwards
        if (a.points !== -1 && b.points === -1)
            return -1;
        if (a.points === -1 && b.points !== -1)
            return 1;
        // if both are active, check who's more relevant
        if (a.points !== b.points)
            return a.points - b.points;
        // group by pass type ID
        if (a.typeId !== b.typeId)
            return a.typeId.localeCompare(b.typeId);
        // otherwise order by name
        return a.name.localeCompare(b.name);
    }

    function checkPassList() {
        // recalculates all relevancy points, reorders the list and updates GPS precision
        var passes = [];
        var close = false;
        for (var pass = 0; pass < passList.count; pass++) {
            var thisPass = passList.get(pass);
            if (calcPoints(thisPass))
                close = true;
            passes.push(thisPass);
        }
        passes.sort(comparePasses);
        if (locator.precise !== close)
            locator.precise = close;
        updatePasses(passes);
    }
}
