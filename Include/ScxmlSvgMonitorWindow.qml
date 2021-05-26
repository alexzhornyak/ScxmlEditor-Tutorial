import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 2.0

import ScxmlSvgQmlMonitor 1.0

Window {
    id: windowMonitor

    property alias svgMonitor: svgMonitor

    function rectCenter(rect) {
        var xCenter = rect.x + rect.width / 2
        var yCenter = rect.y + rect.height / 2
        return Qt.point(xCenter, yCenter)
    }

    function fitMonitorInView() {
        svgMonitor.fitInView(Qt.rect(0,0,flickMonitor.width,flickMonitor.height),
                                    Qt.KeepAspectRatio)
    }

    Row {
        id: headerItem

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 5

        height: 30

        spacing: 5

        Label {
            width: 50
            height: parent.height
            text: svgMonitor.zoomPercent + " %"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }

        Button {
            width: 100
            height: parent.height
            text: qsTr("Zoom In")
            onClicked: svgMonitor.zoomIn()
        }

        Button {
            width: 100
            height: parent.height
            text: qsTr("Zoom Out")
            onClicked: svgMonitor.zoomOut()
        }

        Button {
            width: 100
            height: parent.height
            text: qsTr("Zoom Reset")
            onClicked: svgMonitor.resetZoom()
        }

        Button {
            width: 100
            height: parent.height
            text: qsTr("Fit In View")
            onClicked: fitMonitorInView()
        }
    }

    Flickable {
        id: flickMonitor

        anchors.top: headerItem.bottom
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        boundsBehavior: Flickable.StopAtBounds
        contentHeight: svgMonitor.height;
        contentWidth: svgMonitor.width;

        clip: true

        ScxmlSvgQmlMonitor {
            id: svgMonitor

            width: svgMonitor.paintedBounds.width
            height: svgMonitor.paintedBounds.height

            onScxmlStateMachineChanged: {
                if (scxmlStateMachine!=null) {
                    fitMonitorInView()
                }
            }
        }

        MouseArea {
            id: dragArea
            hoverEnabled: true
            anchors.fill: parent

            drag.target: svgMonitor
            drag.filterChildren: true

            drag.minimumX: svgMonitor.x
            drag.maximumX: svgMonitor.width - dragArea.width
            drag.minimumY: svgMonitor.y
            drag.maximumY: svgMonitor.height - dragArea.height
        }

    }

}
