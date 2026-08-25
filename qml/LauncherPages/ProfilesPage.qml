import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "./template"
import "../Actions"

// Adapted plasma-systemmonitor/src/table/BaseTableView.qml
FocusScope {
    id: root

    property var model
    property var editorLoader
    property alias view: tableView

    // From plasma-systemmonitor/src/table/BaseTableView.qml:
    // Column widths in fractions of the entire view width
    // Since column sizes are relative to a given view width, the widths of individual
    // columns need to be expressed as a fraction of the entire width. columnWidths
    // and defaultColumnWidth both store these fractions. Note that minimumColumnWidth
    // is expressed as a pixel value since we do not want that to scale with view width.
    property var columnWidths: []
    property real defaultColumnWidth: 0.1
    property real minimumColumnWidth: 64

    readonly property alias headerHeight: heading.height

    readonly property alias selection: tableView.selectionModel
    readonly property alias rows: tableView.rows
    readonly property alias columns: tableView.columns

    clip: true

    HorizontalHeaderView {
        id: heading

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        syncView: tableView

        width: scrollView.width

        height: 32

        signal resize(int column, real width)

        resizableColumns: true
        clip: true
        activeFocusOnTab: false
        boundsBehavior: Flickable.StopAtBounds
        flickableDirection: Flickable.VerticalFlick

        onResize: (column, width) => {
            tableView.setColumnWidth(column, width)
        }
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent
        anchors.topMargin: heading.height

        property real innerWidth: width - rightPadding

        TableView {
            id: tableView
            anchors.fill: parent

            selectionModel: ItemSelectionModel {
                id: selectionModel
                model: tableView.model

                // From plasma-systemmonitor/src/table/BaseTableView.qml:
                // Below we sync currentIndex to selection if the current index
                // changes but the selection does not. Unfortunately there is a
                // corner case, when the current row is clicked again the
                // selection is removed but current is not. The result is that
                // the current row cannot be re-selected as it is still current.
                // So when that happens, also clear the current index so we can
                // properly re-select.
                //
                // Note that this needs to be done with `callLater()` because
                // the selection change is not atomic and we need to know for
                // sure that we cleared the selection rather than move it.
                onSelectionChanged: (selected, deselected) => {
                    Qt.callLater(maybeClearCurrent)
                }

                function maybeClearCurrent() {
                    if (!hasSelection) {
                        clearCurrentIndex()
                    }
                }
            }
            model: root.model

            activeFocusOnTab: true

            clip: true
            pixelAligned: true
            boundsBehavior: Flickable.StopAtBounds

            selectionBehavior: TableView.SelectRows
            selectionMode: TableView.SingleSelection

            onCurrentRowChanged: {
                // Workaround for QTBUG-114999
                if (!selectionModel.hasSelection) {
                    selectionModel.select(index(currentRow, 0), ItemSelectionModel.Select | ItemSelectionModel.Rows)
                }
            }

            columnWidthProvider: function(index) {
                let column = index
                // From plasma-systemmonitor/src/table/BaseTableView.qml:
                // Resizing sets the explicit column width and has no other trigger. If
                // we don't make use of that value we can't resize. So read the value,
                // convert it to a fraction of total width and write it back to
                // columnWidths, then clear the explicit column width again so that
                // resizing updates the column width properly. This isn't the prettiest
                // of solutions but at least makes things work the way we want.
                let explicitWidth = explicitColumnWidth(index)
                if (explicitWidth >= 0) {
                    let w = explicitWidth / width
                    root.columnWidths[column] = w
                    root.columnWidthsChanged()
                    clearColumnWidths()
                }

                // If last visible column, fill up remaining space
                if (index === columnWidths.length - 1) {
                    let usedSpace = 0
                    for (let i = 0; i < index; i++) {
                        usedSpace += (root.columnWidths[i] ?? root.defaultColumnWidth) * scrollView.innerWidth
                    }
                    return Math.max(Math.floor(scrollView.innerWidth - usedSpace), root.minimumColumnWidth)
                }

                let columnWidth = root.columnWidths[column]
                return Math.max(Math.floor((columnWidth ?? root.defaultColumnWidth) * scrollView.innerWidth), root.minimumColumnWidth)
            }

            rowHeightProvider: function(row) { return 16 }

            delegate: ProfileCellDelegate {
                contextMenu: profilesMenu
                text: model.display
            }
        }
    }

    SelectionRectangle {
        target: tableView

        selectionMode: SelectionRectangle.Drag
        topLeftHandle: null
        bottomRightHandle: null
    }

    ProfilesTableMenu {
        id: profilesMenu
        editorLoader: root.editorLoader
    }
}