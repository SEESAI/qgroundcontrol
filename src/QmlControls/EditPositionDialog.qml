/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import QGroundControl
import QGroundControl.Palette
import QGroundControl.Controls
import QGroundControl.FactControls
import QGroundControl.ScreenTools
import QGroundControl.Controllers

QGCPopupDialog {
    id:         root
    title:      qsTr("Edit Position")
    buttons:    Dialog.Close

    property alias coordinate: controller.coordinate

    property real   _margin:        ScreenTools.defaultFontPixelWidth / 2
    property real   _fieldWidth:    ScreenTools.defaultFontPixelWidth * 10.5

    EditPositionDialogController {
        id: controller

        Component.onCompleted: initValues()
    }

    Column {
        id:         column
        width:      40 * ScreenTools.defaultFontPixelWidth
        spacing:    ScreenTools.defaultFontPixelHeight

        GridLayout {
            anchors.left:   parent.left
            anchors.right:  parent.right
            columnSpacing:  _margin
            rowSpacing:     _margin
            columns:        2

            QGCLabel {
                text: qsTr("Latitude")
            }
            FactTextField {
                fact:               controller.latitude
                Layout.fillWidth:   true
            }

            QGCLabel {
                text: qsTr("Longitude")
            }
            FactTextField {
                fact:               controller.longitude
                Layout.fillWidth:   true
            }

            QGCButton {
                text:               qsTr("Set Geographic")
                Layout.alignment:   Qt.AlignRight
                Layout.columnSpan:  2
                onClicked: {
                    controller.setFromGeo()
                    root.close()
                }
            }

            Item { width: 1; height: ScreenTools.defaultFontPixelHeight; Layout.columnSpan: 2}

            QGCLabel {
                text: qsTr("Zone")
            }
            FactTextField {
                fact:               controller.zone
                Layout.fillWidth:   true
            }

            QGCLabel {
                text: qsTr("Hemisphere")
            }
            FactComboBox {
                fact:               controller.hemisphere
                indexModel:         false
                Layout.fillWidth:   true
            }

            QGCLabel {
                text: qsTr("Easting")
            }
            FactTextField {
                fact:               controller.easting
                Layout.fillWidth:   true
            }

            QGCLabel {
                text: qsTr("Northing")
            }
            FactTextField {
                fact:               controller.northing
                Layout.fillWidth:   true
            }

            QGCButton {
                text:               qsTr("Set UTM")
                Layout.alignment:   Qt.AlignRight
                Layout.columnSpan:  2
                onClicked: {
                    controller.setFromUTM()
                    root.close()
                }
            }

            Item { width: 1; height: ScreenTools.defaultFontPixelHeight; Layout.columnSpan: 2}

            QGCLabel {
                text:              qsTr("MGRS")
            }
            FactTextField {
                fact:              controller.mgrs
                Layout.fillWidth:  true
            }

            QGCButton {
                text:              qsTr("Set MGRS")
                Layout.alignment:  Qt.AlignRight
                Layout.columnSpan: 2
                onClicked: {
                    controller.setFromMGRS()
                    root.close()
                }
            }

            Item { width: 1; height: ScreenTools.defaultFontPixelHeight; Layout.columnSpan: 2}

            QGCButton {
                text:              qsTr("Set From Vehicle Position")
                visible:           QGroundControl.multiVehicleManager.activeVehicle && QGroundControl.multiVehicleManager.activeVehicle.coordinate.isValid
                Layout.alignment:  Qt.AlignRight
                Layout.columnSpan: 2
                onClicked: {
                    controller.setFromVehicle()
                    root.close()
                }
            }
        }
    }
}
