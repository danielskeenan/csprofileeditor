/**
 * @file Settings.h
 *
 * @author dankeenan
 * @date 4/26/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CS_PROFILE_EDITOR_SRC_CSPROFILEEDITOR_SETTINGS_H_
#define CS_PROFILE_EDITOR_SRC_CSPROFILEEDITOR_SETTINGS_H_

#include <dragoonboots/qsettingscontainer/QSettingsContainer.h>
#include <QString>
#include <QStandardPaths>
#include <QColor>
#include <QPalette>

namespace csprofileeditor {

/**
 * Program settings container
 */
class Settings : public dragoonboots::qsettingscontainer::QSettingsContainer {
 public:
  /** MainWindow Geometry */
  DGSETTINGS_SETTING(QByteArray, MainWindowGeometry, {})

  /** Last path opened in a file dialog */
  DGSETTINGS_SETTING(QString, LastFileDialogPath, QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation))

  /** Recently-opened documents */
  DGSETTINGS_SETTING(QStringList, RecentDocuments, {})

  /** Number of recent documents to store */
  DGSETTINGS_SETTING(int, RecentDocumentsCount, 4)

  /** Path to official ETC CS Personality Editor */
  DGSETTINGS_SETTING(QString, EtcCsPersEditorPath, {})

  /** How errors are formatted */
  DGSETTINGS_SETTING(QColor, ErrorColor, QColor(Qt::GlobalColor::red))

  static QPalette GetErrorPalette() {
    QPalette palette;
    const auto error_color = GetErrorColor();
    palette.setColor(QPalette::Text, error_color);
    palette.setColor(QPalette::WindowText, error_color);
    palette.setColor(QPalette::ToolTipText, error_color);
    palette.setColor(QPalette::ButtonText, error_color);

    return palette;
  }
};

} // csprofileeditor

#endif //CS_PROFILE_EDITOR_SRC_CSPROFILEEDITOR_SETTINGS_H_
