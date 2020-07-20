import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.12

ApplicationWindow {
    property QtObject mediator: null
    property var offset: 100

    id: root
    visible: true
    width: 150
    height: 500

    onClosing: {
        mediator.stopped = true
    }

    onMediatorChanged: {
        if (mediator != null) {
            queueView.yHwm = mediator.hwm
            queueView.yLwm = mediator.lwm
        }
    }

    Item {
        id: rootItem
        anchors.centerIn: parent
        width: parent.width - offset
        height: parent.height - offset

        QueueView {
            id: queueView
            anchors.fill: parent
            value: owner.fullnessQueue
            queueSize: owner.fixedSizeQueue

            onHwmChanged: {
                if(mediator != null)
                    mediator.hwm = hwm
            }
            onLwmChanged: {
                if(mediator != null)
                    mediator.lwm = lwm
            }
            Component.onCompleted: previewComponent.createObject(root)
        }
    }

    Component {
        id: previewComponent
        Rectangle {
            property alias textItem: text

            id: preview
            color: "black"
            opacity: 0.7
            anchors.fill: parent

            MouseArea {
                anchors.fill: parent
                onPressed: parent.destroy()
            }

            Text {
                id: text
                text: qsTr("Drag HWM or LWM \nitem to change them")
                anchors.centerIn: parent
                color: "white"
            }
        }
    }
}
