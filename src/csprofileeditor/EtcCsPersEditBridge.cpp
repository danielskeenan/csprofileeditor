/**
 * @file EtcCsPersEditBridge.cpp
 *
 * @author dankeenan
 * @date 4/27/21
 * @copyright (c) 2021 Dan Keenan
 */

#include "EtcCsPersEditBridge.h"
#include "Settings.h"
#include <csprofile/parameter/Parameter.h>
#include <QApplication>
#include <cslibs/disc/DiscDb.h>
#include <cslibs/effect/EffectDb.h>
#include <cslibs/gel/GelDb.h>
#include <cslibs/gobo/GoboDb.h>
#include <cslibs/except.h>
#include <csprofile/logging.h>
#include <sqlite3.h>
#include <filesystem>

namespace csprofileeditor {

const QStringList EtcCsPersEditBridge::kImagesDataPaths = {
    "bin/config/CSEDIT_IMAGES.dat",
    "config/CSEDIT_IMAGES.dat",
};
const QStringList EtcCsPersEditBridge::kImagesIndexPaths = {
    "bin/config/CSEDIT_IMAGES.idx",
    "config/CSEDIT_IMAGES.idx",
};
const QStringList EtcCsPersEditBridge::kDiscsPaths = {
    "bin/config/DISCS.def",
    "config/DISCS.def",
};
const QStringList EtcCsPersEditBridge::kEffectsPaths = {
    "bin/config/EFFECTS.def",
    "config/EFFECTS.def",
};
const QStringList EtcCsPersEditBridge::kGelsPaths = {
    "bin/config/GELS.def",
    "config/GELS.def",
};
const QStringList EtcCsPersEditBridge::kGobosPaths = {
    "bin/config/GOBOS.def",
    "config/GOBOS.def",
};

bool EtcCsPersEditBridge::ValidateInstallationDirectory() {
  if (Settings::GetEtcCsPersEditorPath().isEmpty()) {
    return false;
  }
  return ValidateInstallationDirectory(Settings::GetEtcCsPersEditorPath());
}

bool EtcCsPersEditBridge::ValidateInstallationDirectory(const QDir &path) {
  return GetCsEditImagesDataPath(path)
      && GetCsEditImagesIndexPath(path)
      && GetCsEditDiscsPath(path)
      && GetCsEditEffectsPath(path)
      && GetCsEditGelsPath(path)
      && GetCsEditGobosPath(path);
}

std::optional<QString> EtcCsPersEditBridge::GetCsEditImagesDataPath() {
  if (Settings::GetEtcCsPersEditorPath().isEmpty()) {
    return {};
  }
  return GetCsEditImagesDataPath(Settings::GetEtcCsPersEditorPath());
}

std::optional<QString> EtcCsPersEditBridge::GetCsEditImagesDataPath(const QDir &root) {
  return GetFirstExistentPath(root, kImagesDataPaths);
}

std::optional<QString> EtcCsPersEditBridge::GetCsEditImagesIndexPath() {
  if (Settings::GetEtcCsPersEditorPath().isEmpty()) {
    return {};
  }
  return GetCsEditImagesIndexPath(Settings::GetEtcCsPersEditorPath());
}

std::optional<QString> EtcCsPersEditBridge::GetCsEditImagesIndexPath(const QDir &root) {
  return GetFirstExistentPath(root, kImagesIndexPaths);
}

std::optional<QString> EtcCsPersEditBridge::GetCsEditDiscsPath() {
  if (Settings::GetEtcCsPersEditorPath().isEmpty()) {
    return {};
  }
  return GetCsEditDiscsPath(Settings::GetEtcCsPersEditorPath());
}

std::optional<QString> EtcCsPersEditBridge::GetCsEditDiscsPath(const QDir &root) {
  return GetFirstExistentPath(root, kDiscsPaths);
}

std::optional<QString> EtcCsPersEditBridge::GetCsEditEffectsPath() {
  if (Settings::GetEtcCsPersEditorPath().isEmpty()) {
    return {};
  }
  return GetCsEditEffectsPath(Settings::GetEtcCsPersEditorPath());
}

std::optional<QString> EtcCsPersEditBridge::GetCsEditEffectsPath(const QDir &root) {
  return GetFirstExistentPath(root, kEffectsPaths);
}

std::optional<QString> EtcCsPersEditBridge::GetCsEditGelsPath() {
  if (Settings::GetEtcCsPersEditorPath().isEmpty()) {
    return {};
  }
  return GetCsEditGelsPath(Settings::GetEtcCsPersEditorPath());
}

std::optional<QString> EtcCsPersEditBridge::GetCsEditGelsPath(const QDir &root) {
  return GetFirstExistentPath(root, kGelsPaths);
}

std::optional<QString> EtcCsPersEditBridge::GetCsEditGobosPath() {
  if (Settings::GetEtcCsPersEditorPath().isEmpty()) {
    return {};
  }
  return GetCsEditGobosPath(Settings::GetEtcCsPersEditorPath());
}

std::optional<QString> EtcCsPersEditBridge::GetCsEditGobosPath(const QDir &root) {
  return GetFirstExistentPath(root, kGobosPaths);
}

static const std::unordered_map<csprofile::parameter::Type, QString> kParameterTypeNames{
    {csprofile::parameter::Type::kNone, ""},
    {csprofile::parameter::Type::kIntensity, qApp->translate("EtcCsPersEditBridge", "Intensity")},
    {csprofile::parameter::Type::kPosition, qApp->translate("EtcCsPersEditBridge", "Position")},
    {csprofile::parameter::Type::kBeam, qApp->translate("EtcCsPersEditBridge", "Beam")},
    {csprofile::parameter::Type::kColor, qApp->translate("EtcCsPersEditBridge", "Color")},
};

QString EtcCsPersEditBridge::GetParameterTypeName(csprofile::parameter::Type type) {
  return kParameterTypeNames.at(type);
}

std::shared_ptr<cslibs::disc::DiscDb> EtcCsPersEditBridge::GetDiscDb() {
  return GetImageDb<cslibs::disc::DiscDb>(GetCsEditDiscsPath(), GetDbPath("disc.db"));
}

std::shared_ptr<cslibs::effect::EffectDb> EtcCsPersEditBridge::GetEffectDb() {
  return GetImageDb<cslibs::effect::EffectDb>(GetCsEditEffectsPath(), GetDbPath("effect.db"));
}

std::shared_ptr<cslibs::gel::GelDb> EtcCsPersEditBridge::GetGelDb() {
  return GetDb<cslibs::gel::GelDb>(GetCsEditGelsPath(), GetDbPath("gel.db"));
}

std::shared_ptr<cslibs::gobo::GoboDb> EtcCsPersEditBridge::GetGoboDb() {
  return GetImageDb<cslibs::gobo::GoboDb>(GetCsEditGobosPath(), GetDbPath("gobo.db"));
}

std::optional<QString> EtcCsPersEditBridge::GetFirstExistentPath(const QDir &root, const QStringList &paths) {
  for (const auto &path : paths) {
    if (root.exists(path)) {
      return root.absoluteFilePath(path);
    }
  }
  return {};
}

QString EtcCsPersEditBridge::GetDbPath(const QString &filename) {
  const QDir db_path(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
  std::filesystem::create_directories(db_path.absolutePath().toStdString());

  return db_path.absoluteFilePath(filename);
}

} // csprofileeditor
