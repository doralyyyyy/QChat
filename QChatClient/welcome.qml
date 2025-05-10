import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick3D 6.0

Window {
    width: 800
    height: 600
    visible: true
    title: "Welcome to QChat"

    View3D {
        anchors.fill: parent

        environment: SceneEnvironment {
            backgroundMode: SceneEnvironment.Color
            clearColor: "#ffffff"
        }

        PerspectiveCamera {
            id: camera
            z: 600
        }

        DirectionalLight {
            eulerRotation: Qt.vector3d(-45, 45, 0)
        }

        Model {
            source: "#Rectangle"
            scale: Qt.vector3d(4, 4, 4)
            materials: [
                DefaultMaterial {
                    diffuseMap: Texture {
                        source: "qrc:icon.png"
                    }
                }
            ]

            // 从侧面（90度）开始，旋转到另一侧（-90度）
            NumberAnimation on eulerRotation.y {
                from: 90
                to: -90
                duration: 4000
                loops: Animation.Infinite
            }
        }
    }

    Text {
        text: "欢迎使用 QChat"
        font.pixelSize: 32
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        color: "#444"
    }
}
