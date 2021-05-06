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
};

} // csprofileeditor

#endif //CS_PROFILE_EDITOR_SRC_CSPROFILEEDITOR_SETTINGS_H_
