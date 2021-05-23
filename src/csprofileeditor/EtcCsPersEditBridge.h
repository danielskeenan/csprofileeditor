/**
 * @file EtcCsPersEditBridge.h
 *
 * @author dankeenan
 * @date 4/27/21
 * @copyright (c) 2021 Dan Keenan
 */

#ifndef CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_ETCCSPERSEDITBRIDGE_H_
#define CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_ETCCSPERSEDITBRIDGE_H_

#include <QDir>
#include <memory>
#include <cslibs/Db.h>
#include <cslibs/except.h>
#include <csprofile/logging.h>
#include <sqlite3.h>

namespace csprofile::parameter {
enum class Type;
} // csprofile::parameter

namespace cslibs {

namespace disc {
class DiscDb;
} // disc

namespace effect {
class EffectDb;
} // effect

namespace gel {
class GelDb;
} // gel

namespace gobo {
class GoboDb;
} // gobo

} // cslibs

namespace csprofileeditor {

/**
 * Handle interfacing with the official editor
 */
class EtcCsPersEditBridge {
 public:
  [[nodiscard]] static std::optional<QString> FindInstallationDirectory();
  [[nodiscard]] static bool ValidateInstallationDirectory();
  [[nodiscard]] static bool ValidateInstallationDirectory(const QDir &path);

  [[nodiscard]] static std::optional<QString> GetCsEditImagesDataPath();
  [[nodiscard]] static std::optional<QString> GetCsEditImagesDataPath(const QDir &root);
  [[nodiscard]] static std::optional<QString> GetCsEditImagesIndexPath();
  [[nodiscard]] static std::optional<QString> GetCsEditImagesIndexPath(const QDir &root);
  [[nodiscard]] static std::optional<QString> GetCsEditDiscsPath();
  [[nodiscard]] static std::optional<QString> GetCsEditDiscsPath(const QDir &root);
  [[nodiscard]] static std::optional<QString> GetCsEditEffectsPath();
  [[nodiscard]] static std::optional<QString> GetCsEditEffectsPath(const QDir &root);
  [[nodiscard]] static std::optional<QString> GetCsEditGelsPath();
  [[nodiscard]] static std::optional<QString> GetCsEditGelsPath(const QDir &root);
  [[nodiscard]] static std::optional<QString> GetCsEditGobosPath();
  [[nodiscard]] static std::optional<QString> GetCsEditGobosPath(const QDir &root);

  [[nodiscard]] static QString GetParameterTypeName(csprofile::parameter::Type type);

  [[nodiscard]] static std::shared_ptr<cslibs::disc::DiscDb> GetDiscDb();
  [[nodiscard]] static std::shared_ptr<cslibs::effect::EffectDb> GetEffectDb();
  [[nodiscard]] static std::shared_ptr<cslibs::gel::GelDb> GetGelDb();
  [[nodiscard]] static std::shared_ptr<cslibs::gobo::GoboDb> GetGoboDb();

 private:
  static const QStringList kImagesDataPaths;
  static const QStringList kImagesIndexPaths;
  static const QStringList kDiscsPaths;
  static const QStringList kEffectsPaths;
  static const QStringList kGelsPaths;
  static const QStringList kGobosPaths;

  [[nodiscard]] static std::optional<QString> GetFirstExistentPath(const QDir &root, const QStringList &paths);
  [[nodiscard]] static QString GetDbPath(const QString &filename);

  template<class T, typename = std::enable_if_t<std::is_base_of_v<cslibs::ImageDb, T>>>
  static std::shared_ptr<T> GetImageDb(const std::optional<QString> &defs_path, const QString &db_path) {
    static std::shared_ptr<T> db;
    if (!db) {
      const auto data_index_path = GetCsEditImagesIndexPath();
      const auto data_path = GetCsEditImagesDataPath();
      if (!defs_path || !data_index_path || !data_path) {
        return {};
      }
      try {
        db = std::make_shared<T>(defs_path->toStdString(),
                                 data_index_path->toStdString(),
                                 data_path->toStdString(),
                                 db_path.toStdString(),
                                 true);
      } catch (const cslibs::except::DefsError &e) {
        csprofile::logging::warn("Failed to get db: {}", e.what());
        return {};
      } catch (const cslibs::except::DbError &e) {
        csprofile::logging::warn("Failed to get db: {}", e.what());
        return {};
      } catch (const SQLite::Exception &e) {
        if (e.getErrorCode() != SQLITE_CANTOPEN) {
          csprofile::logging::warn("Database error: {}", e.what());
        }
        return {};
      }
    }

    return db;
  }

  template<class T, typename = std::enable_if_t<std::is_base_of_v<cslibs::Db, T>>>
  static std::shared_ptr<T> GetDb(const std::optional<QString> &defs_path, const QString &db_path) {
    static std::shared_ptr<T> db;
    if (!db) {
      if (!defs_path) {
        return {};
      }
      try {
        db = std::make_shared<T>(defs_path->toStdString(),
                                 db_path.toStdString(),
                                 true);
      } catch (const cslibs::except::DefsError &e) {
        csprofile::logging::warn("Failed to get db: {}", e.what());
        return {};
      } catch (const cslibs::except::DbError &e) {
        csprofile::logging::warn("Failed to get db: {}", e.what());
        return {};
      } catch (const SQLite::Exception &e) {
        if (e.getErrorCode() != SQLITE_CANTOPEN) {
          csprofile::logging::warn("Database error: {}", e.what());
        }
        return {};
      }
    }

    return db;
  }
};

} // csprofileeditor

#endif //CSPROFILEEDITOR_SRC_CSPROFILEEDITOR_ETCCSPERSEDITBRIDGE_H_
