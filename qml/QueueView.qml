import QtQuick 2.12
import QtQuick.Controls 2.4

Rectangle {
    property var hwm
    property var lwm
    property real yHwm
    property real yLwm
    property var value
    property var queueSize
    property var convergenceThreshold


    id: control
    radius: 5
    color: "lightGray"
    border.color: Qt.darker(color)
    border.width: 1

    Component.onCompleted: {
        if(queueSize > 0)
            convergenceThreshold = control.height / queueSize
        if (convergenceThreshold < handleHwm.height * 2) {
            convergenceThreshold = handleHwm.height * 2
        }
    }
    Rectangle {
        width: parent.width
        height: parent.height * (value / 100)
        anchors.bottom: parent.bottom
        radius: parent.radius
        color: "Grey"
    }

    Rectangle {
        id: handleHwm
        x: 0; y: control.height * (1 - yHwm / 100)
        width: parent.width
        height: 8
        clip: true
        color: "black"
        onYChanged: {
            if (y > 0)
                hwm = Math.ceil(100 - ((y * 100.0) / control.height))
        }

        DragHandler {
            id: handlerHwm
            xAxis.enabled: false
            yAxis.minimum: control.y + handleHwm.height
            yAxis.maximum: handleLwm.y - convergenceThreshold
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.SizeVerCursor
        }
    }
    Text {
        id: textHwm
        text: qsTr("HWM")
        width: 50
        height: 15
        color: handleHwm.color
        anchors.left: handleHwm.right
        anchors.verticalCenter: handleHwm.verticalCenter
        horizontalAlignment: Text.AlignHCenter
    }

    Rectangle {
        id: handleLwm
        x: 0; y: control.height * (1 - yLwm / 100)
        width: parent.width
        height: 8
        clip: true
        color: "black"
        onYChanged: {
            if (y > 0)
                lwm = Math.floor(100 - ((y * 100.0) / control.height))
        }

        DragHandler {
            id: handlerLwm
            xAxis.enabled: false
            yAxis.minimum: handleHwm.y + convergenceThreshold
            yAxis.maximum: control.height - handleLwm.height * 2
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.SizeVerCursor
        }
    }

    Text {
        id: textLwm
        text: qsTr("LWM")
        width: 50
        height: 15
        color: handleLwm.color
        anchors.left: handleLwm.right
        anchors.verticalCenter: handleLwm.verticalCenter
        horizontalAlignment: Text.AlignHCenter
    }
}
